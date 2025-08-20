#ifndef USB_FS_H
#define USB_FS_H
#include "sysclocks.h"
#include "gpio.h"
#define NVIC_BASE       (0xE000E100UL)
#define USB_OTG_FS_BASE (0x50000000UL)
// USB OTG Registers
#define OTG_FS_GOTGCTL         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x000))
#define OTG_FS_GOTGINT         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x004))
#define OTG_FS_GAHBCFG         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x008))
#define OTG_FS_GUSBCFG         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x00C))
#define OTG_FS_GRSTCTL         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x010))
#define OTG_FS_GINTSTS         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x014))
#define OTG_FS_GINTMSK         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x018))
#define OTG_FS_GRXSTSR         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x01C))
#define OTG_FS_GRXSTSP         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x020))
#define OTG_FS_GRXFSIZ         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x024))
#define OTG_FS_DIEPTXF0       (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x028))
#define OTG_FS_GCCFG           (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x038))

#define OTG_FS_GAHBCFG_GINTMSK_EN (1 << 0)
#define OTG_FS_GINTSTS_RXFLVL_MSK (1 << 4)
#define OTG_FS_GRSTCTL_AHBIDL (1 << 31)
#define OTG_FS_GRSTCTL_CSRST  (1 << 0)
#define OTG_FS_GUSBCFG_FHMOD  (1 << 29)
#define OTG_FS_GUSBCFG_FDMOD  (1 << 30)
#define OTG_FS_GUSBCFG_TRDT   (0x6 << 10)

#define OTG_FS_GCCFG_PWRDWN (1 << 16)
#define OTG_FS_GCCFG_VBUSASEN (1 << 18)
#define OTG_FS_GCCFG_VBUSBSEN (1 << 19)
#define OTG_FS_GCCFG_NOVBUSSENS (1 << 21)

#define OTG_FS_GRXFSIZ_RXFD    ((128ul & 0xFFFF ) << 0)
#define OTG_FS_DIEPTXF0_TX0FD ((64ul & 0xFFFF ) << 16)
#define OTG_FS_DIEPTXF0_TX0FSA ((128ul & 0xFFFF) << 0)
// Device Mode Registers
#define OTG_FS_DCFG            (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x800))
#define OTG_FS_DCTL            (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x804))
#define OTG_FS_DSTS            (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x808))
#define OTG_FS_DIEPMSK         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x810))
#define OTG_FS_DOEPMSK         (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x814))
#define OTG_FS_DAINT           (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x818))
#define OTG_FS_DAINTMSK        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x81C))

#define OTG_FS_DCFG_DSPD       (0x3 << 0)
#define OTG_FS_DCFG_NZLSOHSK   (1 << 2)

// IN Endpoint 0 Registers
#define OTG_FS_DIEPCTL0        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x900))
#define OTG_FS_DIEPINT0        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x908))
#define OTG_FS_DIEPTSIZ0       (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x910))
#define OTG_FS_DTXFSTS0        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x918))

// OUT Endpoint 0 Registers  
#define OTG_FS_DOEPCTL0        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0xB00))
#define OTG_FS_DOEPINT0        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0xB08))
#define OTG_FS_DOEPTSIZ0       (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0xB10))

// IN Endpoint 1 Registers
#define OTG_FS_DIEPCTL1        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x920))
#define OTG_FS_DIEPINT1        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x928))
#define OTG_FS_DIEPTSIZ1       (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x930))
#define OTG_FS_DTXFSTS1        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x938))

// OUT Endpoint 1 Registers
#define OTG_FS_DOEPCTL1        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0xB20))
#define OTG_FS_DOEPINT1        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0xB28))
#define OTG_FS_DOEPTSIZ1       (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0xB30))

// IN Endpoint 2 Registers  
#define OTG_FS_DIEPCTL2        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x940))
#define OTG_FS_DIEPINT2        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x948))
#define OTG_FS_DIEPTSIZ2       (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x950))
#define OTG_FS_DTXFSTS2        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x958))

// FIFO Registers
#define OTG_FS_DIEPTXF1        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x104))
#define OTG_FS_DIEPTXF2        (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x108))

// FIFO Access
#define OTG_FS_FIFO0           (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x1000))
#define OTG_FS_FIFO1           (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x2000))
#define OTG_FS_FIFO2           (*(volatile uint32_t *)(USB_OTG_FS_BASE + 0x3000))

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
extern void USB_core_device_init(void);

#endif // USB_FS_H