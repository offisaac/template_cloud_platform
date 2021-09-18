/**
  ******************************************************************************
  * @file   callback.cpp
  * @brief  callback function running file.
  ******************************************************************************
  * @note
 */

/* Includes ------------------------------------------------------------------*/
#include "internal.h"

/* Private define ------------------------------------------------------------*/

/* function declarations ---------------------------------------------*/



void User_CAN2_RxCpltCallback(CAN_RxBuffer *CAN_RxMessage)
{
  static CAN_COB CAN_RxCOB;
  BaseType_t xHigherPriorityTaskWoken;
  //Send To CAN Receive Queue
  if (CAN2_RxPort != NULL)
  {
    CAN_RxCOB.ID = CAN_RxMessage->header.StdId;
    CAN_RxCOB.DLC = CAN_RxMessage->header.DLC;
    memcpy(&CAN_RxCOB.Data, CAN_RxMessage->data, CAN_RxCOB.DLC);
    xQueueSendFromISR(CAN2_RxPort, &CAN_RxCOB,
        &xHigherPriorityTaskWoken);
  }
}


uint32_t User_UART2_RxCpltCallback(uint8_t *Recv_Data, uint16_t ReceiveLen)
{
  USART_COB Usart_RxCOB;
  BaseType_t xHigherPriorityTaskWoken;
  //Send To UART Receive Queue
  if (DR16_QueueHandle != NULL)
  {
    Usart_RxCOB.port_num = 2;
    Usart_RxCOB.len = ReceiveLen;
    Usart_RxCOB.address = Recv_Data;
    xQueueSendFromISR(DR16_QueueHandle, &Usart_RxCOB,&xHigherPriorityTaskWoken);
  }
  return 0;
}


