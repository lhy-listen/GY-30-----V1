/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_sci_i2c.h"
#include "r_i2c_master_api.h"
#include "r_sci_uart.h"
            #include "r_uart_api.h"
FSP_HEADER
extern const i2c_master_cfg_t g_i2c5_cfg;
/* I2C on SCI Instance. */
extern const i2c_master_instance_t g_i2c5;
#ifndef sci_i2c_master_callback
void sci_i2c_master_callback(i2c_master_callback_args_t * p_args);
#endif

extern const sci_i2c_extended_cfg_t g_i2c5_cfg_extend;
extern sci_i2c_instance_ctrl_t g_i2c5_ctrl;
/** UART on SCI Instance. */
            extern const uart_instance_t      g_uart8;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_uart_instance_ctrl_t     g_uart8_ctrl;
            extern const uart_cfg_t g_uart8_cfg;
            extern const sci_uart_extended_cfg_t g_uart8_cfg_extend;

            #ifndef uart8_callback
            void uart8_callback(uart_callback_args_t * p_args);
            #endif
/** UART on SCI Instance. */
            extern const uart_instance_t      g_uart9;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_uart_instance_ctrl_t     g_uart9_ctrl;
            extern const uart_cfg_t g_uart9_cfg;
            extern const sci_uart_extended_cfg_t g_uart9_cfg_extend;

            #ifndef uart9_callback
            void uart9_callback(uart_callback_args_t * p_args);
            #endif
/** UART on SCI Instance. */
            extern const uart_instance_t      g_uart7;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_uart_instance_ctrl_t     g_uart7_ctrl;
            extern const uart_cfg_t g_uart7_cfg;
            extern const sci_uart_extended_cfg_t g_uart7_cfg_extend;

            #ifndef uart7_callback
            void uart7_callback(uart_callback_args_t * p_args);
            #endif
/** UART on SCI Instance. */
            extern const uart_instance_t      g_uart4;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_uart_instance_ctrl_t     g_uart4_ctrl;
            extern const uart_cfg_t g_uart4_cfg;
            extern const sci_uart_extended_cfg_t g_uart4_cfg_extend;

            #ifndef uart4_callback
            void uart4_callback(uart_callback_args_t * p_args);
            #endif
/** UART on SCI Instance. */
            extern const uart_instance_t      g_uart2;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_uart_instance_ctrl_t     g_uart2_ctrl;
            extern const uart_cfg_t g_uart2_cfg;
            extern const sci_uart_extended_cfg_t g_uart2_cfg_extend;

            #ifndef uart2_callback
            void uart2_callback(uart_callback_args_t * p_args);
            #endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
