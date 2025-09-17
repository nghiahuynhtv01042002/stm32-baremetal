#include "uart.h"
#include "nvic.h" 
#include "isr.h"
volatile UART_Mode_t current_uart_mode = UART_MODE_NORMAL;
volatile uint8_t uart_tx_buffer[UART_TX_BUFFER_SIZE];
volatile uint8_t uart_rx_buffer[UART_RX_BUFFER_SIZE];
volatile uint16_t uart_tx_head = 0, uart_tx_tail = 0;
volatile uint16_t uart_rx_head = 0, uart_rx_tail = 0;
volatile bool uart_tx_busy = false;
// function prototypes
/*
    We could use __attribute__((weak, alias("alias_symbol"))) to provide default (weak) implementations.
    However, when compiling with the -flto option (Link Time Optimization), 
    this may cause multiple definition errors because the same weak symbols can appear 
    in multiple translation units after LTO merges them.

    To avoid this issue, we use #pragma weak instead.
*/

// void UART_Normal_SendData(const uint8_t *buffer, uint16_t length) __attribute__((weak, alias("UART_Normal_SendData_weak")));
// uint16_t UART_Normal_ReceiveData(uint8_t *buffer, uint16_t max_length) __attribute__((weak, alias("UART_Normal_ReceiveData_weak")));
// void UART_Interrupt_SendData(const uint8_t *buffer, uint16_t length) __attribute__((weak, alias("UART_Interrupt_SendData_weak")));
// uint16_t UART_Interrupt_ReceiveData(uint8_t *buffer, uint16_t max_length) __attribute__((weak, alias("UART_Interrupt_ReceiveData_weak")));
// void UART_DMA_SendData(const uint8_t *data, uint16_t length) __attribute__((weak, alias("UART_DMA_SendData_weak")));
// uint16_t UART_DMA_ReceiveData(UART_Config_t* uart_cfg, uint8_t *app_buffer, uint16_t max_length) __attribute__((weak, alias("UART_DMA_ReceiveData_weak")));

// func_weak functions are weak aliases used when the user does not provide definitions
#pragma weak UART_Normal_SendData = UART_Normal_SendData_weak
#pragma weak UART_Normal_ReceiveData = UART_Normal_ReceiveData_weak
#pragma weak UART_Interrupt_SendData = UART_Interrupt_SendData_weak
#pragma weak UART_Interrupt_ReceiveData = UART_Interrupt_ReceiveData_weak
#pragma weak UART_DMA_SendData = UART_DMA_SendData_weak
#pragma weak UART_DMA_ReceiveData = UART_DMA_ReceiveData_weak

// Private function to calculate BRR value
static uint32_t UART_CalculateBRR(uint32_t baudrate) {
    // APB1 clock = SystemCoreClock / 2
    uint32_t apb1_clock = SystemCoreClock / 2;
    return (apb1_clock + (baudrate / 2)) / baudrate;
}

// Initialize UART GPIO pins (PA2-TX, PA3-RX)
static void UART_GPIO_Init(void) {
    // Enable GPIOA clock
    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOA_EN;
    
    // Configure PA2 and PA3 as alternate function
    GPIOA_MODER &= ~((3 << (2*2)) | (3 << (2*3))); // Clear bits
    GPIOA_MODER |= (2 << (2*2)) | (2 << (2*3));    // Set alternate function mode
    
    // Set alternate function AF7 for USART2
    GPIOA_AFRL &= ~((0xF << (4*2)) | (0xF << (4*3))); // Clear AF bits
    GPIOA_AFRL |= (7 << (4*2)) | (7 << (4*3));        // Set AF7
    
    // Set speed to high
    GPIOA_OSPEEDR |= (3 << (2*2)) | (3 << (2*3));
    
    // Set pull-up for RX pin
    GPIOA_PUPDR &= ~((3 << (2*3)));
    GPIOA_PUPDR |= (1 << (2*3));
}

// Normal mode initialization
void UART_Normal_Init(uint32_t baudrate) {
    // Initialize GPIO
    UART_GPIO_Init();
    
    // Enable USART2 clock
    RCC_APB1ENR |= RCC_APB1ENR_USART2EN;
    
    // Disable UART
    USART2_CR1 &= ~USART_CR1_UE;
    
    // Configure baud rate
    USART2_BRR = UART_CalculateBRR(baudrate);
    
    // Configure UART: 8 data bits, no parity, 1 stop bit
    USART2_CR1 &= ~(USART_CR1_M | USART_CR1_PCE);
    USART2_CR2 &= ~(3 << 12); // 1 stop bit
    
    // Enable transmitter and receiver
    USART2_CR1 |= USART_CR1_TE | USART_CR1_RE;
    
    // Enable UART
    USART2_CR1 |= USART_CR1_UE;
}

