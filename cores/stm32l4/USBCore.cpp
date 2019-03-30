/*
 * Copyright (c) 2016 Thomas Roell.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal with the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimers.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimers in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of Thomas Roell, nor the names of its contributors
 *     may be used to endorse or promote products derived from this Software
 *     without specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * WITH THE SOFTWARE.
 */

#include "Arduino.h"
#include "stm32l4_wiring_private.h"
#include "USBAPI.h"

#if defined(USBCON)

void USBDeviceClass::init()
{
#if defined(USB_CLASS)
    USBD_Initialize((const uint8_t*)USB_MANUFACTURER, (const uint8_t*)USB_PRODUCT, USB_CLASS, STM32L4_CONFIG_USB_VBUS, STM32L4_USB_IRQ_PRIORITY);

#if (USB_TYPE == USB_TYPE_CDC_DAP) || (USB_TYPE == USB_TYPE_CDC_MSC_DAP)
    stm32l4_usbd_dap_initialize(STM32L4_CONFIG_DAP_SWCLK, STM32L4_CONFIG_DAP_SWDIO);
#endif
#endif

    initialized = true;
}

bool USBDeviceClass::attach()
{
#if defined(USB_CLASS)
    if (!initialized)
	return false;

    USBD_Attach();

    return true;
#else
    return false;
#endif
}

bool USBDeviceClass::detach()
{
#if defined(USB_CLASS)
    if (!initialized)
	return false;

    USBD_Detach();

    return true;
#else
    return false;
#endif
}

void USBDeviceClass::poll()
{
#if defined(USB_CLASS)
    if (initialized) {
	USBD_Poll();
    }
#endif
}
    
bool USBDeviceClass::connected()
{
    return USBD_Connected();
}

bool USBDeviceClass::configured()
{
    return USBD_Configured();
}

bool USBDeviceClass::suspended()
{
    return USBD_Suspended();
}

USBDeviceClass USBDevice;

