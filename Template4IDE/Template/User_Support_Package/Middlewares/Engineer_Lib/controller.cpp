/**
  *************************************************************************************
  * @file   : controller.cpp
  * @brief  : Header for controller(Robomaster 2019 Engineer version 4.0) robot lib.
  *           This file provides several controllers of 'Wheel Speed Loop', 
  *           'Differential Speed Loop', 'Position Loop', and 'Attitude Control Loop'.
  * @author : Mentos_Seetoo
  * @date   : July,2019
  * @version: 1.0
  ************************************************************************************
*/
/* Includes ------------------------------------------------------------------*/ 
#include "controller.h"
#include "Engineer.h"
/* Algorithm Support */
#include "PID.h"
#include "Tool_PID.h"

float MAX_ADJUST_SPEED = 1000.0f;

/* Controller List*/
myPID Wheel_LF(18,40,0),
      Wheel_RF(18,40,0),
      Wheel_RM(18,40,0),
      Wheel_RB(18,40,0),
      Wheel_LB(18,40,0),
      Wheel_LM(18,40,0),
      DiffWheel_LF(1,1,0),
      DiffWheel_RF(1,1,0),
      DiffWheel_RM(1,1,0),
      DiffWheel_RB(1,1,0),
      DiffWheel_LB(1,1,0),
      DiffWheel_LM(1,1,0),
      Pose_Pitch(0,0,0);

/* Using FuzzyPID for position loop with the feedback from vision*/
FUZZYPID_without_gyroscope Pose_XAxis, Pose_YAxis, Pose_Yaw;

myPID* Chassis_Speed[WHEEL_NUM] = {&Wheel_LF, &Wheel_RF, &Wheel_RB, &Wheel_LB, &Wheel_LM, &Wheel_RM};
myPID* Diff_Speed[WHEEL_NUM] = {&DiffWheel_LF, &DiffWheel_RF, &DiffWheel_RB, &DiffWheel_LB, &DiffWheel_LM, &DiffWheel_RM};
/* Functions -----------------------------------------------------------------*/
/**
* @brief  Initialize controller parameters for Fuzzy PID
* @param  None.
* @return None.
* @author Mentos Seetoo
*/
void Controller_Init(void)
{
#ifdef BACK_UP_VERSION
  /*X-Axis*/
  Pose_XAxis.Set_FUZZYPID(           //一个单位0.1mm
  0.0f,0.0f,0.0f,-0.0f,                     //P,I,D,Target
  -10000.0f, +10000.0f, -10000.0f, +10000.0f, -10000.0f, +10000.0f,
                                            //PTermMin,PTermMax,ITermMin,ITermMax,DTermMin,DTermMax
  -800.0f,+800.0f,0,                        //N_Target_MIN,P_Target_MAX,DeadZone
  -200,200,-100,100,-15,15,         //NB 负大的量度
  5.0f,5.0f, 4.0f,4.0f, 20.0f, 20.0f,
  0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f,
  0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f);
  
  /*Y-Axis*/
  Pose_YAxis.Set_FUZZYPID(//一个单位0.1mm
  0.0f,0.0f,0.0f,0.0f,//P,I,D,Target
  -10000.0f, +10000.0f, -10000.0f, +10000.0f, -10000.0f, +10000.0f,	//PTermMin,PTermMax,ITermMin,ITermMax,DTermMin,DTermMax
  -3000.0f,+3000.0f,0,//N_Target_MIN,P_Target_MAX,DeadZone
  -700,700,-400,400,-25,25,//NB 负大的量度
  5.0f,5.0f, 3.0f,3.0f, 6.0f,6.0f,
  0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f,
  0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f);
  
  Pose_Yaw.Set_FUZZYPID(//一个单位0.01度
  0.0f,0.0f,0.0f,0.0f,//P,I,D,Target
  -10000.0f, +10000.0f, -10000.0f, +10000.0f, -10000.0f, +10000.0f,	//PTermMin,PTermMax,ITermMin,ITermMax,DTermMin,DTermMax
  -3000.0f,+3000.0f,0,//N_Target_MIN,P_Target_MAX,DeadZone
  -40,40,-20,20,-5,5,//NB 负大的量度
  -25.0f,-25.0f, -50.0f,-50.0f, -70.0f,-70.0f,
  0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f,
  0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f);
#else
  /*X-Axis*/
  Pose_XAxis.Set_FUZZYPID(           //一个单位0.1mm
  0.0f,0.0f,0.0f,-0.0f,                     //P,I,D,Target
  -10000.0f, +10000.0f, -10000.0f, +10000.0f, -10000.0f, +10000.0f,
                                            //PTermMin,PTermMax,ITermMin,ITermMax,DTermMin,DTermMax
  -800.0f,+800.0f,0,                        //N_Target_MIN,P_Target_MAX,DeadZone
  -200,200,-100,100,-15,15,         //NB 负大的量度
  5.0f,5.0f, 4.0f,4.0f, 20.0f, 20.0f,
  0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f,
  0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f);
  
  /*Y-Axis*/
  Pose_YAxis.Set_FUZZYPID(//一个单位0.1mm
  0.0f,0.0f,0.0f,0.0f,//P,I,D,Target
  -10000.0f, +10000.0f, -10000.0f, +10000.0f, -10000.0f, +10000.0f,	//PTermMin,PTermMax,ITermMin,ITermMax,DTermMin,DTermMax
  -3000.0f,+3000.0f,0,//N_Target_MIN,P_Target_MAX,DeadZone
  -700,700,-400,400,-25,25,//NB 负大的量度
  5.0f,5.0f, 3.0f,3.0f, 6.0f,6.0f,
  0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f,
  0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f);
  
  Pose_Yaw.Set_FUZZYPID(//一个单位0.01度
  0.0f,0.0f,0.0f,0.0f,//P,I,D,Target
  -10000.0f, +10000.0f, -10000.0f, +10000.0f, -10000.0f, +10000.0f,	//PTermMin,PTermMax,ITermMin,ITermMax,DTermMin,DTermMax
  -3000.0f,+3000.0f,0,//N_Target_MIN,P_Target_MAX,DeadZone
  -40,40,-20,20,-5,5,//NB 负大的量度
  -25.0f,-25.0f, -50.0f,-50.0f, -70.0f,-70.0f,
  0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f,
  0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f);
#endif
}

