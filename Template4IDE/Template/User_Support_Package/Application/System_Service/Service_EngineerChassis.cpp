/**
  **************************************************************************************
  * @file   : Service_EngineerChassis.cpp
  * @brief  : Engineer Chassis Motion support file.This file provides service to control
  *           the motion of Engineer Chassis.There are three parts contained in this 
  *           service, which are 'Sensor Fusion','Center Control' and 'Action Execute'. 
  *           User can command robot to move and act through this service.
  *           More details of these three parts are illustrated below:
  *           (1) Sensor Fusion: This part is to provide tasks to read data from sensors 
  *               and solve the attitude of robot.
  *           (2) Center Control: 
  *           (3) Action Execute: This part is to provide tasks to control the operator
  *           and excute actions command.
  * @author ：Mentos_Seetoo
  * @date   ：June,2019
  * @version：1.0
  ***************************************************************************************
**/
#include "Service_EngineerChassis.h"
#include "Service_Devices.h"
#include "Tool_Filter.h"
/*----------------------------- Private Data Type -----------------------------*/
/* Robot Core Object */
CEngineer Engineer_Master;
extern float debug_easy_vb1,debug_easy_vb2;
/**
* @brief  Initialization of Engineer-Chassis service
* @param  None.
* @return None.
*/
void Service_EngineerChassis_Init(void)
{
  //Create service task
  xTaskCreate(Task_EngineerControl,"Robo.Engineer Ctrl", Small_Stack_Size, NULL,PriorityHigh, &EngineerControl_Handle);
}

/*------------------------------- Engineer Control -----------------------------*/
/*Task Define ---------------------------*/
TaskHandle_t EngineerControl_Handle;

/*Function Prototypes--------------------*/
/**
* @brief  Task for central control.
* @param  None.
* @return None.
*/
int32_t debug_vision_x,debug_vision_y,debug_vision_z;
void Task_EngineerControl(void *arg)
{
  /* Cache for Task */
  uint8_t index;
  uint8_t temp_magnet = 0,temp_PES_Lift = 0, temp_PES_Stop = 0;
  static _Body_Operation_t *Operation_Flags;
  static float VisionX_SliderFilter[10] = {0},
               VisionY_SliderFilter[10] = {0},
               VisionZ_SliderFilter[10] = {0};
  static uint8_t X_FilterIndex = 0, Y_FilterIndex, Z_FilterIndex = 0;
  /* Pre-Load for task */
  Engineer_Master.Init();
  /* Infinite loop ----------------------------------------------------------*/
  TickType_t xLastWakeTime_CenterControl;
  xLastWakeTime_CenterControl = xTaskGetTickCount();
  for(;;)
  {
    //Engineer_Master.threshold_time = debug_easy_vb1;
    Engineer_Master.Update_NowTime(xTaskGetTickCount());
    /* ------------------------------ Sensor Fusion ------------------------------ */
    //Update Flags
    Operation_Flags = Engineer_Master.IO_Port(BODY_OPERATION_OUT);
    Operation_Flags->detected_flag = NUC_Obj.Is_Detected();
    temp_magnet    = HAL_GPIO_ReadPin(Magnet_Lift_GPIO_Port,Magnet_Lift_Pin);
    temp_PES_Lift  = !HAL_GPIO_ReadPin(PES_Lift_GPIO_Port,PES_Lift_Pin);
    temp_PES_Stop  = !HAL_GPIO_ReadPin(PES_Stop_GPIO_Port,PES_Stop_Pin);

#ifdef BACK_UP_VERSION
    Engineer_Master.IO_Port(IMU_IN, IMU_Obj.GetRoll(), IMU_Obj.GetPitch(), IMU_Obj.GetYaw());
#else
    //Engineer_Master.IO_Port(IMU_IN, IMU_Obj.GetRoll(), -IMU_Obj.GetPitch(), IMU_Obj.GetYaw());
#endif
    Engineer_Master.IO_Port(CHASSIS_MAGNET, CHASSIS_MAGNET_IN, temp_magnet);
    Engineer_Master.IO_Port(CHASSIS_PES_PULL, CHASSIS_PES_IN, temp_PES_Lift);
    Engineer_Master.IO_Port(CHASSIS_PES_STOP, CHASSIS_PES_IN, temp_PES_Stop);

    for(index = 0 ; index < WHEEL_NUM ; ++index)
    { //为保证写入顺序，请确保数组顺序为_Unit_Type中的顺序，以下循环有同样要求
      Engineer_Master.IO_Port((_Unit_Type)index, VELOCITY_IN, ESC_C620::ESC_List[index]->Motor.rpm);
      Engineer_Master.IO_Port((_Unit_Type)index, TORQUE_IN, ESC_C620::ESC_List[index]->Motor.current);
      Engineer_Master.IO_Port((_Unit_Type)index, CHASSIS_TEMPRE_IN, ESC_C620::ESC_List[index]->Motor.temperature);
      Engineer_Master.IO_Port((_Unit_Type)index, CONNECTION_STATE_IN, ESC_C620::ESC_List[index]->Is_Connected());
    }
    
    
    /*Debug Code Below*/
    debug_vision_x = (int32_t)NUC_Obj.Vision_DataPack.Vision_X;
    debug_vision_y = (int32_t)NUC_Obj.Vision_DataPack.Vision_Y;
    debug_vision_z = (int32_t)NUC_Obj.Vision_DataPack.Vision_Z;
    /*Debug Code Beyond*/

    /* -------------------------------------- Central Control -------------------------------- */
    Engineer_Master.Central_Control();
    
    /* --------------------------------------- Action Excute --------------------------------- */
    //Motor Output
    if(DR16.GetStatus() == Connection_Established){
      for(index = 0 ; index < WHEEL_NUM ; ++index)
      {
        ESC_C620::ESC_List[index]->Motor.SetTarget(Engineer_Master.IO_Port((_Unit_Type)index, VELOCITY_OUT));
      }
    }
    else {
      for(index = 0 ; index < WHEEL_NUM ; ++index)
        ESC_C620::ESC_List[index]->Motor.SetTarget(0);
    }
    
    //Switch view by servo
    if(Engineer_Master.view_flag == FORWARD) {Look_Forward()}
    else {Look_Down();}
    
    //Lift, lay down or release the chassis.
    if(Engineer_Master.Get_CurrentMode() == UP_STAIRS)
    {
      if(Operation_Flags->upstairs_flag == STAGE1_LIFT || Operation_Flags->upstairs_flag == STAGE2_LIFT)
        SuperRelay_Obj.Write_Cmd(Upstairs_Cylinder_ON);
      
      else if(Operation_Flags->upstairs_flag == TRANSITION)
        SuperRelay_Obj.Write_Cmd(Upstairs_Cylinder_OFF);
      
      else if(Operation_Flags->upstairs_flag == MANUAL_UPSTAIRS)
        SuperRelay_Obj.Write_Cmd(Upstairs_Cylinder_Release);
    }
    else
    {
      SuperRelay_Obj.Write_Cmd(Upstairs_Cylinder_Release);
    }
    
  /* Pass control to the next task ------------------------------------------*/
    vTaskDelayUntil(&xLastWakeTime_CenterControl,1);
  }
}

/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/