// Interrupt mode initialization
void UART_Interrupt_Init(uint32_t baudrate, uint8_t *tx_buf, uint8_t *rx_buf, uint16_t tx_size, uint16_t rx_size) {
    // Initialize GPIO
    UART_GPIO_Init();
    
    // Enable USART2 clock
    RCC_APB1ENR |= RCC_APB1ENR_USART2EN;
    
    // Disable UART
    USART2_CR1 &= ~USART_CR1_UE;
    
    // Configure baud rate
    USART2_BRR = UART_CalculateBRR(baudrate);
    
    // Configure UART
    USART2_CR1 &= ~(USART_CR1_M | USART_CR1_PCE);
    USART2_CR2 &= ~(3 << 12);
    
    // Enable RX interrupt
    USART2_CR1 |= USART_CR1_RXNEIE;
    
    // Enable UART interrupt
    NVIC_SetPriority(USART2_IRQn, 1);
    NVIC_EnableIRQ(USART2_IRQn);
    
    // Initialize buffer pointers
    uart_tx_head = uart_tx_tail = 0;
    uart_rx_head = uart_rx_tail = 0;
    uart_tx_busy = false;
    
    // Enable transmitter and receiver
    USART2_CR1 |= USART_CR1_TE | USART_CR1_RE;
    
    // Enable UART
    USART2_CR1 |= USART_CR1_UE;
}

void UART_DMA_Init(uint32_t baudrate, uint8_t *tx_buf, uint8_t *rx_buf, uint16_t tx_size, uint16_t rx_size) {
    // Initialize GPIO
    UART_GPIO_Init();
    
    // Enable clocks
    RCC_APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC_AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    
    // Disable UART
    USART2_CR1 &= ~USART_CR1_UE;
    
    // Configure baud rate
    USART2_BRR = UART_CalculateBRR(baudrate);
    
    // Configure UART
    USART2_CR1 &= ~(USART_CR1_M | USART_CR1_PCE);
    USART2_CR2 &= ~(3 << 12);
    
    // Enable DMA for TX and RX
    USART2_CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;
    
    // Configure DMA1 Stream 6 for TX (Channel 4)
    DMA1_S6CR &= ~DMA_SxCR_EN; // Disable stream
    while(DMA1_S6CR & DMA_SxCR_EN); // Wait until disabled
    
    DMA1_S6PAR = (uint32_t)&USART2_DR;
    DMA1_S6M0AR = (uint32_t)tx_buf;
    DMA1_S6CR = (4 << 25) |           // Channel 4
                (1 << 6) |            // Memory to peripheral
                DMA_SxCR_MINC |       // Memory increment
                DMA_SxCR_TCIE |       // Transfer complete interrupt
                DMA_SxCR_TEIE |       // Transfer error interrupt
                DMA_SxCR_DMEIE;       // Direct mode error interrupt
    
    // Configure DMA1 Stream 5 for RX (Channel 4)
    DMA1_S5CR &= ~DMA_SxCR_EN;
    while(DMA1_S5CR & DMA_SxCR_EN);
    
    DMA1_S5PAR = (uint32_t)&USART2_DR;
    DMA1_S5M0AR = (uint32_t)rx_buf;
    DMA1_S5NDTR = rx_size;
    DMA1_S5CR = (4 << 25) |           // Channel 4
                DMA_SxCR_CIRC |       // Circular mode
                DMA_SxCR_MINC |       // Memory increment
                DMA_SxCR_HTIE |       // Half transfer interrupt  
                DMA_SxCR_TCIE |       // Transfer complete interrupt
                DMA_SxCR_TEIE |       // Transfer error interrupt
                DMA_SxCR_DMEIE |      // Direct mode error interrupt
                DMA_SxCR_EN;          // Enable stream
    
    // Initialize tracking variables
    dma_tx_done = false;
    dma_rx_last_pos = 0;
    dma_rx_overflow = false;
    
    // Enable DMA interrupts
    NVIC_SetPriority(DMA1_Stream5_IRQn, 2);
    NVIC_SetPriority(DMA1_Stream6_IRQn, 2);
    NVIC_EnableIRQ(DMA1_Stream5_IRQn);
    NVIC_EnableIRQ(DMA1_Stream6_IRQn);
    
    // Enable transmitter and receiver
    USART2_CR1 |= USART_CR1_TE | USART_CR1_RE;
    
    // Enable UART
    USART2_CR1 |= USART_CR1_UE;
}
// General UART initialization
void UART_Init(UART_Config_t *config) {
    switch(config->mode) {
        case UART_MODE_NORMAL:
            UART_Normal_Init(config->baudrate);
            break;
        case UART_MODE_DMA:
            UART_DMA_Init(config->baudrate, config->tx_buffer, config->rx_buffer, 
                         config->tx_buffer_size, config->rx_buffer_size);
            break;
        case UART_MODE_INTERRUPT:
            UART_Interrupt_Init(config->baudrate, config->tx_buffer, config->rx_buffer,
                               config->tx_buffer_size, config->rx_buffer_size);
            break;
    }
    current_uart_mode = config->mode;
}
// Normal
// Weak alias for UART_Normal_SendData
void UART_Normal_SendData_weak(const uint8_t *buffer, uint16_t length) {
    uint16_t count = 0;
        while(count < length) {
            if (current_uart_mode == UART_MODE_NORMAL) {
                while(!(USART2_SR & USART_SR_TXE));
                USART2_DR = *buffer++;
                count++;
            }
        }
}

