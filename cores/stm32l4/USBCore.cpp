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
#if 0 // #ifdef USB_CLASS_WEBUSB  
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


}  // extern C

#endif /* USBCON */

