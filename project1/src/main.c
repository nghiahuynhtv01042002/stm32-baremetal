#include <stdint.h>
#include "sysclocks.h"
#include "usbfs.h"
#include "gpio.h"
#include "nvic.h"
#include "uart.h"

// #define NORMAL
// #define INTERRUPT
#define DMA

extern uint32_t SystemCoreClock;
uint8_t tx_buf[UART_TX_BUFFER_SIZE];
uint8_t rx_buf[UART_RX_BUFFER_SIZE];

void UART_EchoTask(UART_Config_t* uart_cfg){
    static uint8_t data[UART_RX_BUFFER_SIZE];
    uint16_t length = UART_ReceiveData(uart_cfg, data,UART_RX_BUFFER_SIZE);
    if (length > 0) {
        UART_SendData((const uint8_t *)"echo :",7);
        UART_SendData((const uint8_t *)data,length);
    }
}
int main(void) {

    UART_Config_t uart_cfg;
#ifdef NORMAL
    uart_cfg.mode = UART_MODE_NORMAL;
#endif
#ifdef INTERRUPT
    uart_cfg.mode = UART_MODE_INTERRUPT;
#endif
#ifdef DMA
    uart_cfg.mode = UART_MODE_DMA;
#endif

    uart_cfg.baudrate = 115200;
    uart_cfg.tx_buffer = tx_buf;
    uart_cfg.rx_buffer = rx_buf;
    uart_cfg.tx_buffer_size = UART_TX_BUFFER_SIZE;
    uart_cfg.rx_buffer_size = UART_RX_BUFFER_SIZE;

    UART_Init(&uart_cfg);
    
#ifdef NORMAL
    UART_SendData((const uint8_t *)"UART Normal Echo Test Start\r\n",30);
#endif
#ifdef INTERRUPT
    UART_SendData((const uint8_t *)"UART Interupt Echo Test Start\r\n",32);
#endif
#ifdef DMA
    UART_SendData((const uint8_t *)"UART DMA Echo Test Start\r\n",27);
#endif

    while(1) {
        UART_EchoTask(&uart_cfg);
        delay_ms(1000);
    }
}