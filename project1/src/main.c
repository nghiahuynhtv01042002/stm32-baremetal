#include <stdint.h>
#include "sysclocks.h"
#include "usbfs.h"
#include "gpio.h"
#include "nvic.h"
#include "uart.h"
#define test_RX
// #define test_TX
extern uint32_t SystemCoreClock;
extern void GPIOConfig(void);
extern void Config_MCO(void);
extern void UART_Normal_SendData(const uint8_t *str,uint16_t length);
uint8_t tx_buf[UART_TX_BUFFER_SIZE];
uint8_t rx_buf[UART_RX_BUFFER_SIZE];

uint16_t UART_Normal_ReceiveData(uint8_t *buffer, uint16_t max_length) {
    uint16_t count = 0;
    while(count < max_length) {
        if(USART2_SR & USART_SR_RXNE) {
            uint8_t c = USART2_DR;
            buffer[count++] = c;
            // get the end of data frame '\n'(LF char)
            if(c == '\n') break;
        }
    }
    return count;
}

void UART_Normal_SendData(const uint8_t *str,uint16_t length) {
    while(*str) {
        // Wait TXE
        while(!(USART2_SR & USART_SR_TXE));
        char c = *str++;
        USART2_DR = c;
        // get the end of data frame '\n'(LF char)
        if(c == '\n') break;
    }
}

uint16_t UART_Interrupt_ReceiveData(uint8_t *buffer, uint16_t max_length) {
    uint16_t count = 0;
    while(count < max_length && uart_rx_head != uart_rx_tail) {
        buffer[count] = uart_rx_buffer[uart_rx_tail];
        uart_rx_tail = (uart_rx_tail + 1) % UART_RX_BUFFER_SIZE;
        count++;
        // get the end of data frame '\n'(LF char)
        if(buffer[count-1] == '\n') break; 
    }
    return count;
}
void UART_Interrupt_SendData(const uint8_t *buffer, uint16_t length) {
    for (uint16_t idx = 0; idx < length; ++idx ) {
        uint16_t next_head = (uart_tx_head + 1) % UART_TX_BUFFER_SIZE;
        // Buffer is full
        if(next_head == uart_tx_tail) {
            return;
        }
        uart_tx_buffer[uart_tx_head] = buffer[idx]; 
        uart_tx_head = next_head;
         if(buffer[idx] == '\n') break;
    }
    //  Enable TXEIE to send data
    if(!uart_tx_busy) {
        uart_tx_busy = true;
        USART2_CR1 |= USART_CR1_TXEIE;
    }
}

void UART_Normal_EchoTask(UART_Config_t* uart_cfg) {
    static uint8_t data[UART_RX_BUFFER_SIZE];
    memset(data,'0',UART_RX_BUFFER_SIZE);
    uint16_t len = UART_Normal_ReceiveData(data, UART_RX_BUFFER_SIZE);
    // length is meanlless in this implement
    if(len > 0) {
        UART_Normal_SendData("echo :",8);
        UART_Normal_SendData((const uint8_t *)data,sizeof(data));
    }

}
void UART_Interrupt_EchoTask(UART_Config_t* uart_cfg) {
    static uint8_t data[UART_RX_BUFFER_SIZE];
    memset(data,'0',UART_RX_BUFFER_SIZE);
    uint16_t len = UART_Interrupt_ReceiveData(data, UART_RX_BUFFER_SIZE);
    if (len > 0) {
        UART_Interrupt_SendData("echo :",8);
        UART_Interrupt_SendData((const uint8_t *)data,sizeof(data));
    }

}

int main(void) {
    // SystemClockConfig();
    GPIOConfig();
    GPIOx_Set_MODER(&GPIOD_MODER, 13, 0x01);
    UART_Config_t uart_cfg;
    // uart_cfg.mode = UART_MODE_NORMAL;  
    uart_cfg.mode = UART_MODE_INTERRUPT;  
    uart_cfg.baudrate = 115200;
    uart_cfg.tx_buffer = tx_buf;
    uart_cfg.rx_buffer = rx_buf;
    uart_cfg.tx_buffer_size = UART_TX_BUFFER_SIZE;
    uart_cfg.rx_buffer_size = UART_RX_BUFFER_SIZE;

    UART_Init(&uart_cfg);

    UART_Normal_SendData("UART Echo Test Start\r\n",0);
    // GPIOx_write(&GPIOD_ODR, 13);
    while(1) {

        // UART_Normal_EchoTask(&uart_cfg);
        UART_Interrupt_EchoTask(&uart_cfg);
        delay_ms(1000);
    }
}
