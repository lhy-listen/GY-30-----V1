#ifndef DRIVER_UART_H
#define DRIVER_UART_H

#include "hal_data.h"
#include <stdbool.h>
#include <stdint.h>

// 【图纸】串口设备类
typedef struct {
    uart_ctrl_t      * p_ctrl;      
    uart_cfg_t const * p_cfg;       
    volatile bool      tx_complete; 
    volatile bool      rx_complete; 
    volatile uint8_t   rx_byte;     
} UartDevice_t;

// 【宣告对象存在】
extern UartDevice_t UartSensor;  // SCI2
extern UartDevice_t UartHmi;     // SCI4
extern UartDevice_t UartPc;      // SCI7
extern UartDevice_t UartFace;    // SCI9
extern UartDevice_t Uart8;       // 【新增】宣告：新业务员 Uart8 上线！

// 【宣告方法】
void Uart_Init(UartDevice_t *dev);
void Uart_StartRx(UartDevice_t *dev);
void Uart_Send(UartDevice_t *dev, uint8_t *data, uint32_t len);
void Uart_Printf(UartDevice_t *dev, const char *format, ...);

#endif // DRIVER_UART_H