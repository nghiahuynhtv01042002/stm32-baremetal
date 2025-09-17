/* This file is used to override weak aliases defined in uart.h */
#include "uart.h"
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
void UART_DMA_SendData(const uint8_t *data, uint16_t length) {
    if (length == 0) return;
    
    while(DMA1_S6CR & DMA_SxCR_EN) ;
    
    // Clear transfer complete flag
    DMA1_HIFCR |= (1 << 21); // Clear TCIF6
    DMA1_HIFCR |= (1 << 16); // Clear FEIF6
    DMA1_HIFCR |= (1 << 18); // Clear DMEIF6
    DMA1_HIFCR |= (1 << 19); // Clear TEIF6
    DMA1_HIFCR |= (1 << 20); // Clear HTIF6
    
    // Reset flag
    dma_tx_done = false;
    
    // Configure new transfer
    DMA1_S6M0AR = (uint32_t)data;
    DMA1_S6NDTR = length;
    
    // Enable stream
    DMA1_S6CR |= DMA_SxCR_EN;
}