#include "driver_dht22.h"

// 【造出实物】绑定到 PA10 引脚 (对应 PORT_10_PIN_10)
Dht22Device_t Dht22 = { .pin = BSP_IO_PORT_10_PIN_10 };

// ---------------- 内部厨房操作，不对外公开 (static) ----------------
static void Dht22_DelayUs(uint32_t us) {
    R_BSP_SoftwareDelay(us, BSP_DELAY_UNITS_MICROSECONDS);
}

static void Dht22_SetPinOutLow(Dht22Device_t *dev) {
    R_IOPORT_PinCfg(&g_ioport_ctrl, dev->pin, (uint32_t)(IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_DRIVE_HIGH));
    R_IOPORT_PinWrite(&g_ioport_ctrl, dev->pin, BSP_IO_LEVEL_LOW);
}

static void Dht22_SetPinIn(Dht22Device_t *dev) {
    R_IOPORT_PinCfg(&g_ioport_ctrl, dev->pin, (uint32_t)(IOPORT_CFG_PORT_DIRECTION_INPUT));
}

static bsp_io_level_t Dht22_ReadPin(Dht22Device_t *dev) {
    bsp_io_level_t level = BSP_IO_LEVEL_HIGH;
    R_IOPORT_PinRead(&g_ioport_ctrl, dev->pin, &level);
    return level;
}

// ---------------- 对外公开的方法 ----------------
bool Dht22_Read(Dht22Device_t *dev, float *humidity, float *temperature) {
    uint8_t data[5] = {0}; 
    
    Dht22_SetPinOutLow(dev);
    Dht22_DelayUs(1000); 
    Dht22_SetPinIn(dev);
    Dht22_DelayUs(40);   
    
    if (Dht22_ReadPin(dev) != BSP_IO_LEVEL_LOW) return false;
    while (Dht22_ReadPin(dev) == BSP_IO_LEVEL_LOW);  
    while (Dht22_ReadPin(dev) == BSP_IO_LEVEL_HIGH); 
    
    for (int i = 0; i < 5; i++) {
        uint8_t byte = 0;
        for (int b = 0; b < 8; b++) {
            byte <<= 1;
            while (Dht22_ReadPin(dev) == BSP_IO_LEVEL_HIGH); 
            while (Dht22_ReadPin(dev) == BSP_IO_LEVEL_LOW);  
            Dht22_DelayUs(40);
            if (Dht22_ReadPin(dev) == BSP_IO_LEVEL_HIGH) {
                byte |= 1;
                while (Dht22_ReadPin(dev) == BSP_IO_LEVEL_HIGH); 
            }
        }
        data[i] = byte;
    }
    
    uint8_t check_sum = (uint8_t)(data[0] + data[1] + data[2] + data[3]);
    if (check_sum != data[4]) return false;
    
    *humidity = (float)(((uint16_t)data[0] << 8) | data[1]) / 10.0f;
    uint16_t temp_raw = (uint16_t)(((uint16_t)data[2] << 8) | data[3]);
    *temperature = (temp_raw & 0x8000) ? -(float)((temp_raw & 0x7FFF)/10.0f) : (float)(temp_raw/10.0f);
    
    return true;
}