/**
  ******************************************************************************
  * Copyright (c) 2019 - ~, RobotLab Development Team of SCUT
  * @file   ：Chassis.cpp
  * @brief  ：Chassis model support file.This file provide object for general
  *           chassis(4 wheels) and a special chassis(Lunar Chassis,6 wheels).
  *           The object contain three movement modes(Normal-Speed, Normal-
  *           Position and Follow-Position).
  * @version：0.0.1
  * @changes：
 *  Date           Author   Version    Notes
 *  2019-06-22     Mentos    0.0.1     first version
  ******************************************************************************
  ==============================================================================
                     ##### How to use this Lib #####
  ==============================================================================
  1.在头文件配置用户的模型参数，注意计算中使用的单位
  
  2.新建"CChassis"对象，构造时设置初始的底盘机动性能参数，选择是否进行力矩优化和姿态
  保护.加载外部的控制器Load_PositionController().
  
  3.按照配置的运行频率调用 Chassis_Control().通过Set_xx()设置参数、目标
    使用Update_xx()更新一些当前值（位置模式下使用）
  
  注意：在整定底盘控制器参数时，请注意通过调节运动加速度大小，使任何一个电机输出不会
       长时间被输出限幅作用（大于电机最大输出时进入饱和区，使底盘运动性能恶化）。
        
  后续版本建议把排序，限幅等算法使用函数指针从外部实现。
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "chassis.h"

/* Functions -----------------------------------------------------------------*/
_chassis_Mode CChassis::Chassis_Control()
{
  //位置环的速度输出,无位置环时使用外部速度指令
  position_control();

  //外部速度指令时启动输出力矩优化
  torque_optimization();
  
  //姿态保护，在优化的基础上叠加输出，具有最高优先级
  attitude_control();
  
  //最终输出轮子速度解算
  speed_control();
  
  return Mode;
}

/**
* @brief  Resolve the command velocity from command pose in 
          position mode.
* @param  None.
* @return None.
* @author Mentos Seetoo
*/
void CChassis::position_control()
{
  static _chassis_GlobalPos Target_Pos,Relative_Pos;
  
  /*Check Postion controller*/
  //assert(position_controller != NULL);
  
  /* Only in position mode */
  Target_Pos = Command_Pos;
  if(Mode == Normal_Position)
  {
    Relative_Pos = Current_Pos - Zero_Pos;
    //Calculate target velocity by external controller.
    Target_Velocity = *(position_controller(&Relative_Pos, &Target_Pos));
  }
  else if(Mode == Follow_Position)
  {
    //Current_Pos & Zero_Pos is disable in this mode.
    memset(&Relative_Pos,0,sizeof(_chassis_GlobalPos));
    
    //Calculate target velocity by external controller.
    Target_Velocity = *(position_controller(&Relative_Pos, &Target_Pos));
  }
}