void Controller_SetMaxAdjustSpeed(float max_adjust_speed)
{
  MAX_ADJUST_SPEED = max_adjust_speed;
}

/**
* @brief  Chassis position(x,y) & pose(yaw) PID.
*         Details:
* @param  CurrentPose, TargetPose
* @return _chassis_Velocity
* @author Mentos Seetoo
*/
_chassis_Velocity* Controller_ChassisPositionLoop(const _chassis_GlobalPos* CurrentPose, const _chassis_GlobalPos* TargetPose)
{
  static _chassis_Velocity Target_Buf;
  
  //Update current value & set the target.
  Pose_XAxis.Current = CurrentPose->x;
  Pose_YAxis.Current = CurrentPose->y;
  Pose_Yaw.Current = CurrentPose->yaw;
  Pose_XAxis.Target  = myConstrain(TargetPose->x,-1000.0f,1000.0f);
  Pose_YAxis.Target  = myConstrain(TargetPose->y,-1500.0f,1500.0f);
  Pose_Yaw.Target  = TargetPose->yaw;
  
  //Calculate output velocity.
  Pose_XAxis.AdjustPID();
  Pose_YAxis.AdjustPID();
  Pose_Yaw.AdjustPID();
  
  Target_Buf.x_speed = myConstrain(Pose_XAxis.Out, -MAX_ADJUST_SPEED, MAX_ADJUST_SPEED);
  Target_Buf.y_speed = myConstrain(Pose_YAxis.Out, -MAX_ADJUST_SPEED, MAX_ADJUST_SPEED);
  Target_Buf.z_speed = myConstrain(Pose_Yaw.Out, -MAX_ADJUST_SPEED, MAX_ADJUST_SPEED);
  
  return &Target_Buf;
}

_chassis_Velocity* Controller_ChassisAttitudeLoop(const _chassis_GlobalPos* CurrentPose, const _chassis_GlobalPos* TargetPose)
{
  static _chassis_Velocity Target_Buf;

  //Update current value & set the target.Convert rad to degree.
  Pose_Yaw.Current = CurrentPose->yaw;
  Pose_Yaw.Target = TargetPose->yaw;
  
  //Calculate output velocity.
  Pose_Yaw.AdjustPID();
  
  Target_Buf.x_speed = 0;
  Target_Buf.y_speed = 0;
  Target_Buf.z_speed = Pose_Yaw.Out;
  
  return &Target_Buf;
}

/**
* @brief  Chassis speed PID & differ speed PID for lunar chassis.
*         Details:
* @param  currentSpeed,targetSpeed
* @return Output value.
* @author Mentos Seetoo
*/
extern CEngineer Engineer_Master;
int32_t* Controller_ChassisSpeedLoop(const int16_t* currentSpeed, const int16_t* targetSpeed)
{
  static int32_t Output_Buf[WHEEL_NUM];
  uint8_t i = 0;

  for(i = 0 ; i < WHEEL_NUM ; ++i)
  {
    /* Speed loop */
    Chassis_Speed[i]->Current = *(currentSpeed + i);
    Chassis_Speed[i]->Target  = *(targetSpeed  + i);
    Chassis_Speed[i]->Adjust();
    Output_Buf[i] = Chassis_Speed[i]->Out;
    if(Engineer_Master.Get_CurrentMode() == UP_STAIRS && Engineer_Master.Operations.upstairs_flag == MANUAL_UPSTAIRS)
    {
      if(i == 2 || i ==3)
        Output_Buf[i] = *(targetSpeed  + i);
    }
    /* Differ speed loop */
    
  }
  
  return Output_Buf;
}


/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
