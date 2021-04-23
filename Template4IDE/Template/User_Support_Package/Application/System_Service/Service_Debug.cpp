/**
  **********************************************************************************
  * @file   : Service_Debug.cpp
  * @brief  : Debug support file.This file provides access ports to debug.
  * @author ：Mentos_Seetoo
  * @date   ：December,2018
  **********************************************************************************
  *   功能:(1)提供各种调试接口
**/
#include "Service_Debug.h"
#include <GCS_Debug.h>

/*Task Define---------------------------------------------------------------------*/
TaskHandle_t Debug_Handle;

/*Function Prototypes-------------------------------------------------------------*/
/**
* @brief  调试服务初始化
* @param  None.
* @return None.
*/
void Service_Debug_Init(void)
{
  /* Register call back function */
  usart_rx_callback_register(&usart1_manage_obj, User_UART1_RxCpltCallback);
  
  xTaskCreate(Task_Debug,       /* Task function. */
         "Debug_Service",       /* Task Name. */
         Tiny_Stack_Size,       /* Stack depth. */
                    NULL,       /* Task parameter */
     PriorityBelowNormal,       /* Priority */
          &Debug_Handle);       /* Task handle */
}

/*------------------------ High Layer (Application Layer) ------------------------*/
/* 以下代码用于整定工程4.0的控制器参数 */
/* Exported */
extern float debug_target,debug_Kp,debug_Ki,debug_Kd;
extern uint8_t debug_tune_type,debug_tune_obj;
extern PID* Chassis_Speed[6];
extern PID* Diff_Speed[6];
extern PID  ChassisPose_XAxis, ChassisPose_YAxis, ChassisPose_ZAxis;

enum Ctrl_Type
{
  Speed_Loop = 0U,
  DifferSpeed_Loop,
  Position_Loop,
};

enum Tune_Obj
{
  LF = 0U,
  RF,
  RB,
  LB,
  LM,
  RM,
  X_Axis,
  Y_Axis,
  Z_Axis
};

/**
* @brief  发送调试信息到GCS上位机(任务优先级最低)
* @param  None.
* @return None.
*/
void Task_Debug(void *arg)
{
  uint8_t i_count = 0;
  for(;;)
  {
    /*数据分流*/
    if(debug_tune_type == Speed_Loop)
    {
      Chassis_Speed[debug_tune_obj]->Kp = debug_Kp;
      Chassis_Speed[debug_tune_obj]->Ki = debug_Ki;
      Chassis_Speed[debug_tune_obj]->Kd = debug_Kd;
    }
    else if(debug_tune_type == DifferSpeed_Loop)
    {
      Diff_Speed[debug_tune_obj]->Kp = debug_Kp;
      Diff_Speed[debug_tune_obj]->Ki = debug_Ki;
      Diff_Speed[debug_tune_obj]->Kd = debug_Kd;
    }
    else if(debug_tune_type == Position_Loop)
    {
      switch(debug_tune_obj)
      {
        case X_Axis:
          ChassisPose_XAxis.Kp = debug_Kp;
          ChassisPose_XAxis.Ki = debug_Ki;
          ChassisPose_XAxis.Kd = debug_Kd;
          break;
        case Y_Axis:
          ChassisPose_YAxis.Kp = debug_Kp;
          ChassisPose_YAxis.Ki = debug_Ki;
          ChassisPose_YAxis.Kd = debug_Kd;
          break;
        case Z_Axis:
          ChassisPose_ZAxis.Kp = debug_Kp;
          ChassisPose_ZAxis.Ki = debug_Ki;
          ChassisPose_ZAxis.Kd = debug_Kd;
          break;
        default:
          break;
      }
    }
    /*上位机发送*/
    Sent_Contorl(&huart1);
    /*Pass to next ready task*/
    vTaskDelay(15);
  }
}


/*-------------------------- Low Layer (Physical Layer) --------------------------*/
/**
* @brief  串口1中断用户回调函数,上位机用
* @param  None.
* @return None.
*/
uint32_t User_UART1_RxCpltCallback(uint8_t* Recv_Data, uint32_t ReceiveLen)
{
  RecHandle(Recv_Data,ReceiveLen);
  return 0;
}




/************************ COPYRIGHT SCUT-ROBOTLAB *****END OF FILE*************/
