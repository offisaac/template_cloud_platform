/**
  ***********************************************************************************
  * @file   : Service_Communication.cpp
  * @brief  : Communication support file.This file provides access ports to interface
  *           with connected devices. Communication service is in Data Link Layer and
              Transport Layer(Over time is allowed in this version).
  * @author ：Mentos_Seetoo
  * @date   ：February,2019
  * @version：1.0
  ***********************************************************************************
                                 ##### Port List #####
  =================================================================================
  |Port Name     Physical-Layer     Data-Link-Layer    Application-Layer    Number
  |————————————————————————————————————————
  |CAN1_Port          CAN1               CAN                DJI-CAN           0
  |CAN2_Port          CAN2               CAN                Custom            1
  |USART1_Port       USART1             Custom              Custom            2
  |USART2_Port       USART2              DBUS               DJI-DR16          3
  |USART3_Port       USART3           SerialLineIP          Custom            4
  |USART4_Port       USART4             Custom              Custom            5
  *
**/
#include "Service_Communication.h"
/* Middleware Protocol Support */
#include "SCUT_SerialLineIP.hpp"
#include "Yesense_usart.h"
#include "referee.h"
/**
* @brief  Initialization of communication service
* @param  None.
* @return None.
*/
void Service_Communication_Init(void)
{
  /* Register call back function */
  usart_rx_callback_register(&usart2_manage_obj, User_UART2_RxCpltCallback);
  usart_rx_callback_register(&usart3_manage_obj, User_UART3_RxCpltCallback);
  usart_rx_callback_register(&usart4_manage_obj, User_UART4_RxCpltCallback);
  usart_rx_callback_register(&usart5_manage_obj, User_UART5_RxCpltCallback);
  
  /* CAN Filter Config*/
  CAN_Filter_Mask_Config(CanFilter_1|CanFifo_0|Can_STDID|Can_DataType,0x200,0x3f0);//筛选器:|编号|FIFOx|ID类型|帧类型|ID|屏蔽位(0x3ff,0x1FFFFFFF)|
  CAN_Filter_Mask_Config(CanFilter_15|CanFifo_0|Can_STDID|Can_DataType,0x002,0x3ff);//筛选器:|编号|FIFOx|ID类型|帧类型|ID|屏蔽位(0x3ff,0x1FFFFFFF)|
  CAN_Filter_Mask_Config(CanFilter_14|CanFifo_0|Can_STDID|Can_DataType,0x003,0x3ff);//筛选器:|编号|FIFOx|ID类型|帧类型|ID|屏蔽位(0x3ff,0x1FFFFFFF)|
  
  xTaskCreate(Task_UsartTransmit,"Com.Usart TxPort" , Tiny_Stack_Size,    NULL, PriorityHigh,   &UartTransmitPort_Handle);
  xTaskCreate(Task_CAN1Transmit, "Com.CAN1 TxPort"  , Tiny_Stack_Size,    NULL, PrioritySuperHigh,   &CAN1SendPort_Handle);
  xTaskCreate(Task_CAN2Transmit, "Com.CAN2 TxPort"  , Tiny_Stack_Size,    NULL, PrioritySuperHigh,   &CAN2SendPort_Handle); 
}

/*----------------------------------------------- CAN Manager ---------------------------------------------*/
/*Task Define ---------------------------*/
TaskHandle_t CAN1SendPort_Handle;
TaskHandle_t CAN2SendPort_Handle;
TaskHandle_t CANReceivePort_Handle;
static void Convert_Data(CAN_RxMessage* input, COB_TypeDef* output);

/*Function Prototypes--------------------*/
/**
* @brief  Tasks for CAN Management.
* @param  None.
* @return None.
*/
void Task_CAN1Transmit(void *arg)
{
  /* Cache for Task */
  uint8_t free_can_mailbox;
  COB_TypeDef CAN_TxMsg;
  /* Pre-Load for task */
  
  /* Infinite loop */
  
  for(;;)
  {
    /* CAN1 Send Port */
    if(xQueueReceive(CAN1_TxPort,&CAN_TxMsg,portMAX_DELAY) == pdPASS)
    {
      do{
        free_can_mailbox = HAL_CAN_GetTxMailboxesFreeLevel(&hcan1);
      }while(free_can_mailbox == 0);
      CANx_SendData(&hcan1,CAN_TxMsg.ID,CAN_TxMsg.Data,CAN_TxMsg.DLC);
    }
  }
}

void Task_CAN2Transmit(void *arg)
{
  /* Cache for Task */
  uint8_t free_can_mailbox;
  COB_TypeDef CAN_TxMsg;
  /* Pre-Load for task */
  
  /* Infinite loop */
  
  for(;;)
  {
    /* CAN2 Send Port */
    if(xQueueReceive(CAN2_TxPort,&CAN_TxMsg,portMAX_DELAY) == pdPASS)
    {
      do{
        free_can_mailbox = HAL_CAN_GetTxMailboxesFreeLevel(&hcan2);
      }while(free_can_mailbox == 0);
      CANx_SendData(&hcan2,CAN_TxMsg.ID,CAN_TxMsg.Data,CAN_TxMsg.DLC);
    }
  }
}