/**
* @brief  Optimize the torque while launching or breaking.
* @param  _chassis_Velocity*: user input target velocity to optimize.
* @author  Mentos_Seetoo
*/
void CChassis::torque_optimization()
{
  static _chassis_Velocity Last_Target;
  static float dspeed,max_accceleration;
  
  /* Only in speed mode */
  if(Mode == Normal_Speed)
  {
    if(Param.torqueOptimize_flag == 1)
    {
      /*
        Launch.(Acceleration = rpm / time)
        If the acceleration(torque) for launch is too large, the chassis is
        easy to slip. When the chassis isn't slipped, distance to speed up 
        will decrease. Below code first change 'Last_Target' because 
        'Target_Velocity' may be changed in elsewhere.
      */
      /* X Axis */
      if(_Chassis_Abs(Command_Velocity.x_speed) > _Chassis_Abs(Last_Target.x_speed))
      { //accelerate
        if(_Chassis_Abs(Command_Velocity.x_speed) <= Param.launch_speed)
          max_accceleration = Param.max_launch_acceleration;
        else
          max_accceleration = Param.max_normal_acceleration;
      }
      else//decelerate
        max_accceleration = Param.max_brake_acceleration;
      
      dspeed = max_accceleration*((float)Param.task_run_interval);
      
      if(_Chassis_Abs(Command_Velocity.x_speed - Last_Target.x_speed) >= dspeed)
      {
        if(Command_Velocity.x_speed >= Last_Target.x_speed)
          Last_Target.x_speed += dspeed;
        else Last_Target.x_speed -= dspeed;
      }
      else
        Last_Target.x_speed = Command_Velocity.x_speed;
      
      /* Y Axis */
      if(_Chassis_Abs(Command_Velocity.y_speed) > _Chassis_Abs(Last_Target.y_speed))
      { //accelerate
        if(_Chassis_Abs(Command_Velocity.y_speed) <= Param.launch_speed)
          max_accceleration = Param.max_launch_acceleration;
        else
          max_accceleration = Param.max_normal_acceleration;
      }
      else//decelerate
        max_accceleration = Param.max_brake_acceleration;
      
      dspeed = max_accceleration*((float)Param.task_run_interval);
      
      if(_Chassis_Abs(Command_Velocity.y_speed - Last_Target.y_speed) >= dspeed)
      {
        if(Command_Velocity.y_speed >= Last_Target.y_speed)
          Last_Target.y_speed += dspeed;
        else Last_Target.y_speed -= dspeed;
      }
      else
        Last_Target.y_speed = Command_Velocity.y_speed;
      
      //Z don't limit acceleration
      Last_Target.z_speed = Command_Velocity.z_speed;
    }
    else
      Last_Target = Command_Velocity;
    
    Target_Velocity = Last_Target;
  }
}

/**
* @brief  Protect the chassis from turn-over.Unit: degree.
* @param  None.
* @return None.
* @author Mentos Seetoo
*/
void CChassis::attitude_control()
{
  static float dYaw = 0, locked_yaw = 0;
  static const float co_rpm2w = 360/60;
  static const float co_wheel2chassis = (CHASSIS_WHEEL_TRACK*WHEEL_RADIUS)/(CHASSIS_WHEEL_BASE*CHASSIS_WHEEL_BASE + CHASSIS_WHEEL_TRACK*CHASSIS_WHEEL_TRACK);
  static _chassis_Velocity* Attitude_Compensation;
  
  /*Check Postion controller*/
  //assert(position_controller != NULL);
  
  if(Target_Velocity.z_speed != 0)
    locked_yaw = Current_Pos.yaw;
  
  /* Only in speed mode */
  if(Mode == Normal_Speed && Param.attitudeOptimize_flag == 1)
  {
    /* Yaw Axis stabilize */
//    dYaw = Target_Velocity.z_speed * co_wheel2chassis * co_rpm2w * Param.task_run_interval;
//    Command_Pos.yaw += dYaw;
    Command_Pos.yaw = locked_yaw;
    /* Pitch Axis stabilize */
    
    
    Attitude_Compensation = attitude_controller(&Current_Pos, &Command_Pos);
    Target_Velocity = Target_Velocity + (*Attitude_Compensation);
  }
}

