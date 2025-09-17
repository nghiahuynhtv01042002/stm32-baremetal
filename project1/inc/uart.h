#ifndef UART_H
#define UART_H

#include "sysclocks.h"
#include "gpio.h"
#include "nvic.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
// UART Mode definitions
typedef enum {
    UART_MODE_NORMAL = 0,
    UART_MODE_DMA,
    UART_MODE_INTERRUPT
} UART_Mode_t;

// UART Configuration
typedef struct {
    uint32_t baudrate;
    UART_Mode_t mode;
    uint8_t *tx_buffer;
    uint8_t *rx_buffer;
    uint16_t tx_buffer_size;
    uint16_t rx_buffer_size;
} UART_Config_t;

// USART2 Base Address (PA2-TX, PA3-RX)
#define USART2_BASE         (0x40004400)
#define USART2_SR           (*(volatile uint32_t *)(USART2_BASE + 0x00))
#define USART2_DR           (*(volatile uint32_t *)(USART2_BASE + 0x04))
#define USART2_BRR          (*(volatile uint32_t *)(USART2_BASE + 0x08))
#define USART2_CR1          (*(volatile uint32_t *)(USART2_BASE + 0x0C))
#define USART2_CR2          (*(volatile uint32_t *)(USART2_BASE + 0x10))
#define USART2_CR3          (*(volatile uint32_t *)(USART2_BASE + 0x14))

// USART Status Register bits
#define USART_SR_PE         (1 << 0)
#define USART_SR_FE         (1 << 1)
#define USART_SR_NE         (1 << 2)
#define USART_SR_ORE        (1 << 3)
#define USART_SR_IDLE       (1 << 4)
#define USART_SR_RXNE       (1 << 5)
#define USART_SR_TC         (1 << 6)
#define USART_SR_TXE        (1 << 7)

// USART Control Register 1 bits
#define USART_CR1_SBK       (1 << 0)
#define USART_CR1_RWU       (1 << 1)
#define USART_CR1_RE        (1 << 2)
#define USART_CR1_TE        (1 << 3)
#define USART_CR1_IDLEIE    (1 << 4)
#define USART_CR1_RXNEIE    (1 << 5)
#define USART_CR1_TCIE      (1 << 6)
#define USART_CR1_TXEIE     (1 << 7)
#define USART_CR1_PEIE      (1 << 8)
#define USART_CR1_PS        (1 << 9)
#define USART_CR1_PCE       (1 << 10)
#define USART_CR1_WAKE      (1 << 11)
#define USART_CR1_M         (1 << 12)
#define USART_CR1_UE        (1 << 13)

// USART Control Register 3 bits
#define USART_CR3_EIE       (1 << 0)
#define USART_CR3_IREN      (1 << 1)
#define USART_CR3_IRLP      (1 << 2)
#define USART_CR3_HDSEL     (1 << 3)
#define USART_CR3_NACK      (1 << 4)
#define USART_CR3_SCEN      (1 << 5)
#define USART_CR3_DMAR      (1 << 6)
#define USART_CR3_DMAT      (1 << 7)
#define USART_CR3_RTSE      (1 << 8)
#define USART_CR3_CTSE      (1 << 9)
#define USART_CR3_CTSIE     (1 << 10)

// DMA1 Base Address
#define DMA1_BASE           (0x40026000)

// DMA1 Stream 5 (USART2_RX) - Channel 4
#define DMA1_S5CR           (*(volatile uint32_t *)(DMA1_BASE + 0x88))
#define DMA1_S5NDTR         (*(volatile uint32_t *)(DMA1_BASE + 0x8C))
#define DMA1_S5PAR          (*(volatile uint32_t *)(DMA1_BASE + 0x90))
#define DMA1_S5M0AR         (*(volatile uint32_t *)(DMA1_BASE + 0x94))
#define DMA1_S5FCR          (*(volatile uint32_t *)(DMA1_BASE + 0x9C))

// DMA1 Stream 6 (USART2_TX) - Channel 4
#define DMA1_S6CR           (*(volatile uint32_t *)(DMA1_BASE + 0xA0))
#define DMA1_S6NDTR         (*(volatile uint32_t *)(DMA1_BASE + 0xA4))
#define DMA1_S6PAR          (*(volatile uint32_t *)(DMA1_BASE + 0xA8))
#define DMA1_S6M0AR         (*(volatile uint32_t *)(DMA1_BASE + 0xAC))
#define DMA1_S6FCR          (*(volatile uint32_t *)(DMA1_BASE + 0xB4))

