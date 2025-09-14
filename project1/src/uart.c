#include "uart.h"
#include "nvic.h" 

volatile UART_Mode_t current_uart_mode = UART_MODE_NORMAL;
volatile uint8_t uart_tx_buffer[UART_TX_BUFFER_SIZE];
volatile uint8_t uart_rx_buffer[UART_RX_BUFFER_SIZE];
volatile uint16_t uart_tx_head = 0, uart_tx_tail = 0;
volatile uint16_t uart_rx_head = 0, uart_rx_tail = 0;
volatile bool uart_tx_busy = false;

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

// DMA mode initialization
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
                DMA_SxCR_TCIE;        // Transfer complete interrupt
    
    // Configure DMA1 Stream 5 for RX (Channel 4)
    DMA1_S5CR &= ~DMA_SxCR_EN;
    while(DMA1_S5CR & DMA_SxCR_EN);
    
    DMA1_S5PAR = (uint32_t)&USART2_DR;
    DMA1_S5M0AR = (uint32_t)rx_buf;
    DMA1_S5NDTR = rx_size;
    DMA1_S5CR = (4 << 25) |           // Channel 4
                DMA_SxCR_CIRC |       // Circular mode
                DMA_SxCR_MINC |       // Memory increment
                DMA_SxCR_TCIE |       // Transfer complete interrupt
                DMA_SxCR_EN;          // Enable stream
    
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

// Send single character (blocking)
void UART_SendChar(char c) {
    if (current_uart_mode == UART_MODE_NORMAL) {
        while(!(USART2_SR & USART_SR_TXE));
        USART2_DR = c;
    }
}

// Receive single character (blocking)
char UART_ReceiveChar(void) {
    if (current_uart_mode == UART_MODE_NORMAL) {
        while(!(USART2_SR & USART_SR_RXNE));
        return USART2_DR;
    }
    return 0;
}


void UART_Normal_SendData(const uint8_t *str, uint16_t length) __attribute__((weak, alias("UART_Normal_SendData_weak")));
// Weak alias for UART_Normal_SendData
void UART_Normal_SendData_weak(const uint8_t *str, uint16_t length) {
    uint16_t count = 0;
        while(count < length) {
            if (current_uart_mode == UART_MODE_NORMAL) {
                while(!(USART2_SR & USART_SR_TXE));
                USART2_DR = *str++;
                count++;
            }
        }
}
uint16_t UART_Normal_ReceiveData(uint8_t *buffer, uint16_t max_length) __attribute__((weak, alias("UART_Normal_ReceiveData_weak")));
uint16_t UART_Normal_ReceiveData_weak(uint8_t *buffer, uint16_t max_length) {
    uint16_t count = 0; 
    while(count < max_length) {
        if(USART2_SR & USART_SR_RXNE){
            buffer[count++] = USART2_DR;
        }
    }
    return count;
}
// DMA send data
void UART_DMA_SendData(const uint8_t *data, uint16_t length) {
    // Wait for previous transfer to complete
    while(DMA1_S6CR & DMA_SxCR_EN);
    
    // Configure new transfer
    DMA1_S6M0AR = (uint32_t)data;
    DMA1_S6NDTR = length;
    
    // Clear transfer complete flag
    DMA1_HIFCR |= (1 << 21); // Clear TCIF6
    
    // Enable stream
    DMA1_S6CR |= DMA_SxCR_EN;
}


// Get RX count in DMA mode
uint16_t UART_DMA_GetRxCount(void) {
    return DMA1_S5NDTR;
}

// Interrupt send data
void UART_INT_SendData(const uint8_t *data, uint16_t length) {
    uint16_t i;
    // Disable TX interrupt temporarily
    USART2_CR1 &= ~USART_CR1_TXEIE;
    
    // Copy data to TX buffer
    for(i = 0; i < length; i++) {
        uint16_t next_head = (uart_tx_head + 1) % UART_TX_BUFFER_SIZE;
        if(next_head != uart_tx_tail) {
            uart_tx_buffer[uart_tx_head] = data[i];
            uart_tx_head = next_head;
        } else {
            break;
        }
    }
    
    // Enable TX interrupt if not busy
    if(!uart_tx_busy && uart_tx_head != uart_tx_tail) {
        uart_tx_busy = true;
        USART2_CR1 |= USART_CR1_TXEIE;
    } else {
        uart_tx_busy = false;
    }
}

// Get RX count in interrupt mode
uint16_t UART_INT_GetRxCount(void) {
    return (uart_rx_head >= uart_rx_tail) ? 
           (uart_rx_head - uart_rx_tail) : 
           (UART_RX_BUFFER_SIZE - uart_rx_tail + uart_rx_head);
}

