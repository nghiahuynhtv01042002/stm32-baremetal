// main.c
#include <stdint.h>
#include "sysclocks.h"
#include "usbfs.h"
#include "gpio.h"

extern uint32_t SystemCoreClock;
extern void GPIOConfig(void);
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

extern void Config_MCO(void);
int main(void) {
    // init sysclock
    SystemClockConfig();
    // config MCO
    Config_MCO();
    GPIOConfig();
    GPIOx_Set_MODER(&GPIOD_MODER,14);
    for(;;) {
        // Toggle PD12
        GPIOx_Toggle(&GPIOD_ODR,14);
        delay_ms(1000);
    }
}