/**
* @brief  Callback function in CAN Interrupt
* @param  None.
* @return None.
*/
void User_CAN1_RxCpltCallback(CAN_RxBuffer *CAN_RxMessage)
{
  static COB_TypeDef   CAN_RxCOB;
  Convert_Data(CAN_RxMessage,&CAN_RxCOB);
  //Send To CAN Receive Queue
  if((CAN_RxMessage->header.StdId & 0xF00) == 0x200)
  {
    if(RMMotor_QueueHandle != NULL)
      xQueueSendFromISR(RMMotor_QueueHandle,&CAN_RxCOB,0);
  }
}

void User_CAN2_RxCpltCallback(CAN_RxBuffer *CAN_RxMessage)
{
  static COB_TypeDef   CAN_RxCOB;
  Convert_Data(CAN_RxMessage,&CAN_RxCOB);
  //Send To CAN Receive Queue
  if((CAN_RxMessage->header.StdId & 0x3FF) == 0x002)
  {
    if(SuperRelay_QueueHandle != NULL)
      xQueueSendFromISR(SuperRelay_QueueHandle,&CAN_RxCOB,0);
  }
  else if((CAN_RxMessage->header.StdId & 0x3FF) == 0x003)
  {
    if(IMU_QueueHandle != NULL)
    xQueueSendFromISR(IMU_QueueHandle,&CAN_RxCOB,0);
  }
}


/**
* @brief  Date convertion，provide lower layer access port 
          for application layer.
* @param  CAN_RxMsg：Lower layer CAN frame.
          CAN_RxCOB：Application layer CAN Frame.
* @return None.
*/
static void Convert_Data(CAN_RxMessage* input, COB_TypeDef* output)
{
  output->ID = input->header.StdId;
  output->DLC = input->header.DLC;
  memcpy(output->Data, input->data, output->DLC);
}


/*---------------------------------------------- USART Manager --------------------------------------------*/
/*Task Define ---------------------------*/
TaskHandle_t UartTransmitPort_Handle;

/*Function Prototypes--------------------*/
/**
* @brief  Tasks for USART Management.
          Attention:In this version we passing the pointer of data not copying
          data itself and we only have one buff zone, so user need to process 
          the data as soon as possible in case of over-write.
* @param  None.
* @return None.
*/
void Task_UsartTransmit(void *arg)
{
  /* Cache for Task */
  UART_HandleTypeDef* pUart_x;
  static std::vector<uint8_t> Packed_COB;
  static USART_COB  Usart_TxCOB;
  /* Pre-Load for task */
  /* Infinite loop */
  for(;;)
  {
    /* Usart Receive Port*/
    if(xQueueReceive(USART_TxPort,&Usart_TxCOB,portMAX_DELAY) == pdPASS)
    {
      /* User Code Begin Here -------------------------------*/
      switch(Usart_TxCOB.port_num)
      {
        case 4:
          pUart_x = &huart3;
          break;
        case 5:
          pUart_x = &huart4;
          break;
      }
      /* User Code End Here ---------------------------------*/
      HAL_UART_Transmit_DMA(pUart_x,(uint8_t*)Usart_TxCOB.address,Usart_TxCOB.len);
    }
  }
}

/**
* @brief  Callback function in USART Interrupt
* @param  None.
* @return None.
*/
/*
  DR16
*/
uint32_t User_UART2_RxCpltCallback(uint8_t* Recv_Data, uint32_t ReceiveLen)
{
  static USART_COB Usart_RxCOB;
  //Send To UART Receive Queue
  if(DR16_QueueHandle != NULL)
  {
    Usart_RxCOB.port_num = 3;
    Usart_RxCOB.len      = ReceiveLen;
    Usart_RxCOB.address  = Recv_Data;
    xQueueSendFromISR(DR16_QueueHandle,&Usart_RxCOB,0);
  }
  return 0;
}

/*
  NUC
*/
uint32_t User_UART3_RxCpltCallback(uint8_t* Recv_Data, uint32_t ReceiveLen)
{
#ifdef BACK_UP_VERSION 
  static USART_COB Usart_RxCOB;
  
  //Send To UART Receive Queue
  if(NUC_QueueHandle != NULL)
  { 
    Usart_RxCOB.port_num = 4;
    Usart_RxCOB.address = Recv_Data;
    Usart_RxCOB.len = ReceiveLen;
    xQueueSendFromISR(NUC_QueueHandle,&Usart_RxCOB,0);
  }
#endif
  return 0;
}

uint32_t User_UART4_RxCpltCallback(uint8_t* Recv_Data, uint32_t ReceiveLen)
{
#ifndef BACK_UP_VERSION
  
  static USART_COB Usart_RxCOB;
  
  if(NUC_QueueHandle != NULL)
  { 
    Usart_RxCOB.port_num = 4;
    Usart_RxCOB.address = Recv_Data;
    Usart_RxCOB.len = ReceiveLen;
    xQueueSendFromISR(NUC_QueueHandle,&Usart_RxCOB,0);
  }
#endif
  return 0;
}

uint32_t User_UART5_RxCpltCallback(uint8_t* Recv_Data, uint32_t ReceiveLen)
{
  static USART_COB Usart_RxCOB;
  if(Referee_QueueHandle != NULL)
    unPackDataFromRF(Recv_Data,ReceiveLen);
  return 0;
}
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
