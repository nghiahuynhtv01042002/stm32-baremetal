#ifndef NVIC_H
#define NVIC_H

#include <stdint.h>

// NVIC Base Address
#define NVIC_BASE           (0xE000E100)

// NVIC Registers
#define NVIC_ISER0          (*(volatile uint32_t *)(NVIC_BASE + 0x000))
#define NVIC_ISER1          (*(volatile uint32_t *)(NVIC_BASE + 0x004))
#define NVIC_ISER2          (*(volatile uint32_t *)(NVIC_BASE + 0x008))

#define NVIC_ICER0          (*(volatile uint32_t *)(NVIC_BASE + 0x080))
#define NVIC_ICER1          (*(volatile uint32_t *)(NVIC_BASE + 0x084))
#define NVIC_ICER2          (*(volatile uint32_t *)(NVIC_BASE + 0x088))

#define NVIC_ISPR0          (*(volatile uint32_t *)(NVIC_BASE + 0x100))
#define NVIC_ISPR1          (*(volatile uint32_t *)(NVIC_BASE + 0x104))
#define NVIC_ISPR2          (*(volatile uint32_t *)(NVIC_BASE + 0x108))

#define NVIC_ICPR0          (*(volatile uint32_t *)(NVIC_BASE + 0x180))
#define NVIC_ICPR1          (*(volatile uint32_t *)(NVIC_BASE + 0x184))
#define NVIC_ICPR2          (*(volatile uint32_t *)(NVIC_BASE + 0x188))

// Priority Register Base
#define NVIC_IPR_BASE       (NVIC_BASE + 0x300)

// IRQ Numbers for STM32F411
#define USART1_IRQn         37
#define USART2_IRQn         38
#define USART6_IRQn         71
#define DMA1_Stream5_IRQn   16
#define DMA1_Stream6_IRQn   17
#define DMA2_Stream6_IRQn   69
#define DMA2_Stream7_IRQn   70

// Function prototypes
void NVIC_EnableIRQ(uint8_t IRQn);
void NVIC_DisableIRQ(uint8_t IRQn);
void NVIC_SetPriority(uint8_t IRQn, uint8_t priority);
void NVIC_SetPendingIRQ(uint8_t IRQn);
void NVIC_ClearPendingIRQ(uint8_t IRQn);

#endif /* NVIC_H */
