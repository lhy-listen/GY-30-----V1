/* generated vector source file - do not edit */
        #include "bsp_api.h"
        /* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
        #if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = sci_uart_rxi_isr, /* SCI2 RXI (Receive data full) */
            [1] = sci_uart_txi_isr, /* SCI2 TXI (Transmit data empty) */
            [2] = sci_uart_tei_isr, /* SCI2 TEI (Transmit end) */
            [3] = sci_uart_eri_isr, /* SCI2 ERI (Receive error) */
            [4] = sci_uart_rxi_isr, /* SCI4 RXI (Receive data full) */
            [5] = sci_uart_txi_isr, /* SCI4 TXI (Transmit data empty) */
            [6] = sci_uart_tei_isr, /* SCI4 TEI (Transmit end) */
            [7] = sci_uart_eri_isr, /* SCI4 ERI (Receive error) */
            [8] = sci_uart_rxi_isr, /* SCI7 RXI (Receive data full) */
            [9] = sci_uart_txi_isr, /* SCI7 TXI (Transmit data empty) */
            [10] = sci_uart_tei_isr, /* SCI7 TEI (Transmit end) */
            [11] = sci_uart_eri_isr, /* SCI7 ERI (Receive error) */
            [12] = sci_uart_rxi_isr, /* SCI9 RXI (Receive data full) */
            [13] = sci_uart_txi_isr, /* SCI9 TXI (Transmit data empty) */
            [14] = sci_uart_tei_isr, /* SCI9 TEI (Transmit end) */
            [15] = sci_uart_eri_isr, /* SCI9 ERI (Receive error) */
            [16] = sci_uart_rxi_isr, /* SCI8 RXI (Receive data full) */
            [17] = sci_uart_txi_isr, /* SCI8 TXI (Transmit data empty) */
            [18] = sci_uart_tei_isr, /* SCI8 TEI (Transmit end) */
            [19] = sci_uart_eri_isr, /* SCI8 ERI (Receive error) */
            [20] = sci_i2c_txi_isr, /* SCI5 TXI (Transmit data empty) */
            [21] = sci_i2c_tei_isr, /* SCI5 TEI (Transmit end) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_SCI2_RXI,GROUP0), /* SCI2 RXI (Receive data full) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_SCI2_TXI,GROUP1), /* SCI2 TXI (Transmit data empty) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_SCI2_TEI,GROUP2), /* SCI2 TEI (Transmit end) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_SCI2_ERI,GROUP3), /* SCI2 ERI (Receive error) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_SCI4_RXI,GROUP4), /* SCI4 RXI (Receive data full) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_SCI4_TXI,GROUP5), /* SCI4 TXI (Transmit data empty) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_SCI4_TEI,GROUP6), /* SCI4 TEI (Transmit end) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_SCI4_ERI,GROUP7), /* SCI4 ERI (Receive error) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_SCI7_RXI,GROUP0), /* SCI7 RXI (Receive data full) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_SCI7_TXI,GROUP1), /* SCI7 TXI (Transmit data empty) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_SCI7_TEI,GROUP2), /* SCI7 TEI (Transmit end) */
            [11] = BSP_PRV_VECT_ENUM(EVENT_SCI7_ERI,GROUP3), /* SCI7 ERI (Receive error) */
            [12] = BSP_PRV_VECT_ENUM(EVENT_SCI9_RXI,GROUP4), /* SCI9 RXI (Receive data full) */
            [13] = BSP_PRV_VECT_ENUM(EVENT_SCI9_TXI,GROUP5), /* SCI9 TXI (Transmit data empty) */
            [14] = BSP_PRV_VECT_ENUM(EVENT_SCI9_TEI,GROUP6), /* SCI9 TEI (Transmit end) */
            [15] = BSP_PRV_VECT_ENUM(EVENT_SCI9_ERI,GROUP7), /* SCI9 ERI (Receive error) */
            [16] = BSP_PRV_VECT_ENUM(EVENT_SCI8_RXI,GROUP0), /* SCI8 RXI (Receive data full) */
            [17] = BSP_PRV_VECT_ENUM(EVENT_SCI8_TXI,GROUP1), /* SCI8 TXI (Transmit data empty) */
            [18] = BSP_PRV_VECT_ENUM(EVENT_SCI8_TEI,GROUP2), /* SCI8 TEI (Transmit end) */
            [19] = BSP_PRV_VECT_ENUM(EVENT_SCI8_ERI,GROUP3), /* SCI8 ERI (Receive error) */
            [20] = BSP_PRV_VECT_ENUM(EVENT_SCI5_TXI,GROUP4), /* SCI5 TXI (Transmit data empty) */
            [21] = BSP_PRV_VECT_ENUM(EVENT_SCI5_TEI,GROUP5), /* SCI5 TEI (Transmit end) */
        };
        #endif
        #endif