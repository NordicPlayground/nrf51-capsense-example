#ifndef __NRF_CAP_SENSE_H__
#define __NRF_CAP_SENSE_H__

#include <stdint.h>
#include <stdbool.h>

#define ROLLING_AVG_FACTOR  2048
#define DEBOUNCE_ITERATIONS 4
#define DEBOUNCE_ITER_MASK  0x0F

#define HIGH_AVG_THRESHOLD  8

#define CAPSENSE_TIMER      NRF_TIMER1

enum capsense_ana_inputs {ANA_AIN0_P26, ANA_AIN1_P27, ANA_AIN2_P01, ANA_AIN3_P02, ANA_AIN4_P03, ANA_AIN5_P04, ANA_AIN6_P05, ANA_AIN7_P06};

typedef struct
{
    uint32_t    ana_pin;
    uint32_t    out_pin;
    uint32_t    rolling_average;
    uint32_t    average, average_counter, average_int;
    uint32_t    value;
    uint32_t    val_max, val_min;
    uint32_t    value_debouncemask; 
    uint32_t    high_threshold;
    bool        pressed;
    uint32_t    ch_num;
}capsense_channel_t;

typedef struct
{
    uint32_t input;
    uint32_t output;
}capsense_config_t;

/*
#define NRF_CAPSENSE_INIT(CONFIG_ARRAY, CHANNEL_NUM) do{                 \
        static capsense_channel_t m_capsense_array[CHANNEL_NUM];                \ 
        nrf_capsense_init(m_capsense_array, CONFIG_ARRAY, CHANNEL_NUM);  \
    }while(0)
*/

void nrf_capsense_init(capsense_channel_t *channel_array, capsense_config_t *config_array, uint32_t channel_num);

capsense_channel_t *nrf_capsense_sample(void);

#endif