// DMA interrupt status registers
#define DMA1_LISR           (*(volatile uint32_t *)(DMA1_BASE + 0x00))
#define DMA1_HISR           (*(volatile uint32_t *)(DMA1_BASE + 0x04))
#define DMA1_LIFCR          (*(volatile uint32_t *)(DMA1_BASE + 0x08))
#define DMA1_HIFCR          (*(volatile uint32_t *)(DMA1_BASE + 0x0C))

// DMA Control Register bits
#define DMA_SxCR_EN         (1 << 0)
#define DMA_SxCR_DMEIE      (1 << 1)
#define DMA_SxCR_TEIE       (1 << 2)
#define DMA_SxCR_HTIE       (1 << 3)
#define DMA_SxCR_TCIE       (1 << 4)
#define DMA_SxCR_PFCTRL     (1 << 5)
#define DMA_SxCR_DIR_0      (1 << 6)
#define DMA_SxCR_DIR_1      (1 << 7)
#define DMA_SxCR_CIRC       (1 << 8)
#define DMA_SxCR_PINC       (1 << 9)
#define DMA_SxCR_MINC       (1 << 10)
#define DMA_SxCR_PSIZE_0    (1 << 11)
#define DMA_SxCR_PSIZE_1    (1 << 12)
#define DMA_SxCR_MSIZE_0    (1 << 13)
#define DMA_SxCR_MSIZE_1    (1 << 14)
#define DMA_SxCR_PINCOS     (1 << 15)
#define DMA_SxCR_PL_0       (1 << 16)
#define DMA_SxCR_PL_1       (1 << 17)
#define DMA_SxCR_DBM        (1 << 18)
#define DMA_SxCR_CT         (1 << 19)
#define DMA_SxCR_PBURST_0   (1 << 21)
#define DMA_SxCR_PBURST_1   (1 << 22)
#define DMA_SxCR_MBURST_0   (1 << 23)
#define DMA_SxCR_MBURST_1   (1 << 24)
#define DMA_SxCR_CHSEL_0    (1 << 25)
#define DMA_SxCR_CHSEL_1    (1 << 26)
#define DMA_SxCR_CHSEL_2    (1 << 27)

// RCC DMA enable
#define RCC_AHB1ENR_DMA1EN  (1 << 21)
#define RCC_APB1ENR_USART2EN (1 << 17)

// Buffer sizes
#define UART_TX_BUFFER_SIZE 256
#define UART_RX_BUFFER_SIZE 256

// Initial funtion
void UART_Normal_Init(uint32_t baudrate);
void UART_DMA_Init(uint32_t baudrate, uint8_t *tx_buf, uint8_t *rx_buf, uint16_t tx_size, uint16_t rx_size);
void UART_Interrupt_Init(uint32_t baudrate, uint8_t *tx_buf, uint8_t *rx_buf, uint16_t tx_size, uint16_t rx_size);

// Normal specific functions
extern void UART_Normal_SendData(const uint8_t *buffer, uint16_t length);
extern uint16_t UART_Normal_ReceiveData(uint8_t *buffer, uint16_t max_length);

// Interrupt specific functions
extern void UART_Interrupt_SendData(const uint8_t *buffer, uint16_t length);
extern uint16_t UART_Interrupt_ReceiveData(uint8_t *buffer, uint16_t max_length);
// DMA specific functions
extern void UART_DMA_SendData(const uint8_t *data, uint16_t length);
uint16_t UART_DMA_ReceiveData(UART_Config_t* uart_cfg, uint8_t *app_buffer, uint16_t max_length);

// Abstract Function prototypes
void UART_Init(UART_Config_t *config);
void UART_SendData(const uint8_t *data, uint16_t length);
uint16_t UART_ReceiveData(UART_Config_t* uart_cfg, uint8_t *data, uint16_t max_length);

// Global variables
extern volatile UART_Mode_t current_uart_mode;
extern volatile uint8_t uart_tx_buffer[UART_TX_BUFFER_SIZE];
extern volatile uint8_t uart_rx_buffer[UART_RX_BUFFER_SIZE];
extern volatile uint16_t uart_tx_head, uart_tx_tail;
extern volatile uint16_t uart_rx_head, uart_rx_tail;
extern volatile bool uart_tx_busy;

extern volatile bool dma_tx_done;
extern volatile bool dma_rx_overflow;
extern volatile uint16_t dma_rx_last_pos;


#endif /* UART_H */