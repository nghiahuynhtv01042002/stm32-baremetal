#include "nvic.h"

void NVIC_EnableIRQ(uint8_t IRQn)
{
    if (IRQn < 32) {
        NVIC_ISER0 |= (1 << IRQn);
    } else if (IRQn < 64) {
        NVIC_ISER1 |= (1 << (IRQn - 32));
    } else if (IRQn < 96) {
        NVIC_ISER2 |= (1 << (IRQn - 64));
    }
}

void NVIC_DisableIRQ(uint8_t IRQn)
{
    if (IRQn < 32) {
        NVIC_ICER0 |= (1 << IRQn);
    } else if (IRQn < 64) {
        NVIC_ICER1 |= (1 << (IRQn - 32));
    } else if (IRQn < 96) {
        NVIC_ICER2 |= (1 << (IRQn - 64));
    }
}

void NVIC_SetPriority(uint8_t IRQn, uint8_t priority)
{
    volatile uint8_t *ipr = (volatile uint8_t *)(NVIC_IPR_BASE + IRQn);
    *ipr = (priority << 4);
}

void NVIC_SetPendingIRQ(uint8_t IRQn)
{
    if (IRQn < 32) {
        NVIC_ISPR0 |= (1 << IRQn);
    } else if (IRQn < 64) {
        NVIC_ISPR1 |= (1 << (IRQn - 32));
    } else if (IRQn < 96) {
        NVIC_ISPR2 |= (1 << (IRQn - 64));
    }
}

void NVIC_ClearPendingIRQ(uint8_t IRQn)
{
    if (IRQn < 32) {
        NVIC_ICPR0 |= (1 << IRQn);
    } else if (IRQn < 64) {
        NVIC_ICPR1 |= (1 << (IRQn - 32));
    } else if (IRQn < 96) {
        NVIC_ICPR2 |= (1 << (IRQn - 64));
    }
}
