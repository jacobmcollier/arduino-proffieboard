/**
  ******************************************************************************
  * @file    usbd_cdc_msc.c
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief
  *           
  *  @verbatim
  *      
  *          ===================================================================      
  *                                Composite CDC_MSC
  *          =================================================================== 
  *      
  *  @endverbatim
  *                                  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_msc.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_conf.h"
#include "usbd_ctlreq.h"
#include "usbd_webusb.h"

#include "stm32l4_gpio.h"

extern USBD_CDC_ItfTypeDef const stm32l4_usbd_cdc_interface;
extern USBD_CDC_ItfTypeDef const stm32l4_usbd_webusb_interface;
extern USBD_StorageTypeDef const dosfs_storage_interface;
extern USBD_HID_ItfTypeDef const stm32l4_usbd_dap_interface;
extern USBD_HID_ItfTypeDef const stm32l4_usbd_hid_interface;
extern void USBD_Configure(void);

extern const char *USBD_SuffixString;

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_CDC_MSC 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_CDC_MSC_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_CDC_MSC_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_CDC_MSC_Private_Macros
  * @{
  */

#define ct_assert(e) extern char (*ct_assert(void)) [sizeof(char[1 - 2*!(e)])]

/**
  * @}
  */ 


/** @defgroup USBD_CDC_MSC_Private_FunctionPrototypes
  * @{
  */


static uint8_t  USBD_CDC_MSC_Init (USBD_HandleTypeDef *pdev, 
				   uint8_t cfgidx);

static uint8_t  USBD_CDC_MSC_DeInit (USBD_HandleTypeDef *pdev, 
				     uint8_t cfgidx);

static uint8_t  USBD_CDC_MSC_Setup (USBD_HandleTypeDef *pdev, 
				    USBD_SetupReqTypedef *req);

static uint8_t  USBD_CDC_MSC_DataIn (USBD_HandleTypeDef *pdev, 
				     uint8_t epnum);

static uint8_t  USBD_CDC_MSC_DataOut (USBD_HandleTypeDef *pdev, 
				      uint8_t epnum);

static uint8_t  USBD_CDC_MSC_EP0_RxReady (USBD_HandleTypeDef *pdev);

static const uint8_t  *USBD_CDC_MSC_GetFSCfgDesc (uint16_t *length);

static const uint8_t  *USBD_CDC_MSC_GetHSCfgDesc (uint16_t *length);

static const uint8_t  *USBD_CDC_MSC_GetOtherSpeedCfgDesc (uint16_t *length);

static const uint8_t  *USBD_CDC_MSC_GetDeviceQualifierDescriptor (uint16_t *length);


/* USB Standard Device Descriptor */
static const uint8_t USBD_CDC_MSC_DeviceQualifierDesc[] =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

ct_assert(sizeof(USBD_CDC_MSC_DeviceQualifierDesc) == USB_LEN_DEV_QUALIFIER_DESC);

/**
  * @}
  */ 

/** @defgroup USBD_CDC_MSC_Private_Variables
  * @{
  */ 


/* CDC_MSC interface class callbacks structure */
static const USBD_ClassTypeDef  USBD_CDC_MSC_CLASS = 
{
  USBD_CDC_MSC_Init,
  USBD_CDC_MSC_DeInit,
  USBD_CDC_MSC_Setup,
  NULL,                 /* EP0_TxSent, */
  USBD_CDC_MSC_EP0_RxReady,
  USBD_CDC_MSC_DataIn,
  USBD_CDC_MSC_DataOut,
  NULL,
  NULL,
  NULL,     
  USBD_CDC_MSC_GetHSCfgDesc,  
  USBD_CDC_MSC_GetFSCfgDesc,    
  USBD_CDC_MSC_GetOtherSpeedCfgDesc, 
  USBD_CDC_MSC_GetDeviceQualifierDescriptor,
#if (USBD_SUPPORT_USER_STRING == 1)
  USBD_F_GetUsrStrDescriptor,
#endif  
};

/* WEBUSB interface class callbacks structure */
static const USBD_ClassTypeDef  USBD_WEBUSB_CLASS_Interface = 
{
  USBD_WEBUSB_Init,
  USBD_WEBUSB_DeInit,
  USBD_WEBUSB_Setup,
  NULL,                 /* EP0_TxSent, */
  NULL,                 /* EP0_RxReady */
  USBD_WEBUSB_DataIn,
  USBD_WEBUSB_DataOut,
  NULL,
  NULL,
  NULL,     
  NULL,
  NULL,
  NULL,
  NULL,
#if (USBD_SUPPORT_USER_STRING == 1)
  NULL,
#endif  
};

static const USBD_ClassTypeDef  USBD_MSC_CLASS_Interface = 
{
  USBD_MSC_Init,
  USBD_MSC_DeInit,
  USBD_MSC_Setup,
  NULL,                 /* EP0_TxSent, */
  NULL,                 /* EP0_RxReady */
  USBD_MSC_DataIn,
  USBD_MSC_DataOut,
  NULL,
  NULL,
  NULL,     
  NULL,
  NULL,
  NULL,
  NULL,
#if (USBD_SUPPORT_USER_STRING == 1)
  NULL,
#endif  
};

