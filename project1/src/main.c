// main.c
#include <stdint.h>
#include "sysclocks.h"
#include "usbfs.h"
#include "gpio.h"

extern uint32_t SystemCoreClock;
extern void GPIOConfig(void);
extern void Config_MCO(void);
int main(void) {
    // init sysclock
    SystemClockConfig();
    // config MCO
    Config_MCO();
    GPIOConfig();
    // Usb config
    USB_GPIO_init();
    USB_core_device_init();
    GPIOx_Set_MODER(&GPIOD_MODER,13,0x01);
    for(;;) {
        // Toggle PD12
        GPIOx_Toggle(&GPIOD_ODR,13);
        delay_ms(1000);
    }
}
