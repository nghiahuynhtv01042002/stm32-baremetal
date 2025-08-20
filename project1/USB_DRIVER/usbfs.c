#include "sysclocks.h"
#include "usbfs.h"
#include "gpio.h"
// USB Device Descriptor (ST VCP compatible)
const uint8_t usb_device_desc[] = {
    18,         // bLength
    0x01,       // bDescriptorType
    0x00, 0x02, // bcdUSB (USB 2.0)
    0x02,       // bDeviceClass (CDC)
    0x02,       // bDeviceSubClass
    0x01,       // bDeviceProtocol
    64,         // bMaxPacketSize0
    0x83, 0x04, // idVendor (STMicroelectronics)
    0x40, 0x57, // idProduct (ST VCP)
    0x00, 0x02, // bcdDevice
    1,          // iManufacturer
    2,          // iProduct
    3,          // iSerialNumber
    1           // bNumConfigurations
};

// USB Configuration Descriptor (CDC)
const uint8_t usb_config_desc[] = {
    // Configuration Descriptor
    9, 0x02, 67, 0x00, 0x02, 0x01, 0x00, 0xC0, 0x32,
    
    // Interface Descriptor (Control Interface)
    9, 0x04, 0x00, 0x00, 0x01, 0x02, 0x02, 0x01, 0x00,
    
    // CDC Header Functional Descriptor
    5, 0x24, 0x00, 0x10, 0x01,
    
    // CDC Call Management Functional Descriptor
    5, 0x24, 0x01, 0x00, 0x01,
    
    // CDC ACM Functional Descriptor
    4, 0x24, 0x02, 0x02,
    
    // CDC Union Functional Descriptor
    5, 0x24, 0x06, 0x00, 0x01,
    
    // Endpoint Descriptor (Control IN)
    7, 0x05, 0x82, 0x03, 0x08, 0x00, 0x10,
    
    // Interface Descriptor (Data Interface)
    9, 0x04, 0x01, 0x00, 0x02, 0x0A, 0x00, 0x00, 0x00,
    
    // Endpoint Descriptor (Data OUT)
    7, 0x05, 0x01, 0x02, 0x40, 0x00, 0x00,
    
    // Endpoint Descriptor (Data IN)
    7, 0x05, 0x81, 0x02, 0x40, 0x00, 0x00
};

// String Descriptors
const uint8_t usb_string_langid[] = {4, 0x03, 0x09, 0x04};
const uint8_t usb_string_mfr[] = {18, 0x03, 'S',0,'T',0,'M',0,'i',0,'c',0,'r',0,'o',0,'s',0};
const uint8_t usb_string_product[] = {22, 0x03, 'S',0,'T',0,'M',0,'3',0,'2',0,' ',0,'V',0,'C',0,'P',0};


void USB_GPIO_init(void){
    // Enable GPIOA clock
    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOA_EN;
    // Set pin PA11, PA12 as AF10 (Mode 10b, AF10 1010b)
    GPIOA_MODER &= ~((1 << 22) | (1 << 24));
    GPIOA_MODER |= ((1<<23) | (1 << 25));
    // Clear a word 32bits for PA11 and PA12
    GPIOA_AFRH &= ~((0x0F << 12) | (0x0F << 16));
    GPIOA_AFRH |= ((0x0A << 12) | (0x0A << 16));
    // Set high speed
    GPIOA_OSPEEDR |= ((0x03 << 22 ) | (0x03 << 24));
    // Set No pull up / pull down
    GPIOA_PUPDR |= ((0x03 << 22 ) | (0x03 << 24));
}
void USB_core_device_init(){
    // Enable RCC
    RCC_AHB2ENR |= RCC_AHB2ENR_OTGFSEN;
    // Wait until AHB is idle
    while (!(OTG_FS_GRSTCTL & OTG_FS_GRSTCTL_AHBIDL));
    // Soft Reset USB core
    OTG_FS_GRSTCTL|= OTG_FS_GRSTCTL_CSRST;
    // Reset process is not imediately. It took several clock so we have to wait until \
    bit OTG_FS_GRSTCTL_CSRST(bit 0) is 0
    while(OTG_FS_GRSTCTL & OTG_FS_GRSTCTL_CSRST);
    // Wait until AHB is idle
    while (!(OTG_FS_GRSTCTL & OTG_FS_GRSTCTL_AHBIDL));
    // Set device mode and clear host mode
    OTG_FS_GUSBCFG |= OTG_FS_GUSBCFG_FDMOD;
    OTG_FS_GUSBCFG &= ~OTG_FS_GUSBCFG_FHMOD;
    // Set USB turnaround time 
    // @Note: i am not sure about the turnaround time 
    OTG_FS_GUSBCFG &= ~(0x0f << 10);
    OTG_FS_GUSBCFG |= OTG_FS_GUSBCFG_TRDT;
    // Enable VBUS sensing
    OTG_FS_GCCFG |= OTG_FS_GCCFG_VBUSBSEN;
    OTG_FS_GCCFG &= ~ OTG_FS_GCCFG_NOVBUSSENS;
    OTG_FS_GCCFG |= OTG_FS_GCCFG_PWRDWN; // power up PHY
    // Unmask interrupts: reset, enum done, SOF
    OTG_FS_GINTMSK = 0; // clear first
    OTG_FS_GINTMSK |= (1 << 12)  // USB reset
                    | (1 << 13)  // Enumeration done
                    | (1 << 3);  // Start-of-Frame
    // Configure DCFG
    OTG_FS_DCFG &= ~0x3;                 // clear speed bits
    OTG_FS_DCFG |= OTG_FS_DCFG_DSPD;     // full-speed device
    OTG_FS_DCFG |= OTG_FS_DCFG_NZLSOHSK; // non-zero-length status OUT handshake
    // Set RX FIFO size (example 128 32-bit words)
    OTG_FS_GRXFSIZ |= OTG_FS_GRXFSIZ_RXFD;
    // Set Non-periodic TX FIFO (example 64 words) // endpoint 0 rx
    OTG_FS_DIEPTXF0 |= (OTG_FS_DIEPTXF0_TX0FD | OTG_FS_DIEPTXF0_TX0FSA);
    // Core device mode ready
}