static const USBD_ClassTypeDef  USBD_HID_CLASS_Interface = 
{
  USBD_HID_Init,
  USBD_HID_DeInit,
  USBD_HID_Setup,
  NULL,                 /* EP0_TxSent, */
  USBD_HID_EP0_RxReady, /* EP0_RxReady */
  USBD_HID_DataIn,
  USBD_HID_DataOut,
  NULL,
  NULL,
  NULL,     
  NULL,
  NULL,
  NULL,
  NULL,
#if (USBD_SUPPORT_USER_STRING == 1)
  NULL,
#endif  
};

static const USBD_ClassTypeDef * USBD_MSC_Class_Interface = NULL;
static const USBD_ClassTypeDef * USBD_HID_Class_Interface = NULL;
static const USBD_ClassTypeDef * USBD_WEBUSB_Class_Interface = NULL;

static uint16_t USBD_MSC_Interface = 0xffff;
static uint16_t USBD_HID_Interface = 0xffff;
static uint16_t USBD_WEBUSB_Interface = 0xffff;

#define USB_CDC_CONFIG_DESC_SIZ   (9+8+(9+5+5+4+5+7)+(9+7+7))
#define USB_MSC_CONFIG_DESC_SIZ   (9+7+7)
#define USB_HID_CONFIG_DESC_SIZ   (9+9+7+7)
#define USB_WEBUSB_CONFIG_DESC_SIZ   (9+7+7)
#define USB_DUMMY_CONFIG_DESC_SIZ   (9)

#define USB_CDC_INTERFACE_CONTROL 0
#define USB_CDC_INTERFACE_DATA    1

#define USB_CDC_INTERFACE_COUNT   2
#define USB_MSC_INTERFACE_COUNT   1
#define USB_HID_INTERFACE_COUNT   1
#define USB_WEBUSB_INTERFACE_COUNT   1
#define USB_DUMMY_INTERFACE_COUNT   1

#define USB_CDC_MSC_CONFIG_DESC_SIZ      (USB_CDC_CONFIG_DESC_SIZ + USB_DUMMY_CONFIG_DESC_SIZ + USB_MSC_CONFIG_DESC_SIZ)
#define USB_CDC_HID_CONFIG_DESC_SIZ      (USB_CDC_CONFIG_DESC_SIZ + USB_DUMMY_CONFIG_DESC_SIZ + USB_HID_CONFIG_DESC_SIZ)
#define USB_CDC_MSC_HID_CONFIG_DESC_SIZ  (USB_CDC_CONFIG_DESC_SIZ + USB_DUMMY_CONFIG_DESC_SIZ + USB_MSC_CONFIG_DESC_SIZ + USB_HID_CONFIG_DESC_SIZ)
#define USB_CDC_WEBUSB_CONFIG_DESC_SIZ   (USB_CDC_CONFIG_DESC_SIZ + USB_WEBUSB_CONFIG_DESC_SIZ)
#define USB_CDC_MSC_WEBUSB_CONFIG_DESC_SIZ  (USB_CDC_CONFIG_DESC_SIZ + USB_WEBUSB_CONFIG_DESC_SIZ + USB_MSC_CONFIG_DESC_SIZ )

#define USB_CDC_MSC_INTERFACE_COUNT      (USB_CDC_INTERFACE_COUNT + USB_DUMMY_CONFIG_INTERFACE_COUNT + USB_MSC_INTERFACE_COUNT)
#define USB_CDC_HID_INTERFACE_COUNT      (USB_CDC_INTERFACE_COUNT + USB_DUMMY_CONFIG_INTERFACE_COUNT + USB_HID_INTERFACE_COUNT)
#define USB_CDC_MSC_HID_INTERFACE_COUNT  (USB_CDC_INTERFACE_COUNT + USB_DUMMY_CONFIG_INTERFACE_COUNT + USB_MSC_INTERFACE_COUNT + USB_HID_INTERFACE_COUNT)
#define USB_CDC_WEBUSB_INTERFACE_COUNT   (USB_CDC_INTERFACE_COUNT + USB_WEBUSB_INTERFACE_COUNT)
#define USB_CDC_MSC_WEBUSB_INTERFACE_COUNT  (USB_CDC_INTERFACE_COUNT + USB_MSC_INTERFACE_COUNT + USB_WEBUSB_INTERFACE_COUNT)


#define USB_WORD(X) LOBYTE(X), HIBYTE(X)
#define USB_ENDPOINT(EP, ATTR, MAX_PACKET_SIZE, INTERVAL)		\
  0x07,                                                        /* bLength */ \
  0x05,                                                        /* bDescriptorType */ \
  EP,                                                          /* bEndpointAddress */ \
  ATTR,                                                        /* bmAttributes */ \
  USB_WORD(MAX_PACKET_SIZE),                                   /* wMaxPacketSize */ \
  INTERVAL                                                     /* bInterval */

