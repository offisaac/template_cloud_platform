/**
  **********************************************************************************
  * @file  ： System_DataPool.cpp
  * @brief   1)User can define datas including variables ,structs ,and arrays in
  *            this file, which are used in deffrient tasks.
  *          2)Datas that would be used in user tasks but defined in lower layer 
  *            also should be externed in <System_DataPool.h> before it's used.
  * @author：Mentos Seetoo
  ***********************************************************************************
  *
**/
#include "System_DataPool.h"

/*------------------------------ RTOS Resources ------------------------------*/
/***** Queue For Communication ******/
QueueHandle_t  USART_RxPort;
QueueHandle_t  USART_TxPort;
QueueHandle_t  CAN1_TxPort;
QueueHandle_t  CAN2_TxPort;

/***** Queue For System Service*****/
QueueHandle_t  DR16_QueueHandle;
QueueHandle_t  RMMotor_QueueHandle;
QueueHandle_t  IMU_QueueHandle;
QueueHandle_t  NUC_QueueHandle;
QueueHandle_t  SuperRelay_QueueHandle;
QueueHandle_t  Referee_QueueHandle;
/*---------------------------- Service Memory Pool ----------------------------*/

/**
* @brief    Initialize RTOS resources and Memory pools.
* @param    None.
* @return   None.
*/
void SysDataPool_Init(void)
{
  /*System resource Initialization*/
  USART_RxPort       = xQueueCreate(4,sizeof(USART_COB));
  USART_TxPort       = xQueueCreate(2,sizeof(USART_COB));
  CAN1_TxPort        = xQueueCreate(4,sizeof(COB_TypeDef));
  CAN2_TxPort        = xQueueCreate(4,sizeof(COB_TypeDef));
  SuperRelay_QueueHandle = xQueueCreate(2,sizeof(COB_TypeDef));
  RMMotor_QueueHandle= xQueueCreate(10,sizeof(COB_TypeDef));
  IMU_QueueHandle    = xQueueCreate(2,sizeof(COB_TypeDef));
  NUC_QueueHandle    = xQueueCreate(2,sizeof(USART_COB));
  DR16_QueueHandle   = xQueueCreate(2,sizeof(USART_COB));
  Referee_QueueHandle = xQueueCreate(2,sizeof(USART_COB));
  /*Memory Pool Initialization*/
  
}


/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/



