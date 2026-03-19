#ifndef DRIVER_BH1750_H
#define DRIVER_BH1750_H

#include "hal_data.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    i2c_master_instance_t const * p_i2c;   // 例如：&g_i2c5
    uint8_t addr_7bit;                    // BH1750: ADDR=GND -> 0x23, ADDR=VCC -> 0x5C
    bool initialized;
} Bh1750_t;

/**
 * @brief 初始化 BH1750（上电+复位+设置连续高分辨率模式 1lx）
 * @param dev 设备对象
 * @param p_i2c FSP I2C instance 指针（例如 &g_i2c5）
 * @param addr_7bit 7位地址（ADDR=GND -> 0x23）
 */
fsp_err_t Bh1750_Init(Bh1750_t * dev, i2c_master_instance_t const * p_i2c, uint8_t addr_7bit);

/**
 * @brief 读取当前光照（lux）。内部会读取2字节 raw 并按 raw/1.2 转换。
 * @param dev 设备对象
 * @param lux_out 输出 lux（用 float）
 */
fsp_err_t Bh1750_ReadLux(Bh1750_t * dev, float * lux_out);

/**
 * @brief 可选：让 BH1750 进入 power down（省电）
 */
fsp_err_t Bh1750_PowerDown(Bh1750_t * dev);

#ifdef __cplusplus
}
#endif

#endif // DRIVER_BH1750_H