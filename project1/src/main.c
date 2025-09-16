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
    for(uint16_t idx =0 ; (*str) && (idx <length); ++idx) {
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
    static uint8_t data_normal[UART_RX_BUFFER_SIZE];
    memset(data_normal,'0',UART_RX_BUFFER_SIZE);
    uint16_t len = UART_Normal_ReceiveData(data_normal, UART_RX_BUFFER_SIZE);
    if(len > 0) {
        UART_Normal_SendData("echo :",8);
        UART_Normal_SendData((const uint8_t *)data_normal,sizeof(data_normal));
    }

}
void UART_Interrupt_EchoTask(UART_Config_t* uart_cfg) {
    static uint8_t data_interrupt[UART_RX_BUFFER_SIZE];
    memset(data_interrupt,'0',UART_RX_BUFFER_SIZE);
    uint16_t len = UART_Interrupt_ReceiveData(data_interrupt, UART_RX_BUFFER_SIZE);
    if (len > 0) {
        UART_Interrupt_SendData("echo :",8);
        UART_Interrupt_SendData((const uint8_t *)data_interrupt,sizeof(data_interrupt));
    }

}
uint16_t UART_DMA_ReceiveData(UART_Config_t* uart_cfg, uint8_t *app_buffer, uint16_t max_length) {
    if (app_buffer == NULL || max_length == 0 ) return 0;

    uint16_t current_ndtr = DMA1_S5NDTR;
    uint16_t current_pos = uart_cfg->rx_buffer_size - current_ndtr;

    uint16_t available_data;
    if (current_pos >= dma_rx_last_pos) {
        available_data = current_pos - dma_rx_last_pos;
    } else {
        available_data = (uart_cfg->rx_buffer_size - dma_rx_last_pos) + current_pos;
        dma_rx_overflow = true;
    }

    uint16_t to_read = (available_data < max_length) ? available_data : max_length;
    uint16_t count = 0;

    while (count < to_read) {
        app_buffer[count++] = uart_cfg->rx_buffer[dma_rx_last_pos];
        dma_rx_last_pos = (dma_rx_last_pos + 1) % uart_cfg->rx_buffer_size;
    }
    return count;
}

void UART_DMA_EchoTask(UART_Config_t* uart_cfg) {
    static uint8_t data_DMA[UART_RX_BUFFER_SIZE];
    uint16_t len = UART_DMA_ReceiveData(uart_cfg,data_DMA, UART_RX_BUFFER_SIZE);
    if (len > 0) {
        UART_DMA_SendData("echo :",8);
        UART_DMA_SendData((const uint8_t *)data_DMA,len);
    } 
}
int main(void) {
    GPIOConfig();
    GPIOx_Set_MODER(&GPIOD_MODER, 13, 0x01);
    UART_Config_t uart_cfg;
#ifdef INTERRUPT
    // ===== Interrupt =====
    uart_cfg.mode = UART_MODE_INTERRUPT;
    uart_cfg.baudrate = 115200;
    uart_cfg.tx_buffer = tx_buf;
    uart_cfg.rx_buffer = rx_buf;
    uart_cfg.tx_buffer_size = UART_TX_BUFFER_SIZE;
    uart_cfg.rx_buffer_size = UART_RX_BUFFER_SIZE;

    UART_Init(&uart_cfg);

    UART_Interrupt_SendData("UART Interupt Echo Test Start\r\n",32);
    while(1) {
        UART_Interrupt_EchoTask(&uart_cfg);
        delay_ms(1000);
    }
#endif
 
#ifdef NORMAL
    // ===== Normal =====
    uart_cfg.mode = UART_MODE_NORMAL;
    uart_cfg.baudrate = 115200;
    uart_cfg.tx_buffer = tx_buf;
    uart_cfg.rx_buffer = rx_buf;
    uart_cfg.tx_buffer_size = UART_TX_BUFFER_SIZE;
    uart_cfg.rx_buffer_size = UART_RX_BUFFER_SIZE;

    UART_Init(&uart_cfg);

    UART_Normal_SendData("UART Normal Echo Test Start\r\n",30);
    while(1) {
        UART_Normal_EchoTask(&uart_cfg);
        delay_ms(1000);
    }
#endif
#ifdef DMA
    // ===== DMA =====
    uart_cfg.mode = UART_MODE_DMA;
    uart_cfg.baudrate = 115200;
    uart_cfg.tx_buffer = tx_buf;
    uart_cfg.rx_buffer = rx_buf;
    uart_cfg.tx_buffer_size = UART_TX_BUFFER_SIZE;
    uart_cfg.rx_buffer_size = UART_RX_BUFFER_SIZE;

    UART_Init(&uart_cfg);
    UART_DMA_SendData("UART DMA Echo Test Start\r\n",27);
    while(1) {
        UART_DMA_EchoTask(&uart_cfg);
        delay_ms(1000);
    }
#endif

}