#ifndef NRF_UART_INT__
#define NRF_UART_INT__

#include <stdbool.h>
#include "nrf.h"

#define UART_GET_TIMEOUT 0x100

typedef enum 
{
    UART_BAUDRATE_1200 = UART_BAUDRATE_BAUDRATE_Baud1200, 
    UART_BAUDRATE_2400 = UART_BAUDRATE_BAUDRATE_Baud2400, 
    UART_BAUDRATE_4800 = UART_BAUDRATE_BAUDRATE_Baud4800,
    UART_BAUDRATE_9600 = UART_BAUDRATE_BAUDRATE_Baud9600, 
    UART_BAUDRATE_14400 = UART_BAUDRATE_BAUDRATE_Baud14400, 
    UART_BAUDRATE_19200 = UART_BAUDRATE_BAUDRATE_Baud19200,
    UART_BAUDRATE_28800 = UART_BAUDRATE_BAUDRATE_Baud28800, 
    UART_BAUDRATE_38400 = UART_BAUDRATE_BAUDRATE_Baud38400, 
    UART_BAUDRATE_57600 = UART_BAUDRATE_BAUDRATE_Baud57600,
    UART_BAUDRATE_76800 = UART_BAUDRATE_BAUDRATE_Baud76800, 
    UART_BAUDRATE_115200 = UART_BAUDRATE_BAUDRATE_Baud115200, 
    UART_BAUDRATE_230400 = UART_BAUDRATE_BAUDRATE_Baud230400,
    UART_BAUDRATE_460800 = UART_BAUDRATE_BAUDRATE_Baud460800, 
    UART_BAUDRATE_1000000 = UART_BAUDRATE_BAUDRATE_Baud1M
} uart_baudrate_t;

typedef enum
{
    UART_PWR_MODE_ALWAYS_ON = 0,
    UART_PWR_MODE_DUMMY_BYTE,
    UART_PWR_MODE_RXD_LOW
} uart_pwrmode_t;

typedef struct
{
    uint8_t         pin_txd;
    uint8_t         pin_rxd;
    uint8_t         pin_rts;
    uint8_t         pin_cts;
    uart_baudrate_t baudrate;
    uint8_t         hwfc     : 1;
    uint8_t         parity   : 1;
    //uart_pwrmode_t  power_mode : 2;
} uart_config_t; 

uint32_t nrf_uart_chars_available(void);

bool nrf_uart_tx_idle(void);

/** Reads a character from UART.
Execution is blocked until UART peripheral detects character has been received.
\return cr Received character.
*/
uint8_t nrf_uart_get(void);

bool nrf_uart_get_timeout(uint8_t *byte_ptr, uint32_t time_us);

/** Sends a character to UART.
Execution is blocked until UART peripheral reports character to have been send.
@param cr Character to send.
*/
void nrf_uart_put(char cr);

/** Sends a string to UART.
Execution is blocked until UART peripheral reports all characters to have been send.
Maximum string length is 254 characters including null character in the end.
@param str Null terminated string to send.
*/
void nrf_uart_putstring(const char *str);

void nrf_uart_on_gpiote_irq(void);

void nrf_uart_power_down(void);
void nrf_uart_power_up(void);

void nrf_uart_flush_rx(void);

/** Configures UART without HW flow control.
@param txd_pin_number Chip pin number to be used for UART TXD
@param rxd_pin_number Chip pin number to be used for UART RXD
@param baudrate The UART baud rate used
*/
void nrf_uart_init(uart_config_t *uart_config);

#endif
