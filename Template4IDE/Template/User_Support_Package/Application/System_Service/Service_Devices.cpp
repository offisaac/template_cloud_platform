 /**
  ***********************************************************************************
  * @file   : Service_Devices.cpp
  * @brief  : Device service support file.This file provide
  * @author ：Mentos_Seetoo
  * @date   ：June,2019
  * @version：0.0.1
  ***********************************************************************************
                                ##### Devices List #####
  =================================================================================
  |Device Name   Application Protocol  Running Frequency
  |——————————————————————————————
  *
**/
#include "Service_Devices.h"

/*------------------------------ System Handlers ------------------------------*/
TaskHandle_t DeviceESC_Handle;
TaskHandle_t DeviceDR16_Handle;
TaskHandle_t DeviceSensors_Handle;

/* 电调 */
ESC_C620 C620_LF(1,OTHER);
ESC_C620 C620_RF(2,OTHER);
ESC_C620 C620_RB(3,OTHER);
ESC_C620 C620_LB(4,OTHER);
ESC_C620 C620_LM(5,OTHER);
ESC_C620 C620_RM(6,OTHER);

/* 遥控器 */
DR16_Classdef DR16;

/* IMU */
Gyro_Classdef IMU_Obj;

/* 视觉NUC */
VisionAlign_Classdef NUC_Obj;

/* 超级继电器板 */
CSuperRelay SuperRelay_Obj;

/**
* @brief  Initialization of device management service
* @param  None.
* @return None.
*/
void Service_Devices_Init(void)
{
  xTaskCreate(Device_ESC,    "Dev.C620"     , Tiny_Stack_Size,    NULL, PrioritySuperHigh,   &DeviceESC_Handle);
  xTaskCreate(Device_DR16,   "Dev.DR16"     , Tiny_Stack_Size,    NULL, PriorityHigh,   &DeviceDR16_Handle);
  xTaskCreate(Device_Sensors,"Dev.Sensors"  , Tiny_Stack_Size,    NULL, PriorityHigh,   &DeviceSensors_Handle);
  xTaskCreate(Device_Sensors,"Dev.Sensors"  , Tiny_Stack_Size,    NULL, PriorityHigh,   &DeviceSensors_Handle);
}

/*----------------------------------------------- Devices Manager ---------------------------------------------*/
/*Task Define ---------------------------*/

/*Function Prototypes--------------------*/
/**
* @brief  Tasks for Devices Management.
*/
void Task_DeviceManager(void *arg)
{
  /* Cache for Task */
  static CAN_RxMessage CAN_RxMsg;
  static COB_TypeDef   CAN_RxCOB;
  /* Pre-Load for task */
  /* Infinite loop */
  for(;;)
  {
    
  }
}

/* User Code Begin Here -------------------------------*/
void Device_ESC(void *arg)
{
  /* Cache for Task */
  int32_t xLastTick;
  uint8_t index;
  static COB_TypeDef  Rx_Msg;
  static COB_TypeDef* Tx_Msg;
  /* Pre-Load for task */

  /* Infinite loop ----------------------------------------------------------*/
  TickType_t xLastWakeTime_RMMotorControl;
  xLastWakeTime_RMMotorControl = xTaskGetTickCount();
  for(;;)
  {
    /* Connection check */
    xLastTick = xTaskGetTickCount();
    for(index = 0; index < ESC_C620::total_num ; ++index)
      ESC_C620::ESC_List[index]->Check_Link(xLastTick);
    
    /* Read Message */
    while(xQueueReceive(RMMotor_QueueHandle,&Rx_Msg,0) == pdPASS)
    {
      ESC_C620::Read_All(&Rx_Msg);
    }
    
    /* Send Message */
    Tx_Msg = ESC_C620::Get_TxCOB(Group1);
    if(Tx_Msg != NULL)
      xQueueSend(CAN1_TxPort,Tx_Msg,0);
    Tx_Msg = ESC_C620::Get_TxCOB(Group0);
    if(Tx_Msg != NULL)
      xQueueSend(CAN1_TxPort,Tx_Msg,0);
    
  /* Pass control to the next task ------------------------------------------*/
    vTaskDelayUntil(&xLastWakeTime_RMMotorControl,1);
  }
}

void Device_DR16(void *arg)
{
  /* Cache for Task */
  int32_t xLastTick;
  static USART_COB Rx_Package;
  /* Pre-Load for task */

  /* Infinite loop ----------------------------------------------------------*/
  TickType_t xLastWakeTime_DR16;
  xLastWakeTime_DR16 = xTaskGetTickCount();
  for(;;)
  {
    xLastTick = xTaskGetTickCount();
    DR16.Check_Link(xLastTick);
    while(xQueueReceive(DR16_QueueHandle,&Rx_Package,0) == pdPASS)
    {
      /* Read Message */
      DR16.SetStatus(Connection_Established);
      DR16.DataCapture((DR16_DataPack_Typedef*)Rx_Package.address);
    }
    
  /* Pass control to the next task ------------------------------------------*/
    vTaskDelayUntil(&xLastWakeTime_DR16,1);
  }
}

void Device_Sensors(void *arg)
{
  /* Cache for Task */
  int32_t xLastTick;
  static USART_COB    UsartRx_Package;
  static USART_COB    UsartTx_Package;
  static COB_TypeDef  CanRx_Package;
  /* Pre-Load for task */
  UsartTx_Package.len = 1;
  
#ifdef BACK_UP_VERSION
  UsartTx_Package.port_num = 4;
#else
  UsartTx_Package.port_num = 5;
#endif
  
  IMU_Obj.Reset();
  /* Infinite loop ----------------------------------------------------------*/
  TickType_t xLastWakeTime_Sensors;
  xLastWakeTime_Sensors = xTaskGetTickCount();
  for(;;)
  {
    xLastTick = xTaskGetTickCount();
    /* Read IMU Message */
    IMU_Obj.Check_Link(xLastTick);
    while(xQueueReceive(IMU_QueueHandle,&CanRx_Package,0) == pdPASS)
    {
      IMU_Obj.SetStatus(Connection_Established);
      IMU_Obj.DataCapture(CanRx_Package.Data);
    }

    /* Exchange NUC Meaasge */
    NUC_Obj.UpdateTimeStamp(xLastTick);
    while(xQueueReceive(NUC_QueueHandle,&UsartRx_Package,0) == pdPASS)
    {
      NUC_Obj.DataCapture((Vision_DataPack_Typedef*)UsartRx_Package.address);
    }
    UsartTx_Package.address = NUC_Obj.Get_TxCOB();
    
    if(UsartTx_Package.address != NULL)
      xQueueSend(USART_TxPort,&UsartTx_Package,0);
    
    /* Read Super Relay Board Message */
    while(xQueueReceive(SuperRelay_QueueHandle,&CanRx_Package,0) == pdPASS)
    {
      memcpy(&SuperRelay_Obj.Data_Package,CanRx_Package.Data,8);
    }
    
  /* Pass control to the next task ------------------------------------------*/
    vTaskDelayUntil(&xLastWakeTime_Sensors,1);
  }
}

/* User Code End Here ---------------------------------*/

/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