#define USB_INTERFACE(INTERFACE_NUMBER, NUM_ENDPOINTS, INTERFACE_CLASS, INTERFACE_SUBCLASS, INTERFACE_PROTOCOL, iINTERFACE) \
  0x09,                                                        /* bLength */ \
  0x04,                                                        /* bDescriptorType */ \
  INTERFACE_NUMBER,                                            /* bInterfaceNumber */ \
  0x00,                                                        /* bAlternateSetting */ \
  NUM_ENDPOINTS,                                               /* bNumEndpoints */ \
  INTERFACE_CLASS,                                             /* bInterfaceClass */ \
  INTERFACE_SUBCLASS,                                          /* bInterfaceSubClass */	\
  INTERFACE_PROTOCOL,                                          /* bInterfaceProtocol */	\
  iINTERFACE                                                   /* iInterface */


#define CDC_CONTROL_INTERFACE_DATA(INTERFACE_NUM)			\
  /**** CDC Control Interface ****/					\
  USB_INTERFACE(INTERFACE_NUM, 0x01,  0x02,0x02,0x01, 0x05),		\
									\
  /**** CDC Header ****/						\
  0x05,                                                        /* bLength */ \
  0x24,                                                        /* bDescriptorType */ \
  0x00,                                                        /* bDescriptorSubtype */	\
  0x10,                                                        /* bcdCDC */ \
  0x01,									\
									\
  /**** CDC Call Management ****/					\
  0x05,                                                        /* bFunctionLength */ \
  0x24,                                                        /* bDescriptorType */ \
  0x01,                                                        /* bDescriptorSubtype */	\
  0x00,                                                        /* bmCapabilities */ \
  0x01,                                                        /* bDataInterface */ \
									\
  /**** CDC ACM ****/							\
  0x04,                                                        /* bFunctionLength */ \
  0x24,                                                        /* bDescriptorType */ \
  0x02,                                                        /* bDescriptorSubtype */	\
  0x02,                                                        /* bmCapabilities */ \
									\
  /**** CDC Union ****/							\
  0x05,                                                        /* bFunctionLength */ \
  0x24,                                                        /* bDescriptorType */ \
  0x06,                                                        /* bDescriptorSubtype */	\
  0x00,                                                        /* bMasterInterface */ \
  0x01,                                                        /* bSlaveInterface0 */ \
									\
  /**** CDC Control Endpoint Descriptor ****/				\
  USB_ENDPOINT(CDC_CMD_EP, 0x03, CDC_CMD_PACKET_SIZE, 0xff)


#define CDC_DATA_INTERFACE_DATA(INTERFACE_NUM)				\
  /**** CDC Data Interface ****/					\
  USB_INTERFACE(INTERFACE_NUM, 0x02,  0x0a,0x00,0x00,  0x06),		\
									\
  /**** CDC Data Endpoint IN ****/					\
  USB_ENDPOINT(CDC_IN_EP, 0x02, CDC_DATA_FS_MAX_PACKET_SIZE, 0x00),	\
									\
  /**** CDC Data Endpoint OUT ****/					\
  USB_ENDPOINT(CDC_OUT_EP, 0x02, CDC_DATA_FS_MAX_PACKET_SIZE, 0x00)

#define MSC_INTERFACE_DATA(INTERFACE_NUM)				\
  /**** MSC Interface ****/					\
  USB_INTERFACE(INTERFACE_NUM, 0x02, 0x08,0x06,0x50, 0x07),	\
								\
  /**** MSC Endpoint IN ****/					\
  USB_ENDPOINT(MSC_EPIN_ADDR, 0x02, MSC_MAX_FS_PACKET, 0x00),	\
								\
  /**** MSC Endpoint OUT ****/					\
  USB_ENDPOINT(MSC_EPOUT_ADDR, 0x02, MSC_MAX_FS_PACKET, 0x00)

#define HID_INTERFACE_DATA(INTERFACE_NUM)				\
  /**** HID Interface ****/						\
  USB_INTERFACE(0x02, 0x02, 0x03,0x00,0x00, 0x09),			\
									\
  /**** HID Descriptor ****/						\
  0x09,                                                        /* bLength */ \
  0x21,                                                        /* bDescriptorType */ \
  0x11,                                                        /* bcdHID */ \
  0x01,									\
  0x00,                                                        /* bCountryCode */ \
  0x01,                                                        /* bNumDescriptors */ \
  0x22,                                                        /* bDescriptorType */ \
  USB_WORD(USB_HID_REPORT_DESC_SIZ),                             /* wItemLength */ \
									\
  /**** HID Endpoint IN ****/						\
  USB_ENDPOINT(HID_EPIN_ADDR, 0x03, HID_EPIN_SIZE, 0x01),		\
									\
  /**** HID Endpoint OUT ****/						\
    USB_ENDPOINT(HID_EPOUT_ADDR, 0x03, HID_EPOUT_SIZE, 0x01)