extern "C" {

extern const uint8_t USBD_DeviceDescriptor[0x12] = {
  0x12,                       /* bLength */
  0x01,                       /* bDescriptorType */
#ifdef USB_CLASS_WEBUSB  
  0x01, 0x02,                 /* bcdUSB */
#else
  0x00, 0x02,                 /* bcdUSB */
#endif  
  0xef,                       /* bDeviceClass */
  0x02,                       /* bDeviceSubClass */
  0x01,                       /* bDeviceProtocol */
  64,                         /* bMaxPacketSize */
  LOBYTE(USB_VID),            /* idVendor */
  HIBYTE(USB_VID),            /* idVendor */
  LOBYTE(USB_PID),            /* idProduct */
  HIBYTE(USB_PID),            /* idProduct */
  0x00, 0x02,                 /* bcdDevice rel. 2.00 */
  1,                          /* Index of manufacturer string */
  2,                          /* Index of product string */
  3,                          /* Index of serial number string */
  1,                          /* bNumConfigurations */
};

#define MS_OS_20_SET_HEADER_DESCRIPTOR 0x00
#define MS_OS_20_SUBSET_HEADER_CONFIGURATION 0x01
#define MS_OS_20_SUBSET_HEADER_FUNCTION 0x02
#define MS_OS_20_FEATURE_COMPATIBLE_ID 0x03
#define MS_OS_20_FEATURE_REG_PROPERTY 0x04
#define MS_OS_20_DESCRIPTOR_LENGTH 0xb2

#ifdef USB_CLASS_WEBUSB
static const uint8_t USBD_BOS_DESCRIPTOR[] = {
  0x05,  // Length
  0x0F,  // Binary Object Store descriptor
  0x39, 0x00,  // Total length
  0x02,  // Number of device capabilities

  // WebUSB Platform Capability descriptor (bVendorCode == 0x01).
  0x18,  // Length
  0x10,  // Device Capability descriptor
  0x05,  // Platform Capability descriptor
  0x00,  // Reserved
  0x38, 0xB6, 0x08, 0x34, 0xA9, 0x09, 0xA0, 0x47,
  0x8B, 0xFD, 0xA0, 0x76, 0x88, 0x15, 0xB6, 0x65,  // WebUSB GUID
  0x00, 0x01,  // Version 1.0
  0x01,  // Vendor request code
  1, // we have a Landing page

  // Microsoft OS 2.0 Platform Capability Descriptor (MS_VendorCode == 0x02)
  0x1C,  // Length
  0x10,  // Device Capability descriptor
  0x05,  // Platform Capability descriptor
  0x00,  // Reserved
  0xDF, 0x60, 0xDD, 0xD8, 0x89, 0x45, 0xC7, 0x4C,
  0x9C, 0xD2, 0x65, 0x9D, 0x9E, 0x64, 0x8A, 0x9F,  // MS OS 2.0 GUID
  0x00, 0x00, 0x03, 0x06,  // Windows version
  MS_OS_20_DESCRIPTOR_LENGTH, 0x00,  // Descriptor set length
  0x02,  // Vendor request code
  0x00   // Alternate enumeration code
};

const uint8_t *USBD_F_BOSDescriptor(uint16_t *length) {
  *length = sizeof(USBD_BOS_DESCRIPTOR);
  return USBD_BOS_DESCRIPTOR;
}

static const uint8_t USBD_MS_OS_20_DESCRIPTOR[] = {
  /* Microsoft OS 2.0 descriptor set header (table 10) */
  0x0A, 0x00,  /* Descriptor size (10 bytes) */
  MS_OS_20_SET_HEADER_DESCRIPTOR, 0x00,  /* MS OS 2.0 descriptor set header */
  0x00, 0x00, 0x03, 0x06,  /* Windows version (8.1) (0x06030000) */
  MS_OS_20_DESCRIPTOR_LENGTH, 0x00,  /* Size, MS OS 2.0 descriptor set */

  /* Microsoft OS 2.0 configuration subset header */
  0x08, 0x00,  /* Descriptor size (8 bytes)	*/
  MS_OS_20_SUBSET_HEADER_CONFIGURATION, 0x00,  /* MS OS 2.0 configuration subset header	*/
  0x00,        /* bConfigurationValue	*/
  0x00,        /* Reserved				*/
  0xA8, 0x00,  /* Size, MS OS 2.0 configuration subset	*/

  /* Microsoft OS 2.0 function subset header	*/
  0x08, 0x00,  /* Descriptor size (8 bytes) */
  MS_OS_20_SUBSET_HEADER_FUNCTION, 0x00,  /* MS OS 2.0 function subset header */
  2, // Interface number, always 2 for zadig compatibility
  0x00,        /* Reserved */
  0xA0, 0x00,  /* Size, MS OS 2.0 function subset */

  /* Microsoft OS 2.0 compatible ID descriptor (table 13) */
  0x14, 0x00,  /* wLength	*/
  MS_OS_20_FEATURE_COMPATIBLE_ID, 0x00,  /* MS_OS_20_FEATURE_COMPATIBLE_ID */
  'W',  'I',  'N',  'U',  'S',  'B',  0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  /* Custom property */
  0x84, 0x00,   /* wLength: */
  MS_OS_20_FEATURE_REG_PROPERTY, 0x00,   /* wDescriptorType: MS_OS_20_FEATURE_REG_PROPERTY: 0x04 (Table 9) */
  0x07, 0x00,   /* wPropertyDataType: REG_MULTI_SZ (Table 15) */
  0x2a, 0x00,   /* wPropertyNameLength:  */
  /* bPropertyName: DeviceInterfaceGUID */
  'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00, 't', 0x00, 'e', 0x00,
  'r', 0x00, 'f', 0x00, 'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00, 'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00,
  0x00, 0x00,
  0x50, 0x00,   /* wPropertyDataLength */
  /* bPropertyData: {975F44D9-0D08-43FD-8B3E-127CA8AFFF9D}. */
  '{', 0x00, '9', 0x00, '7', 0x00, '5', 0x00, 'F', 0x00, '4', 0x00, '4', 0x00, 'D', 0x00, '9', 0x00, '-', 0x00,
  '0', 0x00, 'D', 0x00, '0', 0x00, '8', 0x00, '-', 0x00, '4', 0x00, '3', 0x00, 'F', 0x00, 'D', 0x00, '-', 0x00,
  '8', 0x00, 'B', 0x00, '3', 0x00, 'E', 0x00, '-', 0x00, '1', 0x00, '2', 0x00, '7', 0x00, 'C', 0x00, 'A', 0x00,
  '8', 0x00, 'A', 0x00, 'F', 0x00, 'F', 0x00, 'F', 0x00, '9', 0x00, 'D', 0x00, '}', 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t *USBD_WEBUSB_MS_OS_20_DESCRIPTOR(uint16_t *length) {
  *length = sizeof(USBD_MS_OS_20_DESCRIPTOR);
  return (uint8_t *)USBD_MS_OS_20_DESCRIPTOR;
} 

// Default landing page
SetWebUSBLandingPage2(1, "https://webusb.github.io/arduino/demos/console/", __attribute__((weak)));

const uint8_t *USBD_WEBUSB_LandingPageDescriptor(uint16_t *length) {
  *length = WEBUSB_landingpage.length;
  return (uint8_t*)&WEBUSB_landingpage;
} 
#endif
  
}  // extern C

#endif /* USBCON */