// Check if data is available
bool UART_DataAvailable(void) {
    switch(current_uart_mode) {
        case UART_MODE_NORMAL:
            return (USART2_SR & USART_SR_RXNE);
        case UART_MODE_INTERRUPT:
            return (uart_rx_head != uart_rx_tail);
        default:
            return false;
    }
}

void UART_SendData(const uint8_t *data, uint16_t length) {
    switch(current_uart_mode) {
        case UART_MODE_NORMAL:
            UART_Normal_SendData(data, length) ;
            break;
        case UART_MODE_DMA:
            UART_DMA_SendData(data, length);
            break;
        case UART_MODE_INTERRUPT:
            UART_INT_SendData(data, length);
            break;
    }
}

// Send string
void UART_SendString(const char *str) {
    if (current_uart_mode == UART_MODE_NORMAL) {
        UART_Normal_SendData((const uint8_t*)str, strlen(str));
    } else if (current_uart_mode == UART_MODE_DMA) {
        UART_DMA_SendData((const uint8_t*)str, strlen(str));
    } else if (current_uart_mode == UART_MODE_INTERRUPT) {
        UART_INT_SendData((const uint8_t*)str, strlen(str));
    }
}

// Abstraction for UART_ReceiveData
uint16_t UART_ReceiveData(uint8_t *data, uint16_t max_length) {
    uint16_t count = 0;

    switch(current_uart_mode) {
        case UART_MODE_NORMAL:
            UART_Normal_ReceiveData(data, UART_RX_BUFFER_SIZE);
            break;

        case UART_MODE_INTERRUPT:
            while(count < max_length && uart_rx_head != uart_rx_tail) {
                data[count++] = uart_rx_buffer[uart_rx_tail];
                uart_rx_tail = (uart_rx_tail + 1) % UART_RX_BUFFER_SIZE;
            }
            break;

        case UART_MODE_DMA:
            // In DMA circular mode, we need to calculate received bytes
            {
                uint16_t ndtr = DMA1_S5NDTR;
                uint16_t received = UART_RX_BUFFER_SIZE - ndtr;
                uint16_t to_copy = (received > max_length) ? max_length : received;

                for(uint16_t i = 0; i < to_copy; i++) {
                    data[i] = uart_rx_buffer[i]; // assume simple linear copy for circular buffer
                }
                count = to_copy;
            }
            break;
    }

    return count;
}

// Restart DMA reception (useful after buffer overflow or re-init)
void UART_DMA_StartReceive(void)
{
    // Disable stream first
    DMA1_S5CR &= ~DMA_SxCR_EN;
    while(DMA1_S5CR & DMA_SxCR_EN);

    // Reset NDTR to buffer size
    DMA1_S5NDTR = UART_RX_BUFFER_SIZE;
    DMA1_S5M0AR = (uint32_t)uart_rx_buffer;

    // Clear interrupt flags
    DMA1_LIFCR |= 0x3D000000; // Clear all flags for Stream5

    // Enable stream
    DMA1_S5CR |= DMA_SxCR_EN;
}
// Interrupt handlers
extern void USART2_IRQHandler(void);
void USART2_IRQHandler(void) {
    // RX interrupt
    if(USART2_SR & USART_SR_RXNE) {
        uint8_t data = USART2_DR;
        uint16_t next_head = (uart_rx_head + 1) % UART_RX_BUFFER_SIZE;
        if(next_head != uart_rx_tail) {
            uart_rx_buffer[uart_rx_head] = data;
            uart_rx_head = next_head;
        }
    }
    
    // TX interrupt
    if((USART2_SR & USART_SR_TXE) && (USART2_CR1 & USART_CR1_TXEIE)) {
        if(uart_tx_head != uart_tx_tail) {
            USART2_DR = uart_tx_buffer[uart_tx_tail];
            uart_tx_tail = (uart_tx_tail + 1) % UART_TX_BUFFER_SIZE;
        } else {
            USART2_CR1 &= ~USART_CR1_TXEIE;
            USART2_CR1 |=  USART_CR1_TCIE;   
        }
    }

    // Transmission Complete
    if((USART2_SR & USART_SR_TC) && (USART2_CR1 & USART_CR1_TCIE)) {
        // USART2_SR &= ~USART_SR_TC;
        USART2_CR1 &= ~USART_CR1_TCIE;   
        uart_tx_busy = false;            
    }
}
static volatile bool dma_tx_done = false;
extern void DMA1_Stream6_IRQHandler(void);
void DMA1_Stream6_IRQHandler(void) {
    if(DMA1_HISR & (1 << 21)) {   // check TCIF6
        DMA1_HIFCR |= (1 << 21);   // clear flag
        dma_tx_done = true;        // báo hoàn tất
    }
}