/**
  **********************************************************************************
  * @file   : App_TuneParameters.cpp
  * @brief  : This program is for Robomaster Engineer Remote Control
  *           (Robomaster 2019 version 4.0).
  * @author : Mentos_Seetoo
  * @date   : July,2019
  **********************************************************************************
  *   
**/
#include "App_RemoteControl.h"
#include "Service_Devices.h"
#include "Service_EngineerChassis.h"

/*Task Define---------------------------------------------------------------------*/
TaskHandle_t RemoteControl_Handle;
extern float debug_Xtarget,debug_Ytarget,debug_Ztarget;
float X_Co,Y_Co,Z_Co;
/*Function Prototypes-------------------------------------------------------------*/
/**
* @brief  Task for remote control.
*/
void Task_RemoteControl(void *arg)
{
  /* Cache for Task */
  uint32_t xLastTick;
  
  /* Pre-Load for task */
  /* Infinite loop ----------------------------------------------------------*/
  TickType_t xLastWakeTime_Remote;
  xLastWakeTime_Remote = xTaskGetTickCount();
  for(;;)
  { 
    if(DR16.GetStatus() == Connection_Established)
    { 
      //S1拨到上面使能速度模式或位置模式
      if(DR16.GetS1() == UP)
      {
        X_Co = debug_Xtarget/WHEEL_MAX_SPEED;
        Y_Co = debug_Ytarget/WHEEL_MAX_SPEED;
        Z_Co = debug_Ztarget/WHEEL_MAX_SPEED;
        Engineer_Master.Lunar_Chassis.Set_TorqueOptimizeFlag(OFF);
        Engineer_Master.Switch_ActionMode(NORMAL);
        Engineer_Master.Lunar_Chassis.Set_SpeedGear(FAST_GEAR);
        Engineer_Master.Manual_XYZ(X_Co, Y_Co, Z_Co);
//        Engineer_Master.Manual_XYZ(-DR16.Get_LX_Norm(),
//                                   -DR16.Get_LY_Norm(),
//                                   DR16.Get_RX_Norm());
//        Engineer_Master.Switch_ActionMode(GET_BULLET);
      }
      //S1在中间为锁死模式
      else if(DR16.GetS1() == MID)
      {
        Engineer_Master.Switch_ActionMode(CHASSIS_LOCK);
      }
      //S1在下面为摇杆模式
      else 
      {
//        //给弹模式下用陀螺仪积分的X，Y，Z，用来调位置环参数
        Engineer_Master.Lunar_Chassis.Set_TorqueOptimizeFlag(ON);
        Engineer_Master.Switch_ActionMode(NORMAL);
        Engineer_Master.Lunar_Chassis.Set_SpeedGear(NORMAL_GEAR);
        Engineer_Master.Manual_XYZ(-DR16.Get_LX_Norm(),
                                   -DR16.Get_LY_Norm(),
                                   DR16.Get_RX_Norm());
      }
    }
    //掉线保护
    else
    {
      Engineer_Master.Switch_ActionMode(CHASSIS_LOCK);
    }
   /* Pass control to the next task ------------------------------------------*/
    vTaskDelayUntil(&xLastWakeTime_Remote,1);
  }
}

/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
