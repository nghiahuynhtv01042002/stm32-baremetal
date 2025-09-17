#ifndef GPIO_H
#define GPIO_H
#include "sysclocks.h"
#include <stdint.h>
// GPIOA configuration
#define GPIOA_BASE      (0x40020000)
#define GPIOA_MODER     (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_OSPEEDR   (*(volatile uint32_t *)(GPIOA_BASE + 0x08))
#define GPIOA_AFRL      (*(volatile uint32_t *)(GPIOA_BASE + 0x20))
#define GPIOA_AFRH      (*(volatile uint32_t *)(GPIOA_BASE + 0x24))
#define GPIOA_PUPDR     (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))
// GPIOC configuration
#define GPIOC_BASE      (0x40020800)
#define GPIOC_MODER     (*(volatile uint32_t *)(GPIOC_BASE + 0x00))
#define GPIOC_AFRH      (*(volatile uint32_t *)(GPIOC_BASE + 0x24))
// GPIOD configuration
#define GPIOD_BASE      (0x40020C00)
#define GPIOD_MODER     (*(volatile uint32_t *)(GPIOD_BASE + 0x00))
#define GPIOD_ODR       (*(volatile uint32_t *)(GPIOD_BASE + 0x14))

void GPIOConfig(void);
void GPIOx_Set_MODER(volatile uint32_t* GPIOx_MODER, uint8_t pinNumber, uint8_t mode);
void GPIOx_Toggle(volatile uint32_t * GPIOx_ODR, uint8_t pinNumber);
#endif /* GPIO_H */
