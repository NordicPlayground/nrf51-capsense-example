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
 
#ifndef __NRF_CAP_SENSE_H__
#define __NRF_CAP_SENSE_H__

#include <stdint.h>
#include <stdbool.h>

// This factor determines how quickly the rolling average will change based on the sampled input.
// A high value makes the average more stable, but means it takes longer to detect changes in the capacitance.
// If the value is too low it could make the average change too quickly, which is an issue when holding a button
// down for a long time.
#define ROLLING_AVG_FACTOR          2048

// This factor determines how many samples are needed for the capsense library to start to provide output.
#define INITIAL_CALIBRATION_TIME    100

// These factors decide how many successive high samples are needed for a capacitive button to be considered pressed. 
// A high value increases button stability, but also increases the latency of button presses. 
#define DEBOUNCE_ITERATIONS         3
#define DEBOUNCE_ITER_MASK          0x07

// This factor determines how much offset from the average it takes to consider a button pressed.
// A high value means the button must be pressed harder (reduced sensitivity), but reduces the chance of false button 
// presses caused by noise. 
#define HIGH_AVG_THRESHOLD          8

// Change these three defines if a different timer should be used by the capsense library. 
#define CAPSENSE_TIMER              NRF_TIMER1
#define CAPSENSE_TIMER_IRQ          TIMER1_IRQn
#define CAPSENSE_TIMER_IRQHandler   TIMER1_IRQHandler

// PPI channel assignment of the capsense library. 
#define CAPSENSE_PPI_CH0            0
#define CAPSENSE_PPI_CH1            1

enum capsense_ana_inputs {ANA_AIN0_P26, ANA_AIN1_P27, ANA_AIN2_P01, ANA_AIN3_P02, ANA_AIN4_P03, ANA_AIN5_P04, ANA_AIN6_P05, ANA_AIN7_P06};

typedef void (*capsense_callback_t)(uint32_t pressed_mask, uint32_t changed_mask);

typedef struct
{
    uint32_t    ana_pin;
    uint32_t    out_pin;
    uint32_t    rolling_average;
    uint32_t    average, average_counter, average_int;
    uint32_t    value;
    uint32_t    val_max, val_min;
    uint32_t    value_debouncemask; 
    bool        pressed;
    uint32_t    ch_num;
}capsense_channel_t;

typedef struct
{
    uint32_t input;
    uint32_t output;
}capsense_config_t;

// Function to initialize the capsense library.
void nrf_capsense_init(capsense_channel_t *channel_array, capsense_config_t *config_array, uint32_t channel_num);

// Function to set the callback which is triggered when one of the capsense channels registers a change.
void nrf_capsense_set_callback(capsense_callback_t callback);

// Function to initiate sampling of all the registered capsense channels. 
// Once sampling of all channels is complete the callback will be called if any change was registered. 
void nrf_capsense_sample(void);

#endif
