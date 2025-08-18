#ifndef USB_FS_H
#define USB_FS_H
#include "sysclocks.h"
#include "gpio.h"
#define NVIC_BASE       (0xE000E100UL)
#define USB_OTG_FS_BASE (0x50000000UL)
// USB OTG Registers
#define USB_GOTGCTL         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x000))
#define USB_GOTGINT         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x004))
#define USB_GAHBCFG         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x008))
#define USB_GUSBCFG         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x00C))
#define USB_GRSTCTL         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x010))
#define USB_GINTSTS         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x014))
#define USB_GINTMSK         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x018))
#define USB_GRXSTSR         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x01C))
#define USB_GRXSTSP         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x020))
#define USB_GRXFSIZ         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x024))
#define USB_HNPTXFSIZ       (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x028))
#define USB_GCCFG           (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x038))

// Device Mode Registers
#define USB_DCFG            (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x800))
#define USB_DCTL            (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x804))
#define USB_DSTS            (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x808))
#define USB_DIEPMSK         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x810))
#define USB_DOEPMSK         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x814))
#define USB_DAINT           (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x818))
#define USB_DAINTMSK        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x81C))

// IN Endpoint 0 Registers
#define USB_DIEPCTL0        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x900))
#define USB_DIEPINT0        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x908))
#define USB_DIEPTSIZ0       (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x910))
#define USB_DTXFSTS0        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x918))

// OUT Endpoint 0 Registers  
#define USB_DOEPCTL0        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0xB00))
#define USB_DOEPINT0        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0xB08))
#define USB_DOEPTSIZ0       (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0xB10))

// IN Endpoint 1 Registers
#define USB_DIEPCTL1        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x920))
#define USB_DIEPINT1        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x928))
#define USB_DIEPTSIZ1       (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x930))
#define USB_DTXFSTS1        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x938))

// OUT Endpoint 1 Registers
#define USB_DOEPCTL1        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0xB20))
#define USB_DOEPINT1        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0xB28))
#define USB_DOEPTSIZ1       (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0xB30))

// IN Endpoint 2 Registers  
#define USB_DIEPCTL2        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x940))
#define USB_DIEPINT2        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x948))
#define USB_DIEPTSIZ2       (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x950))
#define USB_DTXFSTS2        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x958))

// FIFO Registers
#define USB_DIEPTXF1        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x104))
#define USB_DIEPTXF2        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x108))

// FIFO Access
#define USB_FIFO0           (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x1000))
#define USB_FIFO1           (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x2000))
#define USB_FIFO2           (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x3000))

// NVIC Registers
#define NVIC_ISER1          (*(volatile uint32_t *)(NVIC_BASE + 0x04))

// USB States
typedef enum {
    USB_STATE_DEFAULT = 0,
    USB_STATE_ADDRESSED,
    USB_STATE_CONFIGURED
} USB_State_t;

// USB sturcture packet
typedef struct {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} USB_SetupPacket_t;
// decriptor
extern const uint8_t usb_device_desc[];
extern const uint8_t usb_device_desc[];
extern const uint8_t usb_config_desc[];

// String Descriptors
extern const uint8_t usb_string_langid[];
extern const uint8_t usb_string_mfr[];
extern const uint8_t usb_string_product[]; 

// Global variables
extern volatile uint8_t usb_device_state ;
extern volatile uint8_t usb_config_value ;
extern volatile uint8_t usb_device_address ;
extern USB_SetupPacket_t setup_packet;
extern uint8_t ep0_buffer[64];
extern uint8_t rx_buffer[64];
extern uint8_t tx_buffer[64];
extern volatile uint16_t rx_count;
extern volatile uint8_t tx_ready;

extern void USB_GPIO_init(void);
extern void USB_core_init(void);

#endif // USB_FS_H