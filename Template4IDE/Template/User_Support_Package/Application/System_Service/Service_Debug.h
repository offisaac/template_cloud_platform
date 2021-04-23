/**
  **********************************************************************************
  * @file   : Service_Debug.h
  * @brief  : Header of Service_Debug.cpp
  * @author ：Mentos_Seetoo
  * @date   ：December,2018
  **********************************************************************************
  *   功能:(1)提供各种调试接口
**/
#ifndef  _SERVICE_DEBUG_H_
#define  _SERVICE_DEBUG_H_
#include <System_DataPool.h>
#ifdef  __cplusplus
extern "C"{
#endif
/*------------------------------ System Handlers ------------------------------*/
extern TaskHandle_t Debug_Handle;
void Task_Debug(void *arg);
/*------------------------------Function prototypes ---------------------------*/
uint32_t User_UART1_RxCpltCallback(uint8_t* Recv_Data, uint32_t ReceiveLen);
void Service_Debug_Init(void);


#ifdef  __cplusplus
}
#endif
#endif 
/************************ COPYRIGHT SCUT-ROBOTLAB *****END OF FILE*************/
