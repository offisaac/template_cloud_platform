/**
  *******************************************************************************
  * @file    ：System_DataPool.h
  * @brief   ：This file contains functions of memory allocation operation.
  * @author  ：Mentos Seetoo
  * @date    ：2018,November
  *******************************************************************************
  *
**/
#ifndef _DATA_POOL_H_
#define _DATA_POOL_H_

/* Includes ------------------------------------------------------------------*/
/* Middleware Support */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

/* Driver Support */
#include "Board_SuperRelay.h"
#include "Vision_EngineerAlign.h"
#include "IMU_SCUT.h"
#include "IMU_Yesense.h"
#include "ESC_C620.h"
#include "Remote_DR16.h"
#include "drv_can.h"
#include "drv_uart.h"
#include "drv_timer.h"

/* Macro Definitions ---------------------------------------------------------*/
#define Tiny_Stack_Size       64
#define Small_Stack_Size      128
#define Normal_Stack_Size     256
#define Large_Stack_Size      512
#define Huge_Stack_Size       1024
#define PriorityVeryLow       1
#define PriorityLow           2
#define PriorityBelowNormal   3
#define PriorityNormal        4
#define PriorityAboveNormal   5
#define PriorityHigh          6
#define PrioritySuperHigh     7
#define PriorityRealtime      8
#define SUCCESS   1
#define FAILED    0

/* FreeRTOS Resources --------------------------------------------------------*/
/***** Queue For Communication ******/
extern QueueHandle_t  USART_RxPort;
extern QueueHandle_t  USART_TxPort;
extern QueueHandle_t  CAN1_TxPort;
extern QueueHandle_t  CAN2_TxPort;
/***** Queue For Robot Service*****/
extern QueueHandle_t  DR16_QueueHandle;
extern QueueHandle_t  RMMotor_QueueHandle;
extern QueueHandle_t  SuperRelay_QueueHandle;
extern QueueHandle_t  IMU_QueueHandle;
extern QueueHandle_t  NUC_QueueHandle;
extern QueueHandle_t  Referee_QueueHandle;
/***** Service Resources *****/

/* Memory Pool ----------------------------------------------------------------*/
//暂时被遗忘在这,统一标准接口制定后再集中回收
#ifndef __UUCOBTypeDef_DEFINED
#define __UUCOBTypeDef_DEFINED
typedef struct
{
  uint8_t  port_num;
  int16_t  len;
  void*    address;
}USART_COB;
#endif

#ifndef __CCOBTypeDef_DEFINED
#define __CCOBTypeDef_DEFINED
/* CAN message data type(Communication Object/标准数据帧) */
typedef struct{
  uint16_t  ID;
  uint8_t   DLC;
  uint8_t   Data[8];
}COB_TypeDef;
#endif
/* Function Prototypes -------------------------------------------------------*/ 
void SysDataPool_Init(void);


#endif
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
