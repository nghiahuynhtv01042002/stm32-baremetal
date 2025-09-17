#ifndef ISR_H
#include "nvic.h"
#include "uart.h"
#include "sysclocks.h"
#include "gpio.h"

extern void USART2_IRQHandler(void);
extern void DMA1_Stream6_IRQHandler(void);
extern void DMA1_Stream5_IRQHandler(void);
#endif // ISR_H