/**
* @brief  Resolve the speed for chassis & calsulate output.(抬起时待优化)
* @param  None.
* @return None.
* @author Mentos Seetoo
*/
void CChassis::speed_control()
{
  static _ch_16t  TempBuff[WHEEL_NUM] = {0};
  static _ch_f32t scale = 0;
  
  /* Check Speed Controller */
  //assert(speed_controller != NULL);
  
  /*
    Resolution of velocity.
  */
  if(Mode == Halt)
  {
    Target_Velocity.x_speed = 0;
    Target_Velocity.y_speed = 0;
    Target_Velocity.z_speed = 0;
  }
  memset(wheel_rpmOut, 0, sizeof(_ch_16t)*WHEEL_NUM);
  X_Accum(wheel_rpmOut, (_ch_16t)Target_Velocity.x_speed);
  Y_Accum(wheel_rpmOut, (_ch_16t)Target_Velocity.y_speed);
  Z_Accum(wheel_rpmOut, (_ch_16t)Target_Velocity.z_speed);
  
  /*
    Constrain for wheel rpm.Any wheel output speed is larger than 
    the "wheel_max_speed", other wheel output will be constrain 
    proportionally here to keep the right direction.
  */
  for (_ch_U8t k = 0; k < WHEEL_NUM; ++k )
     TempBuff[k] = _Chassis_Abs(wheel_rpmOut[k]);
       
  _Chassis_BubbleSort(TempBuff,WHEEL_NUM);
  if(TempBuff[WHEEL_NUM - 1] >= Param.wheel_max_speed)
      scale = ((_ch_f32t)Param.wheel_max_speed)/((_ch_f32t)TempBuff[WHEEL_NUM - 1]);
  else
      scale = 1.0f;
  
  for (_ch_U8t k = 0; k < WHEEL_NUM; ++k )
     wheel_rpmOut[k] = (_ch_16t)((_ch_f32t)wheel_rpmOut[k] * scale);
  
  /*
    Calculate target output for motors by external controller.
  */
  memcpy(wheel_Out, speed_controller(wheel_rpm, wheel_rpmOut), sizeof(_ch_32t)*WHEEL_NUM);
  
  /*
    Constrain for wheel output to protect the motor.
  */
  for (_ch_U8t k = 0; k < WHEEL_NUM; ++k )
     wheel_Out[k] = _Chassis_Constrain(wheel_Out[k], -MAX_MOTOR_OUTPUT, MAX_MOTOR_OUTPUT);
}


/**
* @brief  Set the chassis gear ,pose target or speed coefficient 
          of three axis.
          (x_Speed = wheel_max_speed * x_co * gear_limit_Co)
* @param  target_X: 1.from -1 to 1 （Speed Mode）
                    2.no limit     (Position Mode)
*         targetGear: slow,normal,fast
* @return None.
* @author Mentos_Seetoo
*/
void CChassis::Set_Target(_ch_f32t target_X, _ch_f32t target_Y ,_ch_f32t target_Z)
{
  static _ch_f32t Co_limit = 0;
  
  if(current_gear == NORMAL_GEAR)    Co_limit = Param.coefficient_normal;
  else if(current_gear == FAST_GEAR) Co_limit = Param.coefficient_fast;
  else Co_limit = Param.coefficient_slow;
  
  /* Calculate velocity(Only in 'Normal-Speed') .*/
  if(Mode == Normal_Speed)
  {
    Command_Velocity.x_speed = _Chassis_Constrain(target_X,-1.0f,1.0f)*Co_limit*Param.wheel_max_speed;
    Command_Velocity.y_speed = _Chassis_Constrain(target_Y,-1.0f,1.0f)*Co_limit*Param.wheel_max_speed;
    Command_Velocity.z_speed = _Chassis_Constrain(target_Z,-1.0f,1.0f)*Param.coefficient_z*Param.wheel_max_speed;
  }
  
  /* Set command pose */
  else if((Mode == Normal_Position) || (Mode == Follow_Position))
  {
    Command_Pos.x   = target_X;
    Command_Pos.y   = target_Y;
    Command_Pos.yaw = target_Z;
  }
}

void CChassis::Set_SpeedGear(_chassis_SpeedGears targetGear)
{
  if(targetGear < 3)
    current_gear = targetGear;
}

void CChassis::Set_SpeedParam(_ch_f32t slow, _ch_f32t normal, _ch_f32t fast, _ch_f32t z)
{
  Param.coefficient_slow   = _Chassis_Constrain(slow, 0.0f, 1.0f);
  Param.coefficient_normal = _Chassis_Constrain(normal, 0.0f, 1.0f);
  Param.coefficient_fast   = _Chassis_Constrain(fast, 0.0f, 1.0f);
  Param.coefficient_z      = _Chassis_Constrain(z, 0.0f, 1.0f);
}

void CChassis::Set_AccelerationParam(_ch_16t launch, _ch_16t normal)
{
  Param.max_launch_acceleration = _Chassis_Abs(launch);
  Param.max_normal_acceleration = _Chassis_Abs(normal);
}

void CChassis::Set_TorqueOptimizeFlag(_ch_flag flag)
{
  Param.torqueOptimize_flag = flag;
}

