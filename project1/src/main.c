#include <stdint.h>
#include "sysclocks.h"
#include "usbfs.h"
#include "gpio.h"
#include "nvic.h"
#include "uart.h"
// #define test_RX
#define test_TX
extern uint32_t SystemCoreClock;
extern void GPIOConfig(void);
extern void Config_MCO(void);
// int main(void) {
//     // init sysclock
//     SystemClockConfig();
//     // config MCO
//     Config_MCO();
//     GPIOConfig();
//     // Usb config
//     USB_GPIO_init();
//     USB_core_device_init();
//     GPIOx_Set_MODER(&GPIOD_MODER,14,0x01);
//     for(;;) {
//         // Toggle PD12
//         GPIOx_Toggle(&GPIOD_ODR,14);
//         delay_ms(1000);
//     }
// }

#ifdef test_TX
uint8_t tx_buf[UART_TX_BUFFER_SIZE];
uint8_t rx_buf[UART_RX_BUFFER_SIZE];
int main(void)
{
    SystemClockConfig();
    GPIOConfig();
    GPIOx_Set_MODER(&GPIOD_MODER,13,0x01);
    UART_Config_t uart_cfg;

    // --- Normal mode ---
    uart_cfg.mode = UART_MODE_NORMAL;
    uart_cfg.baudrate = 115200;
    uart_cfg.tx_buffer = NULL;
    uart_cfg.rx_buffer = NULL;
    uart_cfg.tx_buffer_size = 0;
    uart_cfg.rx_buffer_size = 0;
    UART_Init(&uart_cfg);

    UART_SendString("Hello from UART Normal mode!\r\n");

    // --- DMA mode ---
    uart_cfg.mode = UART_MODE_DMA;
    uart_cfg.tx_buffer = tx_buf;
    uart_cfg.rx_buffer = rx_buf;
    uart_cfg.tx_buffer_size = UART_TX_BUFFER_SIZE;
    uart_cfg.rx_buffer_size = UART_RX_BUFFER_SIZE;
    UART_Init(&uart_cfg);

    UART_SendString("Hello from UART DMA mode!\r\n");

    // --- Interrupt mode ---
    uart_cfg.mode = UART_MODE_INTERRUPT;
    uart_cfg.tx_buffer = tx_buf;
    uart_cfg.rx_buffer = rx_buf;
    uart_cfg.tx_buffer_size = UART_TX_BUFFER_SIZE;
    uart_cfg.rx_buffer_size = UART_RX_BUFFER_SIZE;
    UART_Init(&uart_cfg);

    UART_SendString("Hello from UART Interrupt mode!\r\n");

    while(1)
    {
        // Toggle PD12
        GPIOx_Toggle(&GPIOD_ODR,13);
        delay_ms(1000);
    }
}
// need to update test TX here for now Interrupt mode 
#endif //test_TX
#ifdef test_RX
// need to test RX here for now Normal mode for RX is oke

#endif