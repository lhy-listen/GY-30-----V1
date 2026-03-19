#include "hal_data.h"
#include <stdio.h>
#include <string.h>

#include "driver_uart.h"
#include "driver_dht22.h"
#include "driver_bh1750.h"

/* ====================================================================================
 * 【0】BH1750（GY-30）对象
 * ==================================================================================== */
static Bh1750_t g_bh1750;
/* ====================================================================================
 * 【I2C】FSP I2C 通信完成与异常标志位
 * ==================================================================================== */
volatile bool g_i2c_completed = false;
volatile bool g_i2c_aborted   = false; // 新增：中止标志位

void sci_i2c_master_callback(i2c_master_callback_args_t * p_args)
{
    if (p_args->event == I2C_MASTER_EVENT_TX_COMPLETE ||
        p_args->event == I2C_MASTER_EVENT_RX_COMPLETE)
    {
        g_i2c_aborted = false;
        g_i2c_completed = true;
    }
    else if (p_args->event == I2C_MASTER_EVENT_ABORTED)
    {
        g_i2c_aborted = true; // 记录通信失败
        g_i2c_completed = true; // 放行死循环
    }
}
/* ====================================================================================
 * 【1】硬件中断接线员
 * ==================================================================================== */
void uart2_callback(uart_callback_args_t * p_args) {
    if (p_args->event == UART_EVENT_RX_COMPLETE) UartSensor.rx_complete = true;
}
void uart4_callback(uart_callback_args_t * p_args) {
    if (p_args->event == UART_EVENT_RX_COMPLETE) UartHmi.rx_complete = true;
    else if (p_args->event == UART_EVENT_TX_COMPLETE) UartHmi.tx_complete = true;
}
void uart7_callback(uart_callback_args_t * p_args) {
    if (p_args->event == UART_EVENT_TX_COMPLETE) UartPc.tx_complete = true;
}
void uart9_callback(uart_callback_args_t * p_args) {
    if (p_args->event == UART_EVENT_RX_COMPLETE) UartFace.rx_complete = true;
    else if (p_args->event == UART_EVENT_TX_COMPLETE) UartFace.tx_complete = true;
}
void uart8_callback(uart_callback_args_t * p_args) {
    if (p_args->event == UART_EVENT_RX_COMPLETE) Uart8.rx_complete = true;
    else if (p_args->event == UART_EVENT_TX_COMPLETE) Uart8.tx_complete = true;
}

/* ====================================================================================
 * 【2】公共业务助手
 * ==================================================================================== */
static void Hmi_SendCmd(const char *cmd_str) {
    uint8_t buffer[128];
    int len = snprintf((char *)buffer, sizeof(buffer), "%s", cmd_str);
    buffer[len++] = 0xFF; buffer[len++] = 0xFF; buffer[len++] = 0xFF;
    Uart_Send(&UartHmi, buffer, (uint32_t)len);
}

/* ====================================================================================
 * 【3】系统任务区
 * ==================================================================================== */

// --- 任务 1：温湿度模块 ---
static void Task_DHT22(void) {
    static uint32_t dht22_read_tick = 0;

    dht22_read_tick++;
    if (dht22_read_tick >= 15000000) {
        float hum, temp;
        if (Dht22_Read(&Dht22, &hum, &temp)) {
            int temp_int = (int) temp; int temp_dec = (int) (temp * 10) % 10;
            int hum_int  = (int) hum;  int hum_dec  = (int) (hum * 10) % 10;

            Uart_Printf(&UartPc, "[DHT22] Temp: %d.%d C, Hum: %d.%d %%RH\r\n", temp_int, temp_dec, hum_int, hum_dec);

            char cmd[64];
            snprintf(cmd, sizeof(cmd), "t4.txt=\"%d.%d\"", temp_int, temp_dec);
            Hmi_SendCmd(cmd);

            R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

            snprintf(cmd, sizeof(cmd), "t5.txt=\"%d.%d\"", hum_int, hum_dec);
            Hmi_SendCmd(cmd);
        } else {
            Uart_Printf(&UartPc, "[DHT22] Read Failed!\r\n");
        }
        dht22_read_tick = 0;
    }
}

// --- 任务 2：CO2 传感器模块 ---
static void Task_CO2(void) {
#define FRAME_LENGTH 16
    static uint8_t rx_frame[FRAME_LENGTH];
    static uint8_t rx_index = 0;

    static uint16_t low_co2_count = 0;
    static uint8_t add_rest_count = 0;
    static uint16_t add_send_count = 0;

    if (UartSensor.rx_complete) {
        UartSensor.rx_complete = false;
        uint8_t c = UartSensor.rx_byte;

        if (rx_index == 0) {
            if (c == 0xFE) rx_frame[rx_index++] = c;
        } else {
            rx_frame[rx_index++] = c;
            if (rx_index >= FRAME_LENGTH) {
                uint16_t co2_val = (uint16_t)((((uint16_t)rx_frame[7] << 8) | rx_frame[8]) / 25);

                if (co2_val < (1000 / 25)) {
                    low_co2_count++;
                    if (low_co2_count >= 50) {
                        Hmi_SendCmd("page box");
                        low_co2_count = 0;
                    }
                } else { low_co2_count = 0; }

                char cmd_core[32];
                snprintf(cmd_core, sizeof(cmd_core), "add 1,1,%d", co2_val);
                Hmi_SendCmd(cmd_core);

                add_rest_count++;
                if (add_rest_count >= 50) {
                    R_BSP_SoftwareDelay(20, BSP_DELAY_UNITS_MILLISECONDS);
                    add_rest_count = 0;
                }
                if (++add_send_count >= 300) {
                    Hmi_SendCmd("cle 1,1");
                    add_send_count = 0;
                }
                rx_index = 0;
            }
        }
        Uart_StartRx(&UartSensor);
    }
}

