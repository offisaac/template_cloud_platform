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
#include "Service_EngineerChassis.h"
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
        Small_Stack_Size,       /* Stack depth. */
                    NULL,       /* Task parameter */
          PriorityNormal,       /* Priority */
          &Debug_Handle);       /* Task handle */
}

/*------------------------ High Layer (Application Layer) ------------------------*/
/* 以下代码用于整定工程4.0的控制器参数 */
/* Exported */
#include "PID.h"
extern float debug_Xtarget,debug_Ytarget,debug_Ztarget,debug_Kp,debug_Ki,debug_Kd,debug_gear,debug_easy_vb1,debug_easy_vb2;
extern uint8_t debug_tune_type,debug_tune_obj;
extern myPID* Chassis_Speed[6];
extern myPID* Diff_Speed[6];
extern FUZZYPID_without_gyroscope  Pose_XAxis, Pose_YAxis, Pose_Yaw;
extern _chassis_Parameter debug_Param;
extern uint8_t debug_VisionMode;

enum Ctrl_Type
{
  Speed_Loop = 0U,
  DifferSpeed_Loop,
  Position_Loop,
  Motion_Param
};

enum Tune_Obj
{
  ALL = 0U,
  LF_Obj,
  RF_Obj,
  RB_Obj,
  LB_Obj,
  LM_Obj,
  RM_Obj,
  X_Axis_Obj,
  Y_Axis_Obj,
  Z_Axis_Obj
};

/**
* @brief  发送调试信息到GCS上位机(任务优先级最低)
* @param  None.
* @return None.
*/
void Task_Debug(void *arg)
{
  static uint8_t temp_visionMode;
  static USART_COB  Usart_TxCOB;
  uint8_t i_count = 0;
  for(;;)
  {
    /* 数据修改 */
    //速度环
    if(debug_tune_type == Speed_Loop)
    {
      if(debug_tune_obj == ALL)
      {
        for(i_count = 0; i_count < 6 ; ++i_count)
          Chassis_Speed[i_count]->SetPIDParam(debug_Kp,debug_Ki,debug_Kd);
      }
      else{
        Chassis_Speed[debug_tune_obj]->Kp = debug_Kp;
        Chassis_Speed[debug_tune_obj]->Ki = debug_Ki;
        Chassis_Speed[debug_tune_obj]->Kd = debug_Kd;
      }
    }
    //差速环
    else if(debug_tune_type == DifferSpeed_Loop)
    {
      if(debug_tune_obj == ALL)
      {
        for(i_count = 0; i_count < 6 ; ++i_count)
          Diff_Speed[i_count]->SetPIDParam(debug_Kp,debug_Ki,debug_Kd);
      }
      else{
        Diff_Speed[debug_tune_obj]->Kp = debug_Kp;
        Diff_Speed[debug_tune_obj]->Ki = debug_Ki;
        Diff_Speed[debug_tune_obj]->Kd = debug_Kd;
      }
    }
    //位置环
    else if(debug_tune_type == Position_Loop)
    {
      switch(debug_tune_obj)
      {
        case X_Axis_Obj:
          Pose_XAxis.Kp = debug_Kp;
          Pose_XAxis.Ki = debug_Ki;
          Pose_XAxis.Kd = debug_Kd;
          break;
        case Y_Axis_Obj:
          Pose_YAxis.Kp = debug_Kp;
          Pose_YAxis.Ki = debug_Ki;
          Pose_YAxis.Kd = debug_Kd;
          break;
        case Z_Axis_Obj:
          Pose_Yaw.Kp = debug_Kp;
          Pose_Yaw.Ki = debug_Ki;
          Pose_Yaw.Kd = debug_Kd;
          break;
        default:
          break;
      }
    }
    //机动参数
    else if(debug_tune_type == Motion_Param)
    {
      Engineer_Master.Lunar_Chassis.Set_SpeedGear((_chassis_SpeedGears) debug_gear);
      Engineer_Master.Lunar_Chassis.Set_AccelerationParam(debug_Param.max_launch_acceleration, debug_Param.max_normal_acceleration);
      Engineer_Master.Lunar_Chassis.Set_SpeedParam(debug_Param.coefficient_slow, 
                                                   debug_Param.coefficient_normal, 
                                                   debug_Param.coefficient_fast,
                                                   debug_Param.coefficient_z
                                                   );
    }
    if(debug_VisionMode != temp_visionMode)
    {
      temp_visionMode = debug_VisionMode;
      Usart_TxCOB.address = &temp_visionMode;
      Usart_TxCOB.len = 1;
      Usart_TxCOB.port_num = 4;
      if(USART_TxPort != NULL)
        xQueueSend(USART_TxPort,&Usart_TxCOB,0);
    }
    /*上位机发送*/
    Sent_Contorl(&huart1);
    /*Pass to next ready task*/
    vTaskDelay(1);
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
