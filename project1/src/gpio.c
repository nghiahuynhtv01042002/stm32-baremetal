#include "gpio.h"
#include "sysclocks.h"

void GPIOConfig(){
    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOD_EN;
}
void GPIOx_Set_MODER(volatile uint32_t* GPIOx_MODER, uint8_t pinNumber) {
    // Set pin as output
    *GPIOx_MODER |= (1 << (pinNumber*2) );
    *GPIOx_MODER &= ~(1 << (pinNumber*2 +1));
}

void GPIOx_Toggle(volatile uint32_t * GPIOx_ODR, uint8_t pinNumber) {
    *GPIOx_ODR ^= (1 << pinNumber);
}