#include <stdint.h>
#include "sysclocks.h"
#include "usbfs.h"
#include "gpio.h"
#include "nvic.h"
#include "uart.h"
// #define NORMAL
// #define INTERRUPT
#define DMA
extern uint32_t SystemCoreClock;
extern void GPIOConfig(void);
extern void Config_MCO(void);
extern void UART_Normal_SendData(const uint8_t *str,uint16_t length);
uint8_t tx_buf[UART_TX_BUFFER_SIZE];
uint8_t rx_buf[UART_RX_BUFFER_SIZE];

void UART_DMA_EchoTask(UART_Config_t* uart_cfg) {
    static uint8_t data_DMA[UART_RX_BUFFER_SIZE];
    uint16_t len = UART_DMA_ReceiveData(uart_cfg,data_DMA, UART_RX_BUFFER_SIZE);
    if (len > 0) {
        UART_DMA_SendData("echo :",8);
        UART_DMA_SendData((const uint8_t *)data_DMA,len);
    } 
}

void UART_Normal_EchoTask(UART_Config_t* uart_cfg) {
    static uint8_t data_normal[UART_RX_BUFFER_SIZE];
    memset(data_normal,'0',UART_RX_BUFFER_SIZE);
    uint16_t len = UART_Normal_ReceiveData(data_normal, UART_RX_BUFFER_SIZE);
    if(len > 0) {
        UART_Normal_SendData("echo :",8);
        UART_Normal_SendData((const uint8_t *)data_normal,sizeof(data_normal));
    }

}
void UART_Interrupt_EchoTask(UART_Config_t* uart_cfg) {
    static uint8_t data_interrupt[UART_RX_BUFFER_SIZE];
    memset(data_interrupt,'0',UART_RX_BUFFER_SIZE);
    uint16_t len = UART_Interrupt_ReceiveData(data_interrupt, UART_RX_BUFFER_SIZE);
    if (len > 0) {
        UART_Interrupt_SendData("echo :",8);
        UART_Interrupt_SendData((const uint8_t *)data_interrupt,sizeof(data_interrupt));
    }

}
int main(void) {
    GPIOConfig();
    GPIOx_Set_MODER(&GPIOD_MODER, 13, 0x01);
    UART_Config_t uart_cfg;
#ifdef INTERRUPT
    // ===== Interrupt =====
    uart_cfg.mode = UART_MODE_INTERRUPT;
    uart_cfg.baudrate = 115200;
    uart_cfg.tx_buffer = tx_buf;
    uart_cfg.rx_buffer = rx_buf;
    uart_cfg.tx_buffer_size = UART_TX_BUFFER_SIZE;
    uart_cfg.rx_buffer_size = UART_RX_BUFFER_SIZE;

    UART_Init(&uart_cfg);

    UART_Interrupt_SendData("UART Interupt Echo Test Start\r\n",32);
    while(1) {
        UART_Interrupt_EchoTask(&uart_cfg);
        delay_ms(1000);
    }
#endif
 
#ifdef NORMAL
    // ===== Normal =====
    uart_cfg.mode = UART_MODE_NORMAL;
    uart_cfg.baudrate = 115200;
    uart_cfg.tx_buffer = tx_buf;
    uart_cfg.rx_buffer = rx_buf;
    uart_cfg.tx_buffer_size = UART_TX_BUFFER_SIZE;
    uart_cfg.rx_buffer_size = UART_RX_BUFFER_SIZE;

    UART_Init(&uart_cfg);

    UART_Normal_SendData("UART Normal Echo Test Start\r\n",30);
    while(1) {
        UART_Normal_EchoTask(&uart_cfg);
        delay_ms(1000);
    }
#endif
#ifdef DMA
    // ===== DMA =====
    uart_cfg.mode = UART_MODE_DMA;
    uart_cfg.baudrate = 115200;
    uart_cfg.tx_buffer = tx_buf;
    uart_cfg.rx_buffer = rx_buf;
    uart_cfg.tx_buffer_size = UART_TX_BUFFER_SIZE;
    uart_cfg.rx_buffer_size = UART_RX_BUFFER_SIZE;

    UART_Init(&uart_cfg);
    UART_DMA_SendData("UART DMA Echo Test Start\r\n",27);
    while(1) {
        UART_DMA_EchoTask(&uart_cfg);
        delay_ms(1000);
    }
#endif

}