#include "sysclocks.h"
#include "usbfs.h"

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
    GPIOA_AFRH &= ~((0x0F << 12) | (0x0F << 16)) ;
    GPIOA_AFRH |= ((0x0A << 12) | (0x0A << 16));
}
void USB_core_init(){
    RCC_AHB2ENR |=RCC_AHB2ENR_OTGFSEN;
}