// Weak alias for UART_Normal_ReceiveData
uint16_t UART_Normal_ReceiveData_weak(uint8_t *buffer, uint16_t max_length) {
    uint16_t count = 0; 
    while(count < max_length) {
        if(USART2_SR & USART_SR_RXNE){
            buffer[count++] = USART2_DR;
        }
    }
    return count;
}
// Interrupt
// Weak alias for UART_Interrupt_ReceiveData
void UART_Interrupt_SendData_weak(const uint8_t *buffer, uint16_t length) {
    for (uint16_t idx = 0; idx < length; ++idx ) {
        uint16_t next_head = (uart_tx_head + 1) % UART_TX_BUFFER_SIZE;
        // Buffer is full
        if(next_head == uart_tx_tail) {
            return;
        }
        uart_tx_buffer[uart_tx_head] = buffer[idx]; 
        uart_tx_head = next_head;
    }
    //  Enable TXEIE to send data
    if(!uart_tx_busy) {
        uart_tx_busy = true;
        USART2_CR1 |= USART_CR1_TXEIE;
    }
}

// Weak alias for UART_Interrupt_ReceiveData
uint16_t UART_Interrupt_ReceiveData_weak(uint8_t *buffer, uint16_t max_length) {
    uint16_t count = 0;
    while(count < max_length && uart_rx_head != uart_rx_tail) {
        buffer[count] = uart_rx_buffer[uart_rx_tail];
        uart_rx_tail = (uart_rx_tail + 1) % UART_RX_BUFFER_SIZE;
        count++;
    }
    return count;
}
// DMA 
volatile bool dma_tx_done = false;
volatile bool dma_rx_overflow = false;
volatile uint16_t dma_rx_last_pos = 0;

// Weak alias for UART_DMA_SendData
void UART_DMA_SendData_weak(const uint8_t *data, uint16_t length) {
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

// Weak alias for UART_DMA_ReceiveData
uint16_t UART_DMA_ReceiveData_weak(UART_Config_t* uart_cfg, uint8_t *app_buffer, uint16_t max_length) {
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
// Abstraction for UART_ReceiveData
void UART_SendData(const uint8_t *data, uint16_t length) {
    switch(current_uart_mode) {
        case UART_MODE_NORMAL:
            UART_Normal_SendData(data, length) ;
            break;
        case UART_MODE_DMA:
            UART_DMA_SendData(data, length);
            break;
        case UART_MODE_INTERRUPT:
            UART_Interrupt_SendData(data, length);
            break;
    }
}

// Abstraction for UART_ReceiveData
uint16_t UART_ReceiveData(UART_Config_t* uart_cfg, uint8_t *data, uint16_t max_length) {
    uint16_t length = 0;
    switch(current_uart_mode) {
        case UART_MODE_NORMAL:
            length = UART_Normal_ReceiveData(data, UART_RX_BUFFER_SIZE);
            break;
        case UART_MODE_INTERRUPT:
            length =UART_Interrupt_ReceiveData(data, UART_RX_BUFFER_SIZE);
            break;
        case UART_MODE_DMA:
            length = UART_DMA_ReceiveData(uart_cfg,data, UART_RX_BUFFER_SIZE);
            break;
    }
    return length;
}

void UART_DMA_ClearRxBuffer(void) {
    uint16_t current_ndtr = DMA1_S5NDTR;
    dma_rx_last_pos = UART_RX_BUFFER_SIZE - current_ndtr;
    dma_rx_overflow = false;
}

