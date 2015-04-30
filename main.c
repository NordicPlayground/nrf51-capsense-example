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

#define NUM_CHANNELS 4

static void uart_init(void)
{
    uart_config_t uart_config = {.pin_txd = 9, .pin_rxd = 11, .pin_rts = 8, .pin_cts = 10, .baudrate = UART_BAUDRATE_460800, .hwfc = 1, .parity = 0};
    nrf_uart_init(&uart_config);
}

static void capsense_init(void)
{
    static capsense_channel_t m_capsense_array[NUM_CHANNELS]; 
    capsense_config_t capsense_config[] = {{ANA_AIN2_P01, 14}, 
                                           {ANA_AIN3_P02, 15},
                                           {ANA_AIN4_P03, 19},
                                           {ANA_AIN5_P04, 20}};
    nrf_capsense_init(m_capsense_array, capsense_config, NUM_CHANNELS);  
    //NRF_CAPSENSE_INIT(capsense_config, NUM_CHANNELS);
}

static void print_channel(capsense_channel_t *cap_ch)
{
    printf("\r%.1f   \t%.1f   \t%.1f  \t%.1f  ", (float)cap_ch->value / 16.0f, (float)(cap_ch->average) / 16.0f, (float)cap_ch->val_min / 16.0f, (float)cap_ch->val_max / 16.0f);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t output_counter = 0;
    nrf_gpio_range_cfg_output(21,24);
    
    capsense_init();
 
    uart_init();
    
    printf("\r\n\nCapSense Test Application\r\n");

    while (true)
    { 
        capsense_channel_t *capsense_channels;
        
        nrf_delay_ms(12);

        capsense_channels = nrf_capsense_sample();

        if(output_counter++ > 10)
        {
            printf("\r\n\nVal\tAvg\tMin\tMax\r");
            for(int i = 0; i < NUM_CHANNELS; i++) 
            {
                printf("\n");
                print_channel(&capsense_channels[i]);
            }
            output_counter = 0;
        }
        
        
        nrf_gpio_pin_write(21, (capsense_channels[0].pressed ? 0 : 1));
        nrf_gpio_pin_write(22, (capsense_channels[1].pressed ? 0 : 1));
        nrf_gpio_pin_write(23, (capsense_channels[2].pressed ? 0 : 1));
        nrf_gpio_pin_write(24, (capsense_channels[3].pressed ? 0 : 1));
    }
}


/** @} */