// --- 任务 3：HMI 屏幕指令处理 ---
typedef enum {
    FACE_IDLE = 0,
    FACE_WAIT_RESET,
} FaceWorkState_e;

static FaceWorkState_e g_face_state = FACE_IDLE;
static uint32_t g_face_delay_tick = 0;

static void Task_HMI(void) {
#define HMI_RX_MAX_LEN 64
    static uint8_t hmi_rx_frame[HMI_RX_MAX_LEN];
    static uint16_t hmi_rx_idx = 0;
    static uint32_t hmi_rx_idle_tick = 0;

    if (UartHmi.rx_complete) {
        UartHmi.rx_complete = false;
        uint8_t c = UartHmi.rx_byte;

        if (c >= 0x20 && c <= 0x7E) {
            if (hmi_rx_idx == 0 && c == 'p') {
                // 丢弃第一个 p
            } else if (hmi_rx_idx < (HMI_RX_MAX_LEN - 1)) {
                hmi_rx_frame[hmi_rx_idx++] = c;
            }
        }

        hmi_rx_idle_tick = 0;
        Uart_StartRx(&UartHmi);
    }

    if (hmi_rx_idx > 0) {
        hmi_rx_idle_tick++;
        if (hmi_rx_idle_tick > 50000) {
            hmi_rx_frame[hmi_rx_idx] = '\0';
            Uart_Printf(&UartPc, "[Status] Screen CMD: %s\r\n", hmi_rx_frame);

            if (strcmp((char *) hmi_rx_frame, "START") == 0) {
                Uart_Printf(&UartPc, "[Status] Resetting Face Module...\r\n");
                static const uint8_t reset_cmd[] = {0xEF, 0xAA, 0x10, 0x00, 0x00, 0x10};
                Uart_Send(&UartFace, (uint8_t *) reset_cmd, sizeof(reset_cmd));

                g_face_state = FACE_WAIT_RESET;
                g_face_delay_tick = 0;
            }
            else if (strcmp((char *) hmi_rx_frame, "zhuce") == 0) {
                Uart_Printf(&UartPc, "[Status] Enroll(Face Register) CMD Sent!\r\n");
                static const uint8_t enroll_cmd[] = {0xEF, 0xAA, 0x1D, 0x00, 0x00, 0x1D};
                Uart_Send(&UartFace, (uint8_t *) enroll_cmd, sizeof(enroll_cmd));
            }

            hmi_rx_idx = 0;
            hmi_rx_idle_tick = 0;
        }
    }
}

// --- 任务 4：人脸识别交互模块 ---
static void Task_Face(void) {
#define SCI9_RX_MAX_LEN 256
    static uint8_t sci9_rx_frame[SCI9_RX_MAX_LEN];
    static uint16_t sci9_rx_idx = 0;
    static uint32_t sci9_rx_idle_tick = 0;

    if (g_face_state == FACE_WAIT_RESET) {
        g_face_delay_tick++;
        if (g_face_delay_tick >= 2000000) {
            Uart_Printf(&UartPc, "[Status] Face Recognition Started!\r\n");
            static const uint8_t reco_cmd[] = {0xEF, 0xAA, 0x12, 0x00, 0x02, 0x00, 0x05, 0x15};
            Uart_Send(&UartFace, (uint8_t *) reco_cmd, sizeof(reco_cmd));
            g_face_state = FACE_IDLE;
        }
    }

    if (UartFace.rx_complete) {
        UartFace.rx_complete = false;
        if (sci9_rx_idx < SCI9_RX_MAX_LEN) {
            sci9_rx_frame[sci9_rx_idx++] = UartFace.rx_byte;
        }
        sci9_rx_idle_tick = 0;
        Uart_StartRx(&UartFace);
    }

    if (sci9_rx_idx > 0) {
        sci9_rx_idle_tick++;
        if (sci9_rx_idle_tick > 100000) {
            for (int i = 0; i <= (int)sci9_rx_idx - 9; i++) {
                if (sci9_rx_frame[i] == 0xEF && sci9_rx_frame[i + 1] == 0xAA) {
                    uint8_t cmd_type = sci9_rx_frame[i + 5];

                    if (cmd_type == 0x12) {
                        uint8_t status = sci9_rx_frame[i + 6];
                        uint16_t user_id = (uint16_t)((((uint16_t)sci9_rx_frame[i + 7] << 8) | sci9_rx_frame[i + 8]));

                        if (status == 0x00) {
                            Uart_Printf(&UartPc, "[Face] Verify Success! User ID: %d\r\n", user_id);
                            Hmi_SendCmd("t9.txt=\"识别成功！\"");
                        } else {
                            Hmi_SendCmd("t9.txt=\"识别失败！\"");
                            Uart_Printf(&UartPc, "[Face] Verify Failed: Status 0x%02X\r\n", status);
                        }
                        break;
                    }
                    else if (cmd_type == 0x1D) {
                        uint8_t status = sci9_rx_frame[i + 6];
                        uint16_t face_id = (uint16_t)((((uint16_t)sci9_rx_frame[i + 7] << 8) | sci9_rx_frame[i + 8]));

                        if (status == 0x00) {
                            Uart_Printf(&UartPc, "[Face] Enroll Success! Face_ID: %d\r\n", face_id);
                            Hmi_SendCmd("t10.txt=\"注册成功！\"");
                        } else {
                            Uart_Printf(&UartPc, "[Face] Enroll Failed! Status=0x%02X\r\n", status);
                            Hmi_SendCmd("t10.txt=\"注册失败！\"");
                        }
                        break;
                    }
                }
            }

            sci9_rx_idx = 0;
            sci9_rx_idle_tick = 0;
        }
    }
}

