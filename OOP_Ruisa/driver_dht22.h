#ifndef DRIVER_DHT22_H
#define DRIVER_DHT22_H

#include "hal_data.h"
#include <stdbool.h>

// 【图纸】温湿度传感器类
typedef struct {
    bsp_io_port_pin_t  pin;         
} Dht22Device_t;

// 【宣告对象存在】
extern Dht22Device_t Dht22;

// 【宣告方法】
bool Dht22_Read(Dht22Device_t *dev, float *humidity, float *temperature);

#endif // DRIVER_DHT22_H