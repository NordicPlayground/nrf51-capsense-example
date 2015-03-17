/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
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

#include "nrf_uart_int.h" 
#include <stdio.h>
#include <stdint.h>

struct __FILE { int handle; };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE * p_file) 
{
    nrf_uart_put((uint8_t)ch);
    return 0;
}

int _write(int file, const char * p_data, int len)
{
    const char * p_end_of_sequence = (p_data + len);
    
    while (p_data != p_end_of_sequence)
    {
        nrf_uart_put(*p_data++);
    }
    return len;
}
