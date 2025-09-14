#include "gpio.h"
#include "sysclocks.h"

void GPIOConfig(){
    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOD_EN;
}

/*
set mode for GPIO pin
@param volatile uint32_t* GPIOx_MODER : GPIOx_MODER address register
@param uint8_t pinNumber : GPIO pin number 
@param uint8_t mode:
             0x00 - Input 
             0x01 - Output
             0x10 - Alternative function
             0x11 - Analog
*/
void GPIOx_Set_MODER(volatile uint32_t* GPIOx_MODER, uint8_t pinNumber, uint8_t mode) {
    uint32_t shift = pinNumber * 2;
    uint32_t mask  = 0x3 << shift;   
    *GPIOx_MODER &= ~mask;           
    *GPIOx_MODER |= ((mode & 0x3) << shift); 
}
void GPIOx_write(volatile uint32_t * GPIOx_ODR, uint8_t pinNumber) {
    *GPIOx_ODR |= (1 << pinNumber);
}
void GPIOx_Toggle(volatile uint32_t * GPIOx_ODR, uint8_t pinNumber) {
    *GPIOx_ODR ^= (1 << pinNumber);
}