#define WEBUSB_INTERFACE_DATA(INTERFACE_NUM)				\
  /**** CDC Data Interface ****/					\
  USB_INTERFACE(INTERFACE_NUM, 0x02,  0xff,0x00,0x00,  0x06),		\
									\
  /**** CDC Data Endpoint IN ****/					\
  USB_ENDPOINT(WEBUSB_IN_EP, 0x02, CDC_DATA_FS_MAX_PACKET_SIZE, 0x00),	\
									\
  /**** CDC Data Endpoint OUT ****/					\
  USB_ENDPOINT(WEBUSB_OUT_EP, 0x02, CDC_DATA_FS_MAX_PACKET_SIZE, 0x00)


/* This dummy interface is used to make sure that interface 2 is
 * always webusb a dummy or non-existant. This is helpful on windows
 * as we can use zadig to assign the winusb driver to interface 2
 * without accidentially overriding the device driver for other things.
 */
#define DUMMY_INTERFACE_DATA(INTERFACE_NUM)				\
  /**** CDC Data Interface ****/					\
  USB_INTERFACE(INTERFACE_NUM, 0x00,  0xff,0x00,0x00,  0x06)


#define CDC_INTERFACES_DATA(INTERFACE_NUM)				\
  /**** IAD to associate the two CDC interfaces ****/			\
  0x08,                                                        /* bLength */ \
  0x0b,                                                        /* bDescriptorType */ \
  0x00,                                                        /* bFirstInterface */ \
  0x02,                                                        /* bInterfaceCount */ \
  0x02,                                                        /* bFunctionClass */ \
  0x02,                                                        /* bFunctionSubClass */ \
  0x01,                                                        /* bFunctionProtocol */ \
  0x04,                                                        /* iFunction */ \
									\
  CDC_CONTROL_INTERFACE_DATA(INTERFACE_NUM),				\
  CDC_DATA_INTERFACE_DATA(INTERFACE_NUM + 1)

#define CONFIG_DESCRIPTOR_DATA(SIZE, INTERFACES)			\
  /**** Configuration Descriptor ****/					\
  0x09,                                                        /* bLength */ \
  0x02,                                                        /* bDescriptorType */ \
  USB_WORD(SIZE),                                              /* wTotalLength */ \
  INTERFACES,                                                  /* bNumInterfaces */ \
  0x01,                                                        /* bConfigurationValue */ \
  0x00,                                                        /* iConfiguration */ \
  0x80,                                                        /* bmAttributes */ \
  0x32                                                         /* bMaxPower */


static const uint8_t USBD_CDC_MSC_ConigurationDescriptor_1[] =
{
  CONFIG_DESCRIPTOR_DATA(USB_CDC_CONFIG_DESC_SIZ, 2),
  CDC_INTERFACES_DATA(0),
};

ct_assert(sizeof(USBD_CDC_MSC_ConigurationDescriptor_1) == USB_CDC_CONFIG_DESC_SIZ);

static const uint8_t USBD_CDC_MSC_ConigurationDescriptor_2[] =
{
  CONFIG_DESCRIPTOR_DATA(USB_CDC_MSC_CONFIG_DESC_SIZ, 4),
  CDC_INTERFACES_DATA(0),
  DUMMY_INTERFACE_DATA(2),
  MSC_INTERFACE_DATA(3),
};

ct_assert(sizeof(USBD_CDC_MSC_ConigurationDescriptor_2) == USB_CDC_MSC_CONFIG_DESC_SIZ);


#define USB_HID_REPORT_DESC_SIZ 101

static const uint8_t USBD_HID_ReportDescriptor[USB_HID_REPORT_DESC_SIZ] = 
{
  //	Mouse
  0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)	// 54
  0x09, 0x02,                    // USAGE (Mouse)
  0xa1, 0x01,                    // COLLECTION (Application)
  0x09, 0x01,                    //   USAGE (Pointer)
  0xa1, 0x00,                    //   COLLECTION (Physical)
  0x85, 0x01,                    //     REPORT_ID (1)
  0x05, 0x09,                    //     USAGE_PAGE (Button)
  0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
  0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
  0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
  0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
  0x95, 0x03,                    //     REPORT_COUNT (3)
  0x75, 0x01,                    //     REPORT_SIZE (1)
  0x81, 0x02,                    //     INPUT (Data,Var,Abs)
  0x95, 0x01,                    //     REPORT_COUNT (1)
  0x75, 0x05,                    //     REPORT_SIZE (5)
  0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
  0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
  0x09, 0x30,                    //     USAGE (X)
  0x09, 0x31,                    //     USAGE (Y)
  0x09, 0x38,                    //     USAGE (Wheel)
  0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
  0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
  0x75, 0x08,                    //     REPORT_SIZE (8)
  0x95, 0x03,                    //     REPORT_COUNT (3)
  0x81, 0x06,                    //     INPUT (Data,Var,Rel)
  0xc0,                          //   END_COLLECTION
  0xc0,                          // END_COLLECTION

  //	Keyboard
  0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)	// 47
  0x09, 0x06,                    // USAGE (Keyboard)
  0xa1, 0x01,                    // COLLECTION (Application)
  0x85, 0x02,                    //   REPORT_ID (2)
  0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
  0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
  0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
  0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
  0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
  0x75, 0x01,                    //   REPORT_SIZE (1)
  0x95, 0x08,                    //   REPORT_COUNT (8)
  0x81, 0x02,                    //   INPUT (Data,Var,Abs)
  0x95, 0x01,                    //   REPORT_COUNT (1)
  0x75, 0x08,                    //   REPORT_SIZE (8)
  0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
  0x95, 0x06,                    //   REPORT_COUNT (6)
  0x75, 0x08,                    //   REPORT_SIZE (8)
  0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
  0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
  0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
  0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
  0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
  0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
  0xc0,                          // END_COLLECTION
};

