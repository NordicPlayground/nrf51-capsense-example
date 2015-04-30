#include "nrf_capsense.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

const uint32_t analog_ch_input_no[] = {26, 27, 1, 2, 3, 4, 5, 6};
static uint32_t m_channel_num;
static capsense_channel_t *m_capsense_channel_list;

void nrf_capsense_init(capsense_channel_t *channel_array, capsense_config_t *config_array, uint32_t channel_num)
{
    CAPSENSE_TIMER->PRESCALER = 0;
    CAPSENSE_TIMER->BITMODE = TIMER_BITMODE_BITMODE_16Bit << TIMER_BITMODE_BITMODE_Pos;
    CAPSENSE_TIMER->CC[3] = 50000;
    CAPSENSE_TIMER->INTENSET = TIMER_INTENSET_COMPARE1_Msk;
    
    NRF_LPCOMP->REFSEL = LPCOMP_REFSEL_REFSEL_SupplyFourEighthsPrescaling << LPCOMP_REFSEL_REFSEL_Pos;
    NRF_LPCOMP->ANADETECT = LPCOMP_ANADETECT_ANADETECT_Up << LPCOMP_ANADETECT_ANADETECT_Pos;
    NRF_LPCOMP->INTENSET = LPCOMP_INTENSET_UP_Msk;
    NRF_LPCOMP->ENABLE = LPCOMP_ENABLE_ENABLE_Enabled << LPCOMP_ENABLE_ENABLE_Pos;
   
    m_capsense_channel_list = channel_array;
    m_channel_num = channel_num;
    for(uint32_t ch = 0; ch < channel_num; ch++)
    {
        nrf_gpio_cfg_output(config_array[ch].output);
        nrf_gpio_cfg_input(analog_ch_input_no[config_array[ch].input], NRF_GPIO_PIN_NOPULL);
    
        NRF_PPI->CH[ch].EEP = (uint32_t)&NRF_LPCOMP->EVENTS_UP;
        NRF_PPI->CH[ch].TEP = (uint32_t)&NRF_TIMER1->TASKS_CAPTURE[ch];
        NRF_PPI->CHENSET = 1 << ch;
    
        channel_array[ch].ana_pin = config_array[ch].input;
        channel_array[ch].out_pin = config_array[ch].output;
        channel_array[ch].rolling_average    = 400 * ROLLING_AVG_FACTOR;
        channel_array[ch].average            = 0xFFFF;
        channel_array[ch].average_counter    = channel_array[ch].average_int = 0;
        channel_array[ch].value_debouncemask = 0;
        channel_array[ch].val_max            = 0;
        channel_array[ch].val_min            = 0xFFFF;
        channel_array[ch].high_threshold     = 435;
        channel_array[ch].ch_num             = ch;    
    }
}

capsense_channel_t *nrf_capsense_sample(void)
{
    static bool value_above_threshold = false;
    for(uint32_t ch = 0; ch < m_channel_num; ch++)
    {
        NRF_LPCOMP->PSEL = m_capsense_channel_list[ch].ana_pin << LPCOMP_PSEL_PSEL_Pos;
        NRF_LPCOMP->TASKS_START = 1;
        NRF_TIMER1->TASKS_START = 1;
        nrf_gpio_pin_set(m_capsense_channel_list[ch].out_pin);
        nrf_delay_us(100);
        //NRF_GPIOTE->TASKS_OUT[0] = 1;
        nrf_gpio_pin_clear(m_capsense_channel_list[ch].out_pin);
        NRF_TIMER1->TASKS_STOP = 1;
        NRF_TIMER1->TASKS_CLEAR = 1;
        m_capsense_channel_list[ch].value = NRF_TIMER1->CC[m_capsense_channel_list[ch].ch_num];
        NRF_LPCOMP->TASKS_STOP = 1;
        if(m_capsense_channel_list[ch].value > m_capsense_channel_list[ch].val_max)
        {
            m_capsense_channel_list[ch].val_max = m_capsense_channel_list[ch].value;
        }
        if(m_capsense_channel_list[ch].value < m_capsense_channel_list[ch].val_min)
        {
            m_capsense_channel_list[ch].val_min = m_capsense_channel_list[ch].value;
        }
        
        if(m_capsense_channel_list[ch].average_counter < 100)
            value_above_threshold = (m_capsense_channel_list[ch].value > m_capsense_channel_list[ch].high_threshold);
        else
            value_above_threshold = (m_capsense_channel_list[ch].value > (m_capsense_channel_list[ch].average + HIGH_AVG_THRESHOLD));
        
        m_capsense_channel_list[ch].rolling_average = (m_capsense_channel_list[ch].rolling_average * (ROLLING_AVG_FACTOR-1) + (m_capsense_channel_list[ch].value*ROLLING_AVG_FACTOR))/ROLLING_AVG_FACTOR;
        m_capsense_channel_list[ch].average_counter++;
        m_capsense_channel_list[ch].average_int += m_capsense_channel_list[ch].value;
        m_capsense_channel_list[ch].average = m_capsense_channel_list[ch].average_int / m_capsense_channel_list[ch].average_counter;
        if(m_capsense_channel_list[ch].average_counter > 1000)
        {
            m_capsense_channel_list[ch].average_counter /= 2;
            m_capsense_channel_list[ch].average_int /= 2;
        }
        m_capsense_channel_list[ch].value_debouncemask = (m_capsense_channel_list[ch].value_debouncemask << 1) | (value_above_threshold ? 0x01 : 0);
        if(m_capsense_channel_list[ch].pressed && ((m_capsense_channel_list[ch].value_debouncemask & DEBOUNCE_ITER_MASK) == 0))
        {
            m_capsense_channel_list[ch].pressed = false;
        }
        if(!m_capsense_channel_list[ch].pressed && ((m_capsense_channel_list[ch].value_debouncemask & DEBOUNCE_ITER_MASK) == DEBOUNCE_ITER_MASK))
        {
            m_capsense_channel_list[ch].pressed = true;
        }
    }
    return m_capsense_channel_list;
}

