/**
  ********************************************************************************
  * @file   : Engineer.cpp
  * @brief  : Engineer(Robomaster 2019 version 4.0) robot lib.This file provides 
              APIs to control the whole robot to move and take actions.
  * @author : Mentos_Seetoo
  * @date   : July,2019
  * @version: 2.0
  ********************************************************************************
  ================================================================================
                        ##### How to use this lib #####
  ================================================================================
*/
/* Includes ------------------------------------------------------------------*/ 
#include "Engineer.h"
#include "controller.h"
/* Driver Support */
  
/* Functions -----------------------------------------------------------------*/
/**
* @brief  Init the controllers, and do what?????
* @param  
* @return 
*/
void CEngineer::Init()
{
  SelfCheck_Status();
  /* Controller Init */
  Controller_Init();
  Lunar_Chassis.Load_PositionController(Controller_ChassisPositionLoop);
  Lunar_Chassis.Load_SpeedController(Controller_ChassisSpeedLoop);
  Lunar_Chassis.Load_AttitudeController(Controller_ChassisAttitudeLoop);
  
  /* Motion Paramter Init */
  up_stairs_co1   = UP_STAIRS_MOVE_SPEED;
  up_stairs_co2   = UP_STAIRS_UP_SPEED;
  up_stairs_pitch = UP_STAIRS_STOP_PITCH;
  align_Y_co      = ALIGN_Y_SPEED;
  threshold_time  = THRESHOLD_TIME;
  
  Mode = NORMAL;
}

_ActionMode_t CEngineer::Get_CurrentMode()
{
  return Mode;
}

/**
* @brief  需要修改的参数一般有:
*         1.底盘模式:位置/速度模式
*         2.速度档位:高中低
*         3.指令源(自动/手动指令)
*         4.视角:深度/模组/图传
*         5.启动力矩优化/陀螺仪优化/倾角保护
* @param  None.
* @return robot state
*/
_RobotStatus_t CEngineer::Central_Control()
{
  //Self check.
  SelfCheck_Status();
  
  //Select action.
  Lunar_Chassis.Update_CurrentAttitude(IMU_data.roll, IMU_data.pitch, IMU_data.yaw);
  switch(Mode)
  {
    case CHASSIS_LOCK:
      /*
         紧急停止
      */
      Lunar_Chassis.Switch_Mode(Halt);
      Lunar_Chassis.Set_AttitudeOptimizeFlag(OFF);
      break;
    
    case NORMAL:
      /*
        1.速度模式
        2.默认中档
        3.手动指令
        4.图传视角
        5.全优化开
      */
      action_Battle();
      break;

    case GET_BULLET:
      /*
        1.自动/手动混合控制
        2.默认抵挡
        3.深度视角
        4.倾角保护
      */
      action_Get_Bullet();
      break;
    
    case UP_STAIRS:
      /*
        1.自动/手动混合控制，速度模式
        2.默认抵挡
        3.模组视角
        4.倾角保护
      */
      action_Upstairs();
      break;
    
    case RESCUE:
      /*
        1.自动/手动混合控制，速度模式
        2.图传切模组
        3.救援后切入快速档
        4.力矩优化
      */
      action_Rescue();
      break;
    
    case AUTO_ROTATE:
      /*
        自动旋转180°/自动旋转∞°
      */
      action_auto_rotate();
      break;
    
    case GIVE_BULLET_NORMAL:
      action_Give_Bullet();
      break;
  }
  
  //Speed control for manual input.
  GearBox();
  
  //Chassis control & clear the command.
  Lunar_Chassis.Chassis_Control();

  return State;
}


/**
* @brief  Switch the mode and set the default parameter.
* @param  _ActionMode_t: targetMode
* @return 0:Target mode is current mode.
*         1:Current mode has been changed.
* @author Mentos_Seetoo
*/
uint8_t CEngineer::Switch_ActionMode(_ActionMode_t targetMode)
{
  /* Switching by current mode */
  switch(Mode)
  {
    /* 
      取弹模式
    */
    case GET_BULLET:
      switch(targetMode)
      {
        case NORMAL:
          Lunar_Chassis.Set_SpeedGear(NORMAL_GEAR);
          Lunar_Chassis.Set_TorqueOptimizeFlag(ON);
          view_flag = FORWARD;
          Mode = NORMAL;
          break;
        default:
          //除了复位以外不切换其他状态，取弹状态优先级最高，防止误触
          break;
      }
      break;
      
    /* 
       自动旋转模式
    */
    case AUTO_ROTATE:
      cat_walk_flag = 0;
      switch(targetMode)
      {
        case NORMAL:
          Mode = NORMAL;
          //清除旧指令
          Manual_Target.x = 0;
          Manual_Target.y = 0;
          Manual_Target.z = 0;
          Lunar_Chassis.Set_SpeedGear(NORMAL_GEAR);
          Lunar_Chassis.Set_TorqueOptimizeFlag(ON);
        break;
        
        case GET_BULLET:
          Mode = GET_BULLET;
          //清除旧指令
          Manual_Target.x = 0;
          Manual_Target.y = 0;
          Manual_Target.z = 0;
          Lunar_Chassis.Set_SpeedGear(SLOW_GEAR);
          Lunar_Chassis.Set_TorqueOptimizeFlag(OFF);
          view_flag       = BACK_BULLET;
          Operations.get_bullet_flag = APPROACH;
          Operations.detected_flag   = 0;
        break;
        
        case AUTO_ROTATE:
          Mode = AUTO_ROTATE;
          //清除旧指令
          Manual_Target.x = 0;
          Manual_Target.y = 0;
          Manual_Target.z = 0;
          if(Operations.auto_rotate_flag == CONTINUOUS)
            Lunar_Chassis.Update_ZeroPose();
        break;
        
        default:
          Mode = targetMode;
          //自动旋转状态可切换任意状态
        break;
      }
    break;
    
    /* 
      上岛模式 
    */
    case UP_STAIRS:
      switch(targetMode)
      {
        case NORMAL:
          Mode = NORMAL;
          Manual_Target.x = 0;
          Manual_Target.y = 0;
          Manual_Target.z = 0;
          Lunar_Chassis.Set_SpeedGear(NORMAL_GEAR);
          Lunar_Chassis.Set_TorqueOptimizeFlag(ON);
          view_flag = FORWARD;
        break;
        
        case GET_BULLET:
          Mode = GET_BULLET;
          Lunar_Chassis.Set_SpeedGear(SLOW_GEAR);
          Lunar_Chassis.Set_TorqueOptimizeFlag(OFF);
          view_flag       = BACK_BULLET; 
          Operations.get_bullet_flag = APPROACH;
          Operations.detected_flag   = 0;
          Operations.upstairs_flag   = MANUAL_UPSTAIRS;
        break;
        
        case UP_STAIRS:
          //手动模式下切回自动
          if(Operations.upstairs_flag == MANUAL_UPSTAIRS)
          {
            Mode = UP_STAIRS;
            Lunar_Chassis.Set_SpeedGear(SLOW_GEAR);
            Lunar_Chassis.Set_TorqueOptimizeFlag(OFF);
            view_flag     = BACK_UPSTAIRS;
            Operations.upstairs_flag = STAGE1_LIFT;
          }
        break;
        default:
          //上岛状态下只能复位
          break;
      }
    break;
      
    /* 
      其他状态下默认切换 
    */
    default:
      switch(targetMode)
      {
        case CHASSIS_LOCK:
          Mode = NORMAL;
          break;
        case NORMAL:
          Mode = NORMAL;
          Lunar_Chassis.Set_SpeedGear(NORMAL_GEAR);
          Lunar_Chassis.Set_TorqueOptimizeFlag(ON);
          Operations.upstairs_flag = MANUAL_UPSTAIRS;
          view_flag = FORWARD;
        break;
        
        case GET_BULLET:
          Mode = GET_BULLET;
          Lunar_Chassis.Set_SpeedGear(SLOW_GEAR);
          Lunar_Chassis.Set_TorqueOptimizeFlag(OFF);
          view_flag       = BACK_BULLET;
          Operations.get_bullet_flag = APPROACH;
          Operations.detected_flag   = 0;
        break;
        
        case GIVE_BULLET_NORMAL:
          //切换到慢速档并且降低加速度
          Mode = GIVE_BULLET_NORMAL;
          Lunar_Chassis.Set_SpeedGear(SLOW_GEAR);
          Lunar_Chassis.Set_TorqueOptimizeFlag(OFF);
          break;
        
        case UP_STAIRS:
          Mode = UP_STAIRS;
          Lunar_Chassis.Set_SpeedGear(SLOW_GEAR);
          Lunar_Chassis.Set_TorqueOptimizeFlag(OFF);
          view_flag     = BACK_UPSTAIRS;
          Operations.upstairs_flag = STAGE1_LIFT;
          break;
        
        case RESCUE:
          if(Mode != RESCUE)
          {
            Mode = RESCUE;
            Lunar_Chassis.Set_SpeedGear(NORMAL_GEAR);
            Lunar_Chassis.Set_TorqueOptimizeFlag(ON);
            view_flag   = FORWARD;
            Operations.rescue_flag = ATTACHING;
          }
          break;
        
        case AUTO_ROTATE:
          Mode = AUTO_ROTATE;
          Lunar_Chassis.Update_ZeroPose();
          if(Operations.auto_rotate_flag == CAT_WALK && cat_walk_flag == 0)
            cat_walk_flag = CAT_LEFT;
          break;
      }
      Lunar_Chassis.Switch_Mode(Normal_Speed);
      return 1;
  }
  return 0;
}

/**
* @brief  Speed up or Slow down under specific mode.
* @param  FAST_GEAR,NORMAL_GEAR,SLOW_GEAR
* @return None.
* @author 
*/
void CEngineer::GearBox(void)
{
  static bool gear_box_enable = 0;
  static float Transmit_Co = 0;
  
  /* Gear Box is only for manual input */  
  switch(Mode)
  {
    case NORMAL:
      gear_box_enable = 1;
      if(view_flag == FORWARD)
        Transmit_Co = 1.0f;
      else
        Transmit_Co = -1.0f;
      break;
    
    case RESCUE:
      gear_box_enable = 1;
      if(view_flag == FORWARD)
        Transmit_Co = 1.0f;
      else
        Transmit_Co = -1.0f;
      break;
    
    case UP_STAIRS:
      if(Operations.upstairs_flag == MANUAL_UPSTAIRS)
      {
        gear_box_enable = 1;
        if(view_flag == FORWARD)
          Transmit_Co = 1.0f;
        else
          Transmit_Co = -1.0f;
      }
      else
        gear_box_enable = 0;
      break;
    default:
      gear_box_enable = 0;
      break;
  }
  
  if(gear_box_enable)
  {
    if(gear_flag == SPEED_UP)
      Lunar_Chassis.Set_SpeedGear(FAST_GEAR);
    else if(gear_flag == SLOW_DOWN)
      Lunar_Chassis.Set_SpeedGear(SLOW_GEAR);
    else 
      Lunar_Chassis.Set_SpeedGear(NORMAL_GEAR);
    
    Lunar_Chassis.Set_Target(Transmit_Co*Manual_Target.x, Transmit_Co*Manual_Target.y, Manual_Target.z);
  }
  
}

void CEngineer::Update_NowTime(uint32_t time)
{
  nowtime = time;
}

void CEngineer::controller_set()
{
//  if(Lunar_Chassis.Get_Mode() != Follow_Position && Pose_Yaw.PS_kp != -250.0f)
//  {
//    Pose_Yaw.Set_FUZZYPID(//一个单位0.01度
//    0.0f,0.0f,0.0f,0.0f,//P,I,D,Target
//    -10000.0f, +10000.0f, -10000.0f, +10000.0f, -10000.0f, +10000.0f,	//PTermMin,PTermMax,ITermMin,ITermMax,DTermMin,DTermMax
//    -3000.0f,+3000.0f,0,//N_Target_MIN,P_Target_MAX,DeadZone
//    -60,60,-40,40,-10,10,//NB 负大的量度
//    -150.0f,-150.0f, -250.0f,-250.0f, -250.0f,-250.0f,
//    0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f,
//    0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f);
//  }
//  else
//  {
//    Pose_Yaw.Set_FUZZYPID(//一个单位0.01度
//    0.0f,0.0f,0.0f,0.0f,//P,I,D,Target
//    -10000.0f, +10000.0f, -10000.0f, +10000.0f, -10000.0f, +10000.0f,	//PTermMin,PTermMax,ITermMin,ITermMax,DTermMin,DTermMax
//    -3000.0f,+3000.0f,0,//N_Target_MIN,P_Target_MAX,DeadZone
//    -40,40,-20,20,-5,5,//NB 负大的量度
//    -25.0f,-25.0f, -50.0f,-50.0f, -70.0f,-70.0f,
//    0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f,
//    0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f);
//  }
  
}


/**
* @brief  Command robot to move in current perspective. 
* @param  target x,y,z
* @return None.
*/
void CEngineer::Manual_XYZ(float x_co, float y_co, float z_co)
{
  Manual_Target.x = x_co;
  Manual_Target.y = y_co;
  Manual_Target.z = z_co;
}

void CEngineer::Follow_XYZ(float x_target, float y_target, float z_target)
{
  Auto_Target.x = x_target;
  Auto_Target.y = y_target;
  Auto_Target.z = z_target;
}

/**
* @brief  Set the auto rotate parameters.
* @param  direction : SINGLE/CONTINUOUS
* @return None.
* @author Mentos Seetoo
*/
void CEngineer::Set_AutoRotate(_Auto_Rotate_t direction)
{
  if(direction != Operations.auto_rotate_flag)
    Lunar_Chassis.Update_ZeroPose();

  Operations.auto_rotate_flag = direction;
}


/**
* @brief Check the status of all Units and record the fault unit.
*/
void CEngineer::SelfCheck_Status()
{
  char index = 0;
  
  /* 
    Check body motors ,if there any one is damaged, set 'BODY_MOTOR_DAMAGE'
  */
  while(index < BODY_MOTORS_NUM - 1)
  {
    if((Unit_Status.body_motors >> index) != RM_ONLINE)
    {
      fault_index |= BODY_MOTOR_DAMAGE;
      fault_extMsg = index;
      break;
    }
    index ++;
  }
  
  /* Check chassis motors */
  index = 0;
  while(index < WHEEL_NUM - 1)
  {
    if((Unit_Status.body_motors >> index) != RM_ONLINE)
    {
      fault_index |= CHASSIS_MOTOR_DAMAGE;
      fault_extMsg = index;
      break;
    }
  }
  
  /* Check body_photoelectrics */
  index = 0;
  while(index < BODY_PHOTOELE_NUM - 1)
  {
    if((Unit_Status.body_photoelectrics >> index) != RM_ONLINE)
    {
      fault_index |= BODY_LOST;
      fault_extMsg = index;
      break;
    }
  }
  
  /* Check IMU */
  if(Unit_Status.IMU != RM_ONLINE)
    fault_index |= IMU_LOST;
  
  /* Check NUC */
  if(Unit_Status.NUC != RM_ONLINE)
    fault_index |= NUC_LOST;
} 

/**
* @brief  I/O Stream of the robot.
          These functions is depended on the hardware of robot.
          If your robot is different, just rewrite I/O functions.
* @param  encoder_count,pose,Current(or Torque),Status,Power State
* @return int32_t
*/
void CEngineer::IO_Port(_Unit_Type UnitToOperate, _IO_Type mode, int32_t value)
{
  switch(mode)
  {
    /* Read in current wheel rpm */
    case VELOCITY_IN:
      if(UnitToOperate < 6){
        if((UnitToOperate == WHEEL_LF) || (UnitToOperate == WHEEL_LB) || (UnitToOperate == WHEEL_LM))
          Lunar_Chassis.wheel_rpm[UnitToOperate] = -value;
        else
          Lunar_Chassis.wheel_rpm[UnitToOperate] = value;
      }
      break;
    
    /* Read in current torque(Current) */
    case TORQUE_IN:
      if(UnitToOperate < 6)
        Lunar_Chassis.wheel_torque[UnitToOperate] = value;
      break;
    case CONNECTION_STATE_IN:
      switch(UnitToOperate)
      {
       case BODY_MOTORS:
         Unit_Status.body_motors = value;
         break;
       case IMU:
         Unit_Status.IMU = value;
       case NUC:
         Unit_Status.NUC = value;
       default:
        if(UnitToOperate < 6)
          Unit_Status.chassis_motors |= value >> UnitToOperate;
        break;
      }
      break;
    
    /* Wrong call */
    default:
      break;
  }
}

void CEngineer::IO_Port(_Unit_Type UnitToOperate, _IO_Type mode, uint8_t value)
{
  switch(mode)
  {
    case CHASSIS_TEMPRE_IN:
      if(UnitToOperate < 6)
        Lunar_Chassis.Temperature[UnitToOperate] = value;
    break;
      
    case CHASSIS_MAGNET_IN:
      if(UnitToOperate == CHASSIS_MAGNET)
         Unit_Status.chassis_Magnet_Lift = value;
    break;
      
    case CHASSIS_PES_IN:
      if(UnitToOperate == CHASSIS_PES_PULL)
        Unit_Status.chassis_PES_Pull = value;
      else if(UnitToOperate == CHASSIS_PES_STOP)
        Unit_Status.chassis_PES_Stop = value;
    break;
    
    default:
      break;
  }
}

int32_t CEngineer::IO_Port(_Unit_Type UnitToOperate, _IO_Type mode)
{
  switch(mode)
  {
    case VELOCITY_OUT:
      if((UnitToOperate == WHEEL_LF) || (UnitToOperate == WHEEL_LB) || (UnitToOperate == WHEEL_LM))
        return (-Lunar_Chassis.wheel_Out[UnitToOperate]);
      else
        return Lunar_Chassis.wheel_Out[UnitToOperate];
    
    /* Wrong call */
    default:
      break;
  }
}

void CEngineer::IO_Port(_IO_Type mode,float x,float y ,float z)
{
  if(mode == POSITION_IN)
  {
    CV_data.x = x;
    CV_data.y = y;
    CV_data.z = z;
  }
  else if(mode == IMU_IN)
  {
    IMU_data.roll = x;
    IMU_data.pitch = y;
    IMU_data.yaw = z;
  }
}

_Body_Operation_t* CEngineer::IO_Port(_IO_Type mode)
{
  if(mode == BODY_OPERATION_OUT)
    return &Operations;
  else 
    return NULL;
}


/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