ct_assert(sizeof(USBD_HID_ReportDescriptor) == USB_HID_REPORT_DESC_SIZ);

static const uint8_t USBD_CDC_MSC_ConigurationDescriptor_3[] =
{
  CONFIG_DESCRIPTOR_DATA(USB_CDC_HID_CONFIG_DESC_SIZ, 4),
  CDC_INTERFACES_DATA(0),
  DUMMY_INTERFACE_DATA(2),
  HID_INTERFACE_DATA(3),
};

ct_assert(sizeof(USBD_CDC_MSC_ConigurationDescriptor_3) == USB_CDC_HID_CONFIG_DESC_SIZ);

static const uint8_t USBD_CDC_MSC_ConigurationDescriptor_4[] =
{
  CONFIG_DESCRIPTOR_DATA(USB_CDC_MSC_HID_CONFIG_DESC_SIZ, 4),
  CDC_INTERFACES_DATA(0),
  DUMMY_INTERFACE_DATA(2),
  MSC_INTERFACE_DATA(3),
  HID_INTERFACE_DATA(4),
};

ct_assert(sizeof(USBD_CDC_MSC_ConigurationDescriptor_4) == USB_CDC_MSC_HID_CONFIG_DESC_SIZ);

#define USB_DAP_REPORT_DESC_SIZ 28

static const uint8_t USBD_DAP_ReportDescriptor[] = 
{
  0x06, 0xc0, 0xff,                                            /* Usage Page */
  0x0a, 0x00, 0x0c,                                            /* Usage */
  0xa1, 0x01,                                                  /* Collection (Application) */
  0x75, 0x08,                                                  /* Report Size (8) */
  0x15, 0x00,                                                  /* Logical Minimum (0) */
  0x26, 0xff, 0x00,                                            /* Logical Maximum (255) */
  0x95, 0x40,                                                  /* Report Count (64) */
  0x09, 0x01,                                                  /* Usage (Vendor-Defined 1) */
  0x81, 0x02,                                                  /* Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit) */
  0x95, 0x40,                                                  /* Report Count (64) */
  0x09, 0x02,                                                  /* Usage (Vendor-Defined 2) */
  0x91, 0x02,                                                  /* Output (Data,Var,Abs,NWrp,Lin,Pref,NNul,NVol,Bit) */
  0xc0,                                                        /* End Collection */
};

ct_assert(sizeof(USBD_DAP_ReportDescriptor) == USB_DAP_REPORT_DESC_SIZ);

static const uint8_t USBD_CDC_MSC_ConigurationDescriptor_5[] =
{
  CONFIG_DESCRIPTOR_DATA(USB_CDC_HID_CONFIG_DESC_SIZ, 4),
  CDC_INTERFACES_DATA(0),
  DUMMY_INTERFACE_DATA(2),
  HID_INTERFACE_DATA(3),
};

ct_assert(sizeof(USBD_CDC_MSC_ConigurationDescriptor_5) == USB_CDC_HID_CONFIG_DESC_SIZ);

static const uint8_t USBD_CDC_MSC_ConigurationDescriptor_6[] =
{
  CONFIG_DESCRIPTOR_DATA(USB_CDC_MSC_HID_CONFIG_DESC_SIZ, 5),
  CDC_INTERFACES_DATA(0),
  DUMMY_INTERFACE_DATA(2),
  MSC_INTERFACE_DATA(3),
  HID_INTERFACE_DATA(4),
};

ct_assert(sizeof(USBD_CDC_MSC_ConigurationDescriptor_6) == USB_CDC_MSC_HID_CONFIG_DESC_SIZ);

static const uint8_t USBD_CDC_WEBUSB_ConigurationDescriptor_7[] =
{
  CONFIG_DESCRIPTOR_DATA(USB_CDC_WEBUSB_CONFIG_DESC_SIZ, 3),
  CDC_INTERFACES_DATA(0),
  WEBUSB_INTERFACE_DATA(2),
};

ct_assert(sizeof(USBD_CDC_WEBUSB_ConigurationDescriptor_7) == USB_CDC_WEBUSB_CONFIG_DESC_SIZ);

static const uint8_t USBD_CDC_MSC_WEBUSB_ConigurationDescriptor_8[] =
{
  CONFIG_DESCRIPTOR_DATA(USB_CDC_MSC_WEBUSB_CONFIG_DESC_SIZ, 4),
  CDC_INTERFACES_DATA(0),
  WEBUSB_INTERFACE_DATA(2),
  MSC_INTERFACE_DATA(3),
};

