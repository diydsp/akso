/**
  ******************************************************************************
  * @file    USBH_conf.h
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    19-June-2014
  * @brief   General low level driver configuration
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
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
#ifndef __USBH_CONF__H__
#define __USBH_CONF__H__

#define STM32H743xx

//#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include "stm32h7xx.h"
#include "stm32h7xx_hal_hcd.h"
#include "ch.h"
//#include "logging.h"


/* Includes ------------------------------------------------------------------*/

/** @addtogroup USBH_OTG_DRIVER
  * @{
  */

/** @defgroup USBH_CONF
  * @brief usb otg low level driver configuration file
  * @{
  */

/** @defgroup USBH_CONF_Exported_Defines
  * @{
  */

#define USBH_MAX_NUM_ENDPOINTS                2
#define USBH_MAX_NUM_INTERFACES               6
#define USBH_MAX_NUM_CONFIGURATION            1
#define USBH_KEEP_CFG_DESCRIPTOR              1
#define USBH_MAX_NUM_SUPPORTED_CLASS          1
#define USBH_MAX_SIZE_CONFIGURATION           0x200
#define USBH_MAX_DATA_BUFFER                  0x200
#define USBH_DEBUG_LEVEL                      0
#define USBH_USE_OS                           1

/** @defgroup USBH_Exported_Macros
  * @{
  */

 /* Memory management macros */
#define USBH_malloc               fakemalloc
#define USBH_free                 fakefree
#define USBH_memset               memset
#define USBH_memcpy               memcpy

extern void* fakemalloc(size_t size);
extern void fakefree(void * p);

#define osThreadId thread_t *

#define osThreadDef(name, fn, prio, instances, stacksz) \
  static THD_WORKING_AREA(wa##name, 840); \
  osThreadId name = chThdCreateStatic(wa##name, sizeof(wa##name), HIGHPRIO, (tfunc_t)fn, phost); \
  phost->os_event = name; \
  chRegSetThreadName("usbhost");
#define osThreadCreate(x,y) x
#define osThread(x) x

#if 0
#define osMessageQId InputQueue *
//#define osMessagePut(q,val,time) chSysLockFromIsr(); chIQPutI (q,val); chSysUnlockFromIsr();
#define osMessagePut(q,val,time) chIQPutI (q,val);
#define osMessageGet(q,to) \
   (osEvent)chIQGetTimeout(q, TIME_INFINITE)

#else
#define osMessageQId thread_t *
#define osMessagePutI(q,val,time) chEvtSignalI (q,1<<val);
#define osMessagePut(q,val,time) chEvtSignal (q,1<<val);
#define osMessageGet(q,to) chEvtWaitOne(0xFF)
#endif

// osThreadId
#define osMessageQDef(name, queue_sz, type) \
  static int buf[queue_sz]; \
  MAILBOX_DECL(name, &buf, queue_sz)
#define osMessageCreate(queue_def, thread_id)  thread_id /*&queue_def*/
#define osMessageQ(x) x
#define osWaitForever TIME_INFINITE
#define osEventMessage 1
typedef int osEvent;

//#define DEBUG_ON_GPIO

 /* DEBUG macros */


#if (USBH_DEBUG_LEVEL > 0)
extern void LogTextMessage(const char* format, ...);
#define  USBH_UsrLog(...)   LogTextMessage(__VA_ARGS__);
#else
#define USBH_UsrLog(...)
#endif


#if (USBH_DEBUG_LEVEL > 1)

#define  USBH_ErrLog(...)   LogTextMessage(__VA_ARGS__);
#else
#define USBH_ErrLog(...)
#endif


#if (USBH_DEBUG_LEVEL > 2)
#define  USBH_DbgLog(...)   LogTextMessage(__VA_ARGS__);
#else
#define USBH_DbgLog(...)
#endif

/**
  * @}
  */

/**
  * @}
  */


/** @defgroup USBH_CONF_Exported_Types
  * @{
  */
/**
  * @}
  */


/** @defgroup USBH_CONF_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_CONF_Exported_Variables
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_CONF_Exported_FunctionsPrototype
  * @{
  */
/**
  * @}
  */


#endif //__USBH_CONF__H__


/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
