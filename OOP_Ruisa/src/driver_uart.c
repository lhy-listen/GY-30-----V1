#include "driver_uart.h"
#include <stdio.h>
#include <stdarg.h>

// 【造出实物】把图纸变成真正的全局变量
UartDevice_t UartSensor = { .p_ctrl = &g_uart2_ctrl, .p_cfg = &g_uart2_cfg };
UartDevice_t UartHmi    = { .p_ctrl = &g_uart4_ctrl, .p_cfg = &g_uart4_cfg };
UartDevice_t UartPc     = { .p_ctrl = &g_uart7_ctrl, .p_cfg = &g_uart7_cfg };
UartDevice_t UartFace   = { .p_ctrl = &g_uart9_ctrl, .p_cfg = &g_uart9_cfg };
UartDevice_t Uart8      = { .p_ctrl = &g_uart8_ctrl, .p_cfg = &g_uart8_cfg }; // 【新增】造出 Uart8 实体！

// ---------------- 下面的方法完全不用改！OO编程就是这么爽！ ----------------

void Uart_Init(UartDevice_t *dev) {
    R_SCI_UART_Open(dev->p_ctrl, dev->p_cfg);
}

void Uart_StartRx(UartDevice_t *dev) {
    R_SCI_UART_Read(dev->p_ctrl, (uint8_t *)&dev->rx_byte, 1);
}

void Uart_Send(UartDevice_t *dev, uint8_t *data, uint32_t len) {
    if (data == NULL || len == 0) return;
    
    dev->tx_complete = false; 
    fsp_err_t err = R_SCI_UART_Write(dev->p_ctrl, data, len); 
    
    if (err == FSP_SUCCESS) {
        while (!dev->tx_complete) {} 
    }
}

void Uart_Printf(UartDevice_t *dev, const char *format, ...) {
    static char buffer[256]; 
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    if (len > 0) { 
        Uart_Send(dev, (uint8_t *)buffer, (uint32_t)len); 
    }
}