void CChassis::Set_AttitudeOptimizeFlag(_ch_flag flag)
{
  Param.attitudeOptimize_flag = flag;
}

_ch_U8t CChassis::Switch_Mode(_chassis_Mode target_mode)
{
  /*
    Reserve for parameter check out.
  */
  Mode = target_mode;
  switch(target_mode)
  {
    case Normal_Speed:
      //First time enter to set the zero point.
      if(Mode != Normal_Speed){
        Command_Pos.yaw = Current_Pos.yaw;
      }
      break;
    default:
      break;
  }
  return 1;
}

_chassis_Mode CChassis::Get_Mode()
{
  return Mode;
}

/**
* @brief  Load the controllers
          Attention:pFunc can not return temporary address!
* @param  Current, Target
* @return _chassis_Velocity: calculate the velocity from position loop.
          _ch_32t : Output array for operator.
* @author Mentos Seetoo
*/
void CChassis::Load_AttitudeController(_chassis_Velocity*(*pFunc)(const _chassis_GlobalPos* Current, const _chassis_GlobalPos* Target))
{
  attitude_controller = pFunc;
}

void CChassis::Load_PositionController(_chassis_Velocity*(*pFunc)(const _chassis_GlobalPos* Current, const _chassis_GlobalPos* Target))
{
  position_controller = pFunc;
}

void CChassis::Load_SpeedController(_ch_32t*(*pFunc)(const _ch_16t* current, const _ch_16t* target))
{
  speed_controller = pFunc;
}

/**
* @brief  Update zero pose or current pose of chassis.
* @param  None.
* @return None.
* @author Mentos Seetoo
*/
void CChassis::Update_ZeroPose()
{
  Zero_Pos.x = Current_Pos.x;
  Zero_Pos.y = Current_Pos.y;
  Zero_Pos.roll = Current_Pos.roll;
  Zero_Pos.yaw  = Current_Pos.yaw;
  Zero_Pos.pitch= Current_Pos.pitch;
}

void CChassis::Update_CurrentPosition(_ch_f32t current_x, _ch_f32t current_y )
{
  Current_Pos.x = current_x;
  Current_Pos.y = current_y;
}

void CChassis::Update_CurrentAttitude(_ch_f32t roll, _ch_f32t pitch, _ch_f32t yaw)
{
  Current_Pos.roll  = roll;
  Current_Pos.pitch = pitch;
  Current_Pos.yaw   = yaw;
}



/**
 * @brief  Calculate weight of every axis.
* @param   wheels_speed*: wheel speed to output(Unit:rpm)
           Value: speed of every axis.(Unit:rpm)
 * @retval None.
 * @author YuyongHu
 */ 
static inline void  X_Accum(_ch_16t* wheels_speed, _ch_16t X_Value)
{
    wheels_speed[LF] += X_Value;
    wheels_speed[RF] -= X_Value;
    wheels_speed[RB] += X_Value;
    wheels_speed[LB] -= X_Value;
}
static inline void  Y_Accum(_ch_16t* wheels_speed, _ch_16t Y_Value)
{
    wheels_speed[LF] += Y_Value;
    wheels_speed[RF] += Y_Value;
    wheels_speed[RB] += Y_Value;
    wheels_speed[LB] += Y_Value;
#ifdef LUNAR_CHASSIS
    wheels_speed[LM] += Y_Value;
    wheels_speed[RM] += Y_Value;
#endif
}
static inline void  Z_Accum(_ch_16t* wheels_speed, _ch_16t Z_Value)
{
    wheels_speed[LF] -= Z_Value;
    wheels_speed[RF] += Z_Value;
    wheels_speed[RB] += Z_Value;
    wheels_speed[LB] -= Z_Value;
#ifdef LUNAR_CHASSIS
    static const float z_scale = CHASSIS_WHEEL_TRACK/(CHASSIS_WHEEL_TRACK + CHASSIS_WHEEL_BASE);
    wheels_speed[LM] -= z_scale * Z_Value;
    wheels_speed[RM] += z_scale * Z_Value;
#endif
}


/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
