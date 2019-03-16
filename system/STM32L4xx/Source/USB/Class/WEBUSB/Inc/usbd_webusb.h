/**
  ******************************************************************************
  * @file    usbd_cdc.h
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief   header file for the usbd_cdc.c file.
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
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_WEBUSB_H
#define __USB_WEBUSB_H

#include  "usbd_cdc.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup usbd_cdc
  * @brief This file is the Header file for usbd_cdc.c
  * @{
  */ 


/** @defgroup usbd_cdc_Exported_Defines
  * @{
  */ 
#define WEBUSB_IN_EP                                0x85  /* EP5 for data IN */
#define WEBUSB_OUT_EP                               0x05  /* EP5 for data OUT */


/**
  * @}
  */ 


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */

/**
  * @}
  */ 


/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */ 
  
/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
uint8_t  USBD_WEBUSB_RegisterInterface  (USBD_HandleTypeDef   *pdev, 
                                         const USBD_CDC_ItfTypeDef *fops);

uint8_t  USBD_WEBUSB_SetTxBuffer        (USBD_HandleTypeDef   *pdev,
                                      const uint8_t *pbuff,
                                      uint16_t length);

uint8_t  USBD_WEBUSB_SetRxBuffer        (USBD_HandleTypeDef   *pdev,
                                      uint8_t  *pbuff);
  
uint8_t  USBD_WEBUSB_ReceivePacket      (USBD_HandleTypeDef *pdev);

uint8_t  USBD_WEBUSB_TransmitPacket     (USBD_HandleTypeDef *pdev);
/**
  * @}
  */ 

uint8_t  USBD_WEBUSB_Init (USBD_HandleTypeDef *pdev, 
			uint8_t cfgidx);

uint8_t  USBD_WEBUSB_DeInit (USBD_HandleTypeDef *pdev, 
			  uint8_t cfgidx);

uint8_t  USBD_WEBUSB_Setup (USBD_HandleTypeDef *pdev, 
			 USBD_SetupReqTypedef *req);

uint8_t  USBD_WEBUSB_DataIn (USBD_HandleTypeDef *pdev, 
			  uint8_t epnum);

uint8_t  USBD_WEBUSB_DataOut (USBD_HandleTypeDef *pdev, 
			   uint8_t epnum);


#ifdef __cplusplus
}
#endif

#endif  /* __USB_WEBUSB_H */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