ct_assert(sizeof(USBD_CDC_MSC_WEBUSB_ConigurationDescriptor_8) == USB_CDC_MSC_WEBUSB_CONFIG_DESC_SIZ);


static const uint8_t * USBD_CDC_MSC_ConigurationDescriptorData = NULL;
static uint16_t USBD_CDC_MSC_ConigurationDescriptorLength = 0;

/**
  * @}
  */ 

/** @defgroup USBD_CDC_MSC_Private_Functions
  * @{
  */ 

/**
  * @brief  USBD_CDC_MSC_Init
  *         Initialize the CDC & MSC interfaces
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_CDC_MSC_Init (USBD_HandleTypeDef *pdev, 
				  uint8_t cfgidx)
{
  USBD_Configure();

  USBD_CDC_Init(pdev, cfgidx);

  if (USBD_MSC_Class_Interface) (*USBD_MSC_Class_Interface->Init)(pdev, cfgidx);
  if (USBD_HID_Class_Interface) (*USBD_HID_Class_Interface->Init)(pdev, cfgidx);
  if (USBD_WEBUSB_Class_Interface) (*USBD_WEBUSB_Class_Interface->Init)(pdev, cfgidx);
    
  return USBD_OK;
}

/**
  * @brief  USBD_CDC_MSC_DeInit
  *         DeInitialize the CDC & MSC layers
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_CDC_MSC_DeInit (USBD_HandleTypeDef *pdev, 
				     uint8_t cfgidx)
{
  if (USBD_HID_Class_Interface) (*USBD_HID_Class_Interface->DeInit)(pdev, cfgidx);
  if (USBD_MSC_Class_Interface) (*USBD_MSC_Class_Interface->DeInit)(pdev, cfgidx);
  if (USBD_WEBUSB_Class_Interface) (*USBD_WEBUSB_Class_Interface->DeInit)(pdev, cfgidx);

  USBD_CDC_DeInit(pdev, cfgidx);
    
  return USBD_OK;
}

/**
  * @brief  USBD_CDC_MSC_Setup
  *         Handle the CDC & MSC specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_CDC_MSC_Setup (USBD_HandleTypeDef *pdev, 
				    USBD_SetupReqTypedef *req)
{

  switch (req->bmRequest & USB_REQ_RECIPIENT_MASK)
    {
    case USB_REQ_RECIPIENT_INTERFACE:
      if (req->wIndex == USBD_HID_Interface)
	{
	  if (USBD_HID_Class_Interface) {
	    return (*USBD_HID_Class_Interface->Setup)(pdev, req);
	  }
	}
      else if (req->wIndex == USBD_MSC_Interface)
	{
	  if (USBD_MSC_Class_Interface) {
	    return (*USBD_MSC_Class_Interface->Setup)(pdev, req);
	  }
	}
      else if (req->wIndex == USBD_WEBUSB_Interface)
	{
	  if (USBD_WEBUSB_Class_Interface) {
	    return (*USBD_WEBUSB_Class_Interface->Setup)(pdev, req);
	  }
	}
      else
	{
	  return (USBD_CDC_Setup(pdev, req));
	}
    
    case USB_REQ_RECIPIENT_ENDPOINT:
      if ((req->wIndex == HID_EPIN_ADDR) || (req->wIndex == HID_EPOUT_ADDR))
	{
	  if (USBD_HID_Class_Interface) {
	    return (*USBD_HID_Class_Interface->Setup)(pdev, req);
	  }
	}
      else if ((req->wIndex == MSC_EPIN_ADDR) || (req->wIndex == MSC_EPOUT_ADDR))
	{
	  if (USBD_MSC_Class_Interface) {
	    return (*USBD_MSC_Class_Interface->Setup)(pdev, req);
	  }
	}
      else if ((req->wIndex == WEBUSB_IN_EP) || (req->wIndex == WEBUSB_OUT_EP))
	{
	  if (USBD_WEBUSB_Class_Interface) {
	    return (*USBD_WEBUSB_Class_Interface->Setup)(pdev, req);
	  }
	}
      else
	{
	  return (USBD_CDC_Setup(pdev, req));
	}
    }   
  return USBD_OK;
}

/**
  * @brief  USBD_CDC_MSC_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t  USBD_CDC_MSC_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  if (epnum == (HID_EPIN_ADDR&~0x80))
    {
      return (*USBD_HID_Class_Interface->DataIn)(pdev, epnum);
    }
  else if (epnum == (MSC_EPIN_ADDR&~0x80))
    {
      return (*USBD_MSC_Class_Interface->DataIn)(pdev, epnum);
    }
  else if (epnum == (WEBUSB_IN_EP&~0x80))
    {
      return (*USBD_WEBUSB_Class_Interface->DataIn)(pdev, epnum);
    }
  else
    {
      return (USBD_CDC_DataIn(pdev, epnum));
    }
}

/**
  * @brief  USBD_CDC_MSC_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t  USBD_CDC_MSC_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum)
{      
  if (epnum == (HID_EPOUT_ADDR&~0x80))
    {
      return (*USBD_HID_Class_Interface->DataOut)(pdev, epnum);
    }
  else if (epnum == (MSC_EPOUT_ADDR&~0x80))
    {
      return (*USBD_MSC_Class_Interface->DataOut)(pdev, epnum);
    }
  else if (epnum == (WEBUSB_OUT_EP&~0x80))
    {
      return (*USBD_WEBUSB_Class_Interface->DataOut)(pdev, epnum);
    }
  else
    {
      return (USBD_CDC_DataOut(pdev, epnum));
    }
}



/**
  * @brief  USBD_CDC_MSC_EP0_RxReady
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t  USBD_CDC_MSC_EP0_RxReady (USBD_HandleTypeDef *pdev)
{ 
  if (USBD_HID_Class_Interface) {
      (*USBD_HID_Class_Interface->EP0_RxReady)(pdev);
  }

  USBD_CDC_EP0_RxReady(pdev);

  return USBD_OK;
}

/**
  * @brief  USBD_CDC_MSC_GetFSCfgDesc 
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static const uint8_t  *USBD_CDC_MSC_GetFSCfgDesc (uint16_t *length)
{
  *length = USBD_CDC_MSC_ConigurationDescriptorLength;
  return USBD_CDC_MSC_ConigurationDescriptorData;
}

/**
  * @brief  USBD_CDC_MSC_GetHSCfgDesc 
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static const uint8_t  *USBD_CDC_MSC_GetHSCfgDesc (uint16_t *length)
{
  *length = 0;
  return NULL;
}

/**
  * @brief  USBD_CDC_MSC_GetCfgDesc 
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static const uint8_t  *USBD_CDC_MSC_GetOtherSpeedCfgDesc (uint16_t *length)
{
  *length = 0;
  return NULL;
}

/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
static const uint8_t  *USBD_CDC_MSC_GetDeviceQualifierDescriptor (uint16_t *length)
{
  *length = sizeof(USBD_CDC_MSC_DeviceQualifierDesc);
  return USBD_CDC_MSC_DeviceQualifierDesc;
}

void USBD_CDC_Initialize(USBD_HandleTypeDef *pdev)
{
  USBD_CDC_MSC_ConigurationDescriptorLength = sizeof(USBD_CDC_MSC_ConigurationDescriptor_1);
  USBD_CDC_MSC_ConigurationDescriptorData = USBD_CDC_MSC_ConigurationDescriptor_1;

  USBD_MSC_Class_Interface = NULL;
  USBD_HID_Class_Interface = NULL;

  USBD_MSC_Interface = 0xffff;
  USBD_HID_Interface = 0xffff;

  USBD_RegisterClass(pdev, &USBD_CDC_MSC_CLASS);
  USBD_CDC_RegisterInterface(pdev, &stm32l4_usbd_cdc_interface);
}

void USBD_CDC_MSC_Initialize(USBD_HandleTypeDef *pdev)
{
  USBD_CDC_MSC_ConigurationDescriptorLength = sizeof(USBD_CDC_MSC_ConigurationDescriptor_2);
  USBD_CDC_MSC_ConigurationDescriptorData = USBD_CDC_MSC_ConigurationDescriptor_2;

  USBD_MSC_Class_Interface = &USBD_MSC_CLASS_Interface;
  USBD_HID_Class_Interface = NULL;

  USBD_MSC_Interface = 3;
  USBD_HID_Interface = 0xffff;

  USBD_RegisterClass(pdev, &USBD_CDC_MSC_CLASS);
  USBD_CDC_RegisterInterface(pdev, &stm32l4_usbd_cdc_interface);
  USBD_MSC_RegisterStorage(pdev, &dosfs_storage_interface);
}

void USBD_CDC_HID_Initialize(USBD_HandleTypeDef *pdev)
{
  USBD_CDC_MSC_ConigurationDescriptorLength = sizeof(USBD_CDC_MSC_ConigurationDescriptor_3);
  USBD_CDC_MSC_ConigurationDescriptorData = USBD_CDC_MSC_ConigurationDescriptor_3;

  USBD_MSC_Class_Interface = NULL;
  USBD_HID_Class_Interface = &USBD_HID_CLASS_Interface;

  USBD_MSC_Interface = 0xffff;
  USBD_HID_Interface = 3;

  USBD_RegisterClass(pdev, &USBD_CDC_MSC_CLASS);
  USBD_CDC_RegisterInterface(pdev, &stm32l4_usbd_cdc_interface);
  USBD_HID_RegisterInterface(pdev, &stm32l4_usbd_hid_interface, USBD_HID_ReportDescriptor, sizeof(USBD_HID_ReportDescriptor));
}

void USBD_CDC_MSC_HID_Initialize(USBD_HandleTypeDef *pdev)
{
  USBD_CDC_MSC_ConigurationDescriptorLength = sizeof(USBD_CDC_MSC_ConigurationDescriptor_4);
  USBD_CDC_MSC_ConigurationDescriptorData = USBD_CDC_MSC_ConigurationDescriptor_4;

  USBD_MSC_Class_Interface = &USBD_MSC_CLASS_Interface;
  USBD_HID_Class_Interface = &USBD_HID_CLASS_Interface;

  USBD_MSC_Interface = 3;
  USBD_HID_Interface = 4;

  USBD_RegisterClass(pdev, &USBD_CDC_MSC_CLASS);
  USBD_CDC_RegisterInterface(pdev, &stm32l4_usbd_cdc_interface);
  USBD_MSC_RegisterStorage(pdev, &dosfs_storage_interface);
  USBD_HID_RegisterInterface(pdev, &stm32l4_usbd_hid_interface, USBD_HID_ReportDescriptor, sizeof(USBD_HID_ReportDescriptor));
}

void USBD_CDC_DAP_Initialize(USBD_HandleTypeDef *pdev)
{
  USBD_SuffixString = " CMSIS-DAP";

  USBD_CDC_MSC_ConigurationDescriptorLength = sizeof(USBD_CDC_MSC_ConigurationDescriptor_5);
  USBD_CDC_MSC_ConigurationDescriptorData = USBD_CDC_MSC_ConigurationDescriptor_5;

  USBD_MSC_Class_Interface = NULL;
  USBD_HID_Class_Interface = &USBD_HID_CLASS_Interface;

  USBD_MSC_Interface = 0xffff;
  USBD_HID_Interface = 3;

  USBD_RegisterClass(pdev, &USBD_CDC_MSC_CLASS);
  USBD_CDC_RegisterInterface(pdev, &stm32l4_usbd_cdc_interface);
  USBD_HID_RegisterInterface(pdev, &stm32l4_usbd_dap_interface, USBD_DAP_ReportDescriptor, sizeof(USBD_DAP_ReportDescriptor));
}

void USBD_CDC_MSC_DAP_Initialize(USBD_HandleTypeDef *pdev)
{
  USBD_SuffixString = " CMSIS-DAP";

  USBD_CDC_MSC_ConigurationDescriptorLength = sizeof(USBD_CDC_MSC_ConigurationDescriptor_6);
  USBD_CDC_MSC_ConigurationDescriptorData = USBD_CDC_MSC_ConigurationDescriptor_6;

  USBD_MSC_Class_Interface = &USBD_MSC_CLASS_Interface;
  USBD_HID_Class_Interface = &USBD_HID_CLASS_Interface;

  USBD_MSC_Interface = 3;
  USBD_HID_Interface = 4;

  USBD_RegisterClass(pdev, &USBD_CDC_MSC_CLASS);
  USBD_CDC_RegisterInterface(pdev, &stm32l4_usbd_cdc_interface);
  USBD_MSC_RegisterStorage(pdev, &dosfs_storage_interface);
  USBD_HID_RegisterInterface(pdev, &stm32l4_usbd_dap_interface, USBD_DAP_ReportDescriptor, sizeof(USBD_DAP_ReportDescriptor));
}

void USBD_CDC_WEBUSB_Initialize(USBD_HandleTypeDef *pdev)
{
  USBD_CDC_MSC_ConigurationDescriptorLength = sizeof(USBD_CDC_WEBUSB_ConigurationDescriptor_7);
  USBD_CDC_MSC_ConigurationDescriptorData = USBD_CDC_WEBUSB_ConigurationDescriptor_7;

  USBD_WEBUSB_Class_Interface = &USBD_WEBUSB_CLASS_Interface;
  USBD_WEBUSB_Interface = 2;

  USBD_RegisterClass(pdev, &USBD_CDC_MSC_CLASS);
  USBD_CDC_RegisterInterface(pdev, &stm32l4_usbd_cdc_interface);
  USBD_WEBUSB_RegisterInterface(pdev, &stm32l4_usbd_webusb_interface);
}

void USBD_CDC_MSC_WEBUSB_Initialize(USBD_HandleTypeDef *pdev)
{
  USBD_CDC_MSC_ConigurationDescriptorLength = sizeof(USBD_CDC_MSC_WEBUSB_ConigurationDescriptor_8);
  USBD_CDC_MSC_ConigurationDescriptorData = USBD_CDC_MSC_WEBUSB_ConigurationDescriptor_8;

  USBD_MSC_Class_Interface = &USBD_MSC_CLASS_Interface;
  USBD_WEBUSB_Class_Interface = &USBD_WEBUSB_CLASS_Interface;

  USBD_WEBUSB_Interface = 2;
  USBD_MSC_Interface = 3;

  USBD_RegisterClass(pdev, &USBD_CDC_MSC_CLASS);
  USBD_CDC_RegisterInterface(pdev, &stm32l4_usbd_cdc_interface);
  USBD_MSC_RegisterStorage(pdev, &dosfs_storage_interface);
  USBD_WEBUSB_RegisterInterface(pdev, &stm32l4_usbd_webusb_interface);
}


/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
