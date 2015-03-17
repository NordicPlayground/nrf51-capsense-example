#include "nrf_uart_int.h"
#include "nrf_ringbuffer.h"
#include "nrf.h"
#include "nrf_delay.h"
#include <string.h>

volatile ringbuffer8_t nrf_uart_ptx_buf, nrf_uart_prx_buf;
volatile bool nrf_uart_ptx_idle = true;
uart_config_t nrf_uart_current_config;

uint32_t nrf_uart_chars_available()
{
    return nrf_uart_prx_buf.size;
}
void nrf_uart_flush_rx()
{
    NRF_UART0->INTENCLR = UART_INTENSET_RXDRDY_Msk;  
    nrf_ringbuffer8_init(&nrf_uart_prx_buf);
    NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Msk;  
}
uint8_t nrf_uart_get(void)
{
    uint8_t retval;
    while(nrf_ringbuffer8_empty(&nrf_uart_prx_buf));
    NRF_UART0->INTENCLR = UART_INTENSET_RXDRDY_Msk;   
    retval = nrf_ringbuffer8_get(&nrf_uart_prx_buf);
    NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Msk;
    return retval;
}

bool nrf_uart_get_timeout(uint8_t *byte_ptr, uint32_t time_us)
{
    while(nrf_ringbuffer8_empty(&nrf_uart_prx_buf))
    {
        if(time_us-- == 0)
        {
            return false;
        }
        nrf_delay_us(1);
    }
    NRF_UART0->INTENCLR = UART_INTENSET_RXDRDY_Msk;   
    *byte_ptr = nrf_ringbuffer8_get(&nrf_uart_prx_buf);
    NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Msk;
    return true;
}

void nrf_uart_put(char cr)
{
    while(nrf_ringbuffer8_full(&nrf_uart_ptx_buf));
    NRF_UART0->INTENCLR = UART_INTENSET_TXDRDY_Msk;
    if(nrf_uart_ptx_idle)
    {
        NRF_UART0->TXD = cr;
        nrf_uart_ptx_idle = false;
    }
    else
    {
        nrf_ringbuffer8_put(&nrf_uart_ptx_buf, cr);   
    }
    NRF_UART0->INTENSET = UART_INTENSET_TXDRDY_Msk;
}

void nrf_uart_putstring(const char *str)
{
  uint_fast8_t i = 0;
  uint8_t ch = str[i++];
  while (ch != '\0')
  {
    nrf_uart_put(ch);
    ch = str[i++];
  }
}
bool nrf_uart_tx_idle(void)
{
    return nrf_uart_ptx_idle;
}
void nrf_uart_power_down()
{
    // Clear the TX buffer before entering power down
    while(!nrf_uart_ptx_idle);
    
    NRF_UART0->TASKS_STOPTX = 1;
    NRF_UART0->TASKS_STOPRX = 1;
    NRF_UART0->ENABLE       = UART_ENABLE_ENABLE_Disabled << UART_ENABLE_ENABLE_Pos;
    if((NRF_UART0->CONFIG & UART_CONFIG_HWFC_Msk) == 0)
    {
        NRF_GPIO->PIN_CNF[NRF_UART0->PSELRXD] = GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos | \
                                                GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos;
    }
    else
    {
        NRF_GPIO->PIN_CNF[NRF_UART0->PSELRTS] = GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos | \
                                                GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos;        
    }
    NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_PORT_Msk;
    NRF_GPIOTE->EVENTS_PORT = 0;
    NVIC_ClearPendingIRQ(GPIOTE_IRQn);
    NVIC_EnableIRQ(GPIOTE_IRQn);
}
void nrf_uart_power_up()
{
    NRF_UART0->TASKS_STARTTX = 1;
    NRF_UART0->TASKS_STARTRX = 1;    
}
void nrf_uart_on_gpiote_irq(void)
{
    //uint32_t rxd_state = NRF_GPIO->IN & (1 << NRF_UART0->PSELRXD);

    NRF_GPIO->PIN_CNF[NRF_UART0->PSELRXD] = GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos | \
                                            GPIO_PIN_CNF_DIR_Input      << GPIO_PIN_CNF_DIR_Pos;
    NRF_GPIO->PIN_CNF[NRF_UART0->PSELRTS] = GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos | \
                                            GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos;     
    NRF_UART0->ENABLE           = UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos;    
    NRF_UART0->TASKS_STARTTX = 1;
    NRF_UART0->TXD = 0x55; //nrf_uart_current_config.wakeup_confirm_byte;
    nrf_uart_ptx_idle = false;
}
void nrf_uart_init(uart_config_t *uart_config)
{
    nrf_ringbuffer8_init(&nrf_uart_ptx_buf);
    nrf_ringbuffer8_init(&nrf_uart_prx_buf);
    memcpy((uart_config_t*)&nrf_uart_current_config, uart_config, sizeof(uart_config_t));

    NRF_UART0->PSELTXD = (uint32_t)uart_config->pin_txd;
    NRF_UART0->PSELRXD = (uint32_t)uart_config->pin_rxd;
    
    if(uart_config->hwfc == 1)
    {
        NRF_UART0->PSELRTS = (uint32_t)uart_config->pin_rts;
        NRF_UART0->PSELCTS = (uint32_t)uart_config->pin_cts;
    }
    else
    {
        NRF_UART0->PSELRTS = 0xFFFFFFFF;
        NRF_UART0->PSELCTS = 0xFFFFFFFF;        
    }

    NRF_UART0->BAUDRATE         = uart_config->baudrate << UART_BAUDRATE_BAUDRATE_Pos;
    NRF_UART0->CONFIG           = (uint32_t)uart_config->parity << UART_CONFIG_PARITY_Pos | \
                                  (uint32_t)uart_config->hwfc << UART_CONFIG_HWFC_Pos;
                                    
    NRF_UART0->ENABLE           = UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos;
    NRF_UART0->INTENSET         = UART_INTENSET_RXDRDY_Msk | UART_INTENSET_TXDRDY_Msk;     // Enable RXD and TXD interrupts
    NVIC_SetPriority(UART0_IRQn, 1);
    NVIC_EnableIRQ(UART0_IRQn);
    NRF_UART0->TASKS_STARTTX = 1;
    NRF_UART0->TASKS_STARTRX = 1;
    NRF_UART0->EVENTS_TXDRDY = 0;    
    NRF_UART0->EVENTS_RXDRDY = 0;
}

void UART0_IRQHandler(void)
{
    if(NRF_UART0->EVENTS_TXDRDY != 0)
    {
        NRF_UART0->EVENTS_TXDRDY = 0;
        if(nrf_ringbuffer8_empty(&nrf_uart_ptx_buf))
        {
            nrf_uart_ptx_idle = true;
            // This line is only needed when waking up from power down, consider changing (we are assuming STARTRX = 1 doesnt 
            NRF_UART0->TASKS_STARTRX = 1;
        }
        else
        {
            NRF_UART0->TXD = nrf_ringbuffer8_get(&nrf_uart_ptx_buf);
        }        
    }
    if(NRF_UART0->EVENTS_RXDRDY != 0)
    {
        NRF_UART0->EVENTS_RXDRDY = 0;
        nrf_ringbuffer8_put(&nrf_uart_prx_buf, NRF_UART0->RXD);
    }
}