// --- 任务 5：Uart8 接收透传任务 ---
static void Task_Uart8(void) {
#define UART8_RX_MAX_LEN 512
    static uint8_t rx_frame[UART8_RX_MAX_LEN];
    static uint16_t rx_idx = 0;
    static uint32_t rx_idle_tick = 0;

    if (Uart8.rx_complete) {
        Uart8.rx_complete = false;
        if (rx_idx < UART8_RX_MAX_LEN) {
            rx_frame[rx_idx++] = Uart8.rx_byte;
        }
        rx_idle_tick = 0;
        Uart_StartRx(&Uart8);
    }

    if (rx_idx > 0) {
        rx_idle_tick++;
        if (rx_idle_tick > 50000) {
            Uart_Printf(&UartPc, "\r\n--- [UART8 RX %d bytes] ---\r\n", rx_idx);
            Uart_Send(&UartPc, rx_frame, rx_idx);
            Uart_Printf(&UartPc, "\r\n--------------------------\r\n");
            rx_idx = 0;
            rx_idle_tick = 0;
        }
    }
}

// --- 任务 6：【新增】GY-30(BH1750) 光照读取并通过SCI7输出 ---
static void Task_Light(void)
{
    /* 简单节拍：用 while(1) 自增计数实现（与你现有风格一致） */
    static uint32_t light_tick = 0;
    light_tick++;

    if (light_tick >= 6000000)   // 你可以调这个数值，控制打印频率
    {
        float lux = 0.0f;
        fsp_err_t err = Bh1750_ReadLux(&g_bh1750, &lux);
        if (FSP_SUCCESS == err)
        {
            Uart_Printf(&UartPc, "[BH1750] Lux: %.2f lx\r\n", lux);
        }
        else
        {
            Uart_Printf(&UartPc, "[BH1750] Read failed, err=%d\r\n", (int)err);
        }
        light_tick = 0;
    }
}

/* ====================================================================================
 * 【4】大老板总办公室
 * ==================================================================================== */
void hal_entry(void)
{
    /* 1) 初始化 UART */
    Uart_Init(&UartSensor);
    Uart_Init(&UartHmi);
    Uart_Init(&UartPc);
    Uart_Init(&UartFace);
    Uart_Init(&Uart8);

    Uart_StartRx(&UartSensor);
    Uart_StartRx(&UartHmi);
    Uart_StartRx(&UartFace);
    Uart_StartRx(&Uart8);

    /* 2) 初始化 I2C（必须 open） */
    fsp_err_t err = g_i2c5.p_api->open(g_i2c5.p_ctrl, g_i2c5.p_cfg);
    if (FSP_SUCCESS != err)
    {
        Uart_Printf(&UartPc, "[I2C] open failed, err=%d\r\n", (int)err);
    }
    else
    {
        /* 3) 初始化 BH1750：ADDR=GND -> 0x23 */
        err = Bh1750_Init(&g_bh1750, &g_i2c5, 0x23);
        if (FSP_SUCCESS != err)
        {
            Uart_Printf(&UartPc, "[BH1750] init failed, err=%d\r\n", (int)err);
        }
        else
        {
            Uart_Printf(&UartPc, "[BH1750] init OK (addr=0x23)\r\n");
            /* 给第一次测量一点时间（高分辨率典型120ms） */
            R_BSP_SoftwareDelay(180, BSP_DELAY_UNITS_MILLISECONDS);
        }
    }

    Uart_Printf(&UartPc, "\r\n=== System Boot OK (Added BH1750)! ===\r\n");
    R_BSP_SoftwareDelay(2000, BSP_DELAY_UNITS_MILLISECONDS);

    while (1)
    {
        Task_DHT22();
        Task_CO2();
        Task_HMI();
        Task_Face();
        Task_Uart8();

        /* 新增光照任务 */
        Task_Light();
    }
}