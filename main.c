/* Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "boards.h"
#include "nrf_uart_int.h"
#include "nrf_capsense.h"

// Comment out this line to remove the UART output from the code
#define USE_UART

#define NUM_CHANNELS 4

static capsense_channel_t m_capsense_array[NUM_CHANNELS]; 

#ifdef USE_UART
static uint32_t output_counter = 0;

static void uart_init(void)
{
    uart_config_t uart_config = {.pin_txd = 9, .pin_rxd = 11, .pin_rts = 8, .pin_cts = 10, .baudrate = UART_BAUDRATE_460800, .hwfc = 1, .parity = 0};
    nrf_uart_init(&uart_config);
}

static void print_channel(capsense_channel_t *cap_ch)
{
    printf("\r%.1f   \t%.1f   \t%.1f  \t%.1f  ", (float)cap_ch->value / 16.0f, (float)(cap_ch->average) / 16.0f, (float)cap_ch->val_min / 16.0f, (float)cap_ch->val_max / 16.0f);
}
#endif

static void capsense_event(uint32_t pin_mask, uint32_t pin_changed_mask)
{
    //uint32_t button_pressed_mask = pin_mask & pin_changed_mask;
    //uint32_t button_released_mask = ~pin_mask & pin_changed_mask;
    
    nrf_gpio_pin_write(LED_1, ((pin_mask & 0x01) ? 0 : 1));
    nrf_gpio_pin_write(LED_2, ((pin_mask & 0x02) ? 0 : 1));
    nrf_gpio_pin_write(LED_3, ((pin_mask & 0x04) ? 0 : 1));
    nrf_gpio_pin_write(LED_4, ((pin_mask & 0x08) ? 0 : 1));
}

static void capsense_init(void)
{
    capsense_config_t capsense_config[] = {{ANA_AIN2_P01, 14}, 
                                           {ANA_AIN3_P02, 15},
                                           {ANA_AIN4_P03, 19},
                                           {ANA_AIN5_P04, 20}};
    nrf_capsense_init(m_capsense_array, capsense_config, NUM_CHANNELS); 
    nrf_capsense_set_callback(capsense_event);                                           
}

int main(void)
{
    nrf_gpio_range_cfg_output(LED_1, LED_4);
    
    capsense_init();

#ifdef USE_UART    
    uart_init();

    printf("\r\n\nCapSense Test Application\r\n");    
#endif

    while (true)
    { 
        nrf_delay_ms(12);

        nrf_capsense_sample();

#ifdef USE_UART
        if(output_counter++ > 5)
        {
            printf("\r\n\nVal\tAvg\tMin\tMax\r");
            for(int i = 0; i < NUM_CHANNELS; i++) 
            {
                printf("\n");
                print_channel(&m_capsense_array[i]);
            }
            output_counter = 0;
        }
#endif
    }
}
