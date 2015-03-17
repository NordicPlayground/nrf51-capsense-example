/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup blinky_example_main main.c
 * @{
 * @ingroup blinky_example
 * @brief Blinky Example Application main file.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "boards.h"
#include "nrf_uart_int.h"
#include "nrf_capsense.h"

#define CAPSENSE_OUT        14
#define CAPSENSE_IN         1
#define CAPSENSE_IN_ANA     ANA_AIN2_P01

#define CAPSENSE_OUT2       6
#define CAPSENSE_IN2        2
#define CAPSENSE_IN_ANA2    ANA_AIN3_P02

static void uart_init(void)
{
    uart_config_t uart_config = {.pin_txd = 9, .pin_rxd = 11, .pin_rts = 8, .pin_cts = 10, .baudrate = UART_BAUDRATE_38400, .hwfc = 1, .parity = 0};
    nrf_uart_init(&uart_config);
}

static void capsense_init(void)
{
    capsense_config_t capsense_config[] = {{CAPSENSE_IN_ANA, CAPSENSE_OUT}, {CAPSENSE_IN_ANA2, CAPSENSE_OUT2}};
    static capsense_channel_t m_capsense_array[2];                 
    nrf_capsense_init(m_capsense_array, capsense_config, 2);  
    //NRF_CAPSENSE_INIT(capsense_config, 2);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    nrf_gpio_range_cfg_output(21,24);
    
    capsense_init();
 
    uart_init();
    
    printf("\r\n\nCapSense Test Application\r\n");
    printf("\r\nVal\tAvg\tMin\tMax\r\n");
 
    while (true)
    { 
        capsense_channel_t *capsense_channels;
        
        nrf_delay_ms(12);

        capsense_channels = nrf_capsense_sample();

        printf("\r%.1f   \t%.1f   \t%.1f  \t%.1f  ", (float)capsense_channels[0].value / 16.0f, (float)(capsense_channels[0].average) / 16.0f, (float)capsense_channels[0].val_min / 16.0f, (float)capsense_channels[0].val_max / 16.0f);

        nrf_gpio_pin_write(21, (capsense_channels[0].pressed ? 0 : 1));
        nrf_gpio_pin_write(22, (capsense_channels[0].pressed ? 0 : 1));
        nrf_gpio_pin_write(23, (capsense_channels[1].pressed ? 0 : 1));
        nrf_gpio_pin_write(24, (capsense_channels[1].pressed ? 0 : 1));
    }
}


/** @} */
