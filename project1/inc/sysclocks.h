#ifndef SYS_CLOCKS_H
#define SYS_CLOCKS_H
#include <stdint.h>

// RCC Register definitions
#define RCC_BASE      (0x40023800)
#define RCC_CR        (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_PLLCFGR   (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CFGR      (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_AHB1RSTR  (*(volatile uint32_t *)(RCC_BASE + 0x10))
#define RCC_AHB2RSTR  (*(volatile uint32_t *)(RCC_BASE + 0x14))
#define RCC_AHB1ENR   (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define RCC_AHB2ENR   (*(volatile uint32_t *)(RCC_BASE + 0x34))
#define RCC_APB1ENR   (*(volatile uint32_t *)(RCC_BASE + 0x40))
// RCC_CR bits
#define RCC_CR_HSEON  (1 << 16)
#define RCC_CR_HSERDY (1 << 17)
#define RCC_CR_PLLON  (1 << 24)
#define RCC_CR_PLLRDY (1 << 25)

// RCC_PLLCFGR bits and values
#define RCC_PLLSRC    (1 << 22)  // HSE as PLL source
#define RCC_PLLM      (8)        // PLLM = 8
#define RCC_PLLN      (192 << 6) // PLLN = 192
#define RCC_PLLP      (0 << 16)  // PLLP = 2 (00 = /2) 
#define RCC_PLLQ      (4 << 24)  // PLLQ = 4

// RCC_CFGR bits and values
#define RCC_HPRE      (0x00 << 4)  // AHB prescaler = 1
#define RCC_PPRE1     (0x04 << 10) // APB1 prescaler = 2
#define RCC_PPRE2     (0x04 << 13) // APB2 prescaler = 2
#define RCC_SW        (0x02 << 0)  // PLL as system clock
#define RCC_SWS_MASK  (0x03 << 2)  // System clock switch status mask
#define RCC_SWS_PLL   (0x02 << 2)  // PLL used as system clock

// MCO configuration (optional)
#define RCC_MCO1      (0x3 << 21)  // MCO1 source
#define RCC_MCO1PRE   (0x06 << 24) // MCO1 prescaler
#define RCC_MCO2      (0x00 << 30) // MCO2 source
#define RCC_MCO2PRE   (0x06 << 27) // MCO2 prescaler

// GPIO enable bits
#define RCC_AHB1ENR_GPIOA_EN (1 << 0)
#define RCC_AHB1ENR_GPIOC_EN (1 << 2)
#define RCC_AHB1ENR_GPIOD_EN (1 << 3)
// USB FS enable bit 
#define RCC_AHB2ENR_OTGFSEN (1 << 7)
// FLASH configuration
#define FLASH_BASE    (0x40023C00)
#define FLASH_ACR     (*(volatile uint32_t *)(FLASH_BASE + 0x00))
#define FLASH_ACR_LATENCY_3WS (0x03) 
extern uint32_t SystemCoreClock;
extern void SystemClockConfig(void);
extern void Config_MCO(void);
extern void SystemCoreClockUpdate(void);

static inline void delay_ms(uint32_t ms) {
    uint32_t cycles = (SystemCoreClock / 3000) * ms;
    __asm__ volatile (
        "1: subs %[cycles], %[cycles], #1\n"
        "   bne 1b\n"
        : [cycles] "+r" (cycles)
        :
        : "cc"
    );
}
#endif /* SYS_CLOCKS_H */