/* Includes ------------------------------------------------------------------*/ 
#include "Engineer.h"
#include "controller.h"
/* Driver Support */


/* Functions -----------------------------------------------------------------*/
#ifdef __cplusplus
/**
* @brief Actions that need to do in different mode.
*/
void CEngineer::action_Battle()
{
  Lunar_Chassis.Switch_Mode(Normal_Speed);
  Lunar_Chassis.Set_TorqueOptimizeFlag(ON);
  Lunar_Chassis.Set_AttitudeOptimizeFlag(ON);
}

void CEngineer::action_Get_Bullet()
{
  Lunar_Chassis.Set_AttitudeOptimizeFlag(ON);
  Controller_SetMaxAdjustSpeed(4000);
  /* 
    取弹状态机
  */
  switch(Operations.get_bullet_flag)
  {
    //进入对准，可能未进入视觉控制范围
    case APPROACH:
      //Y轴距离1200内视觉自动切换,手动输入撤销后进入对准模式
      if(Operations.detected_flag == 1 && _Chassis_Abs(CV_data.y) < 1200)
      {
        if(Manual_Target.x == 0 && Manual_Target.y == 0)
          Operations.get_bullet_flag = ALIGNING_FIRST;
        else
        {
          Lunar_Chassis.Switch_Mode(Follow_Position);
          Lunar_Chassis.Set_TorqueOptimizeFlag(OFF);
          Lunar_Chassis.Set_Target(CV_data.x, CV_data.y, -CV_data.z);
        }
      }
      else
      {
        Lunar_Chassis.Switch_Mode(Normal_Speed);
        Lunar_Chassis.Set_SpeedGear(NORMAL_GEAR);
        Lunar_Chassis.Set_Target(-Manual_Target.x, -0.8f*Manual_Target.y, Manual_Target.z);
        Lunar_Chassis.Set_TorqueOptimizeFlag(ON);
      }
    break;
    
    case MANUAL:
      Lunar_Chassis.Switch_Mode(Normal_Speed);
      Lunar_Chassis.Set_SpeedGear(SLOW_GEAR);
      Lunar_Chassis.Set_Target(-Manual_Target.x, - 0.5f*Manual_Target.y, Manual_Target.z);
      Lunar_Chassis.Set_TorqueOptimizeFlag(OFF);
      break;
    
    case ALIGNING_FIRST:
      Lunar_Chassis.Switch_Mode(Follow_Position);
      Lunar_Chassis.Set_TorqueOptimizeFlag(OFF);
      Lunar_Chassis.Set_Target(CV_data.x, CV_data.y, -CV_data.z);
      //手动可打断
      if(Manual_Target.x != 0 || Manual_Target.y != 0)
        Operations.get_bullet_flag = MANUAL;
      break;
      
    case ALIGNING_AGAIN:
      Lunar_Chassis.Switch_Mode(Follow_Position);
      Lunar_Chassis.Set_TorqueOptimizeFlag(OFF);
      Lunar_Chassis.Set_Target(CV_data.x, CV_data.y, -CV_data.z);
      //手动可打断
      if(Manual_Target.x != 0 || Manual_Target.y != 0)
        Operations.get_bullet_flag = MANUAL;
      break;
      
    default:
      break;
  }
  
  //全程深度视角
  view_flag = BACK_BULLET;
}

void CEngineer::action_Give_Bullet()
{
  Lunar_Chassis.Set_SpeedGear(SLOW_GEAR);
  Lunar_Chassis.Set_TorqueOptimizeFlag(OFF);
  Lunar_Chassis.Set_Target(0.5f*Manual_Target.x, 0.5f*Manual_Target.y, Manual_Target.z);
}

void CEngineer::action_Upstairs()
{
  static uint32_t last_time = 0, stop_PES_activeFlag = 0;
  //自动模式下中速档为基速,关闭输入优化
  if(Operations.upstairs_flag != MANUAL_UPSTAIRS){
    Lunar_Chassis.Set_TorqueOptimizeFlag(OFF);
    Lunar_Chassis.Set_SpeedGear(NORMAL_GEAR);
    //模组视角
    view_flag = BACK_UPSTAIRS;
  }
  else
  {
    Lunar_Chassis.Set_TorqueOptimizeFlag(OFF);
  }
  
  /* 
    登岛状态机 
  */
  switch(Operations.upstairs_flag)
  {
    case MANUAL_UPSTAIRS:
      //气缸放气,后轮开环
      Lunar_Chassis.Set_Target(-0 , - Manual_Target.y, Manual_Target.z);
    break;
    
    case STAGE1_LIFT:
      //执行操作，气缸打开，完全抬起后切换状态
      stop_PES_activeFlag = 0;
      if(Unit_Status.chassis_Magnet_Lift == 1 && Unit_Status.chassis_PES_Pull == 0)
        Operations.upstairs_flag = STAGE1_LAY_DOWN;

      Lunar_Chassis.Set_Target(-0, -0.9f, Manual_Target.z);
    break;
    
    case STAGE1_LAY_DOWN:
      //光电有电平转换，执行操作，收腿
      if(Unit_Status.chassis_PES_Pull == 1)
      {
        Operations.upstairs_flag = TRANSITION;
        last_time = nowtime;
      }
      
      Lunar_Chassis.Set_Target(-0, -up_stairs_co1, Manual_Target.z);
    break;
      
    case TRANSITION:
//      //光电被激活后再次被触发时认为已经上完台阶
//      if(Unit_Status.chassis_PES_Stop == 0)
//        stop_PES_activeFlag = 1;
//      else
//        __nop();
//      if(stop_PES_activeFlag == 1 && Unit_Status.chassis_PES_Stop == 1)
//      {
//        Operations.upstairs_flag = LAND_ON;
//        Lunar_Chassis.Set_Target(-Manual_Target.x, -Manual_Target.y, Manual_Target.z);
//      }
      //小于5°倾角时认为已经上完台阶
      if(IMU_data.pitch < up_stairs_pitch)
      {
        Operations.upstairs_flag = LAND_ON;
        Lunar_Chassis.Set_Target(-0, -up_stairs_co1, Manual_Target.z);
      }
      else 
      { //收起气缸一段时间后加速
        if(nowtime - last_time >= threshold_time)
          Lunar_Chassis.Set_Target(-0, -up_stairs_co2, Manual_Target.z);
        else
          Lunar_Chassis.Set_Target(-0, -up_stairs_co1, Manual_Target.z);
      }
    break;
      
    case LAND_ON:
        if(up_stairs_time == TWICE)
        { //二阶模式上完第一阶后清空标志
          Operations.upstairs_flag = STAGE2_LIFT;
          up_stairs_time = ONEC;
        }
        else if(up_stairs_time == ONEC)
        {
          //第一阶完成后继续往前
          //在这里等待，如果有人手信号则切换状态，外部可由按键切换下一个状态
          Lunar_Chassis.Set_TorqueOptimizeFlag(ON);
          Lunar_Chassis.Set_Target(-0, -up_stairs_co1, Manual_Target.z);
        }
    break;
    
    case STAGE2_LIFT:
      stop_PES_activeFlag = 0;
      //执行操作，气缸打开，完全抬起后切换状态
      if(Unit_Status.chassis_Magnet_Lift == 1 && Unit_Status.chassis_PES_Pull == 0)
        Operations.upstairs_flag = STAGE2_LAY_DOWN;

      Lunar_Chassis.Set_Target(-0, -0.9f, Manual_Target.z);
    break;
    
    case STAGE2_LAY_DOWN:
      //光电有电平转换转换，执行操作，收腿
      if(Unit_Status.chassis_PES_Pull == 1)
      {
        Operations.upstairs_flag = TRANSITION;
        last_time = nowtime;
      }

      Lunar_Chassis.Set_Target(-0, -up_stairs_co1, Manual_Target.z);
    break;
  }
  
  if(Manual_Target.y != 0)
    Switch_ActionMode(NORMAL);
}

void CEngineer::action_Rescue()
{ 
  /* 
    救援状态机
  */
  if(Operations.rescue_flag == ATTACHING)
  //打开救援机构
    view_flag = FORWARD;
  //对接成功后切模组相机
  else if(Operations.rescue_flag == PULLING_OUT)
    view_flag = BACK_UPSTAIRS;
  
  Lunar_Chassis.Set_TorqueOptimizeFlag(ON);
}

void CEngineer::action_auto_rotate()
{
  //猫步速度
  float cat_walk_speed = 0.2f;
    
  Lunar_Chassis.Set_AttitudeOptimizeFlag(OFF);
  Lunar_Chassis.Set_TorqueOptimizeFlag(OFF);
  Lunar_Chassis.Set_SpeedGear(FAST_GEAR);
  Controller_SetMaxAdjustSpeed(cat_walk_speed*WHEEL_MAX_SPEED);
  
  //单次旋转
  if(Operations.auto_rotate_flag == SINGLE)
  {
    Lunar_Chassis.Switch_Mode(Normal_Speed);
    Lunar_Chassis.Set_Target(0, 0, -1);
    if(Lunar_Chassis.Current_Pos.yaw - Lunar_Chassis.Zero_Pos.yaw > 144)
    {
      if(_Chassis_Abs(Lunar_Chassis.Current_Pos.yaw - Lunar_Chassis.Zero_Pos.yaw) > 180){
        Lunar_Chassis.Update_ZeroPose();
      }
      Switch_ActionMode(NORMAL);
      Lunar_Chassis.Set_Target(0, 0, 0);
    }
  }
  //连续旋转
  else if(Operations.auto_rotate_flag == CONTINUOUS)
  {
    Lunar_Chassis.Switch_Mode(Normal_Speed);
    Lunar_Chassis.Set_Target(0, 0, 1);
  }
  //猫步
  else if(Operations.auto_rotate_flag == CAT_WALK)
  {
    Lunar_Chassis.Switch_Mode(Normal_Speed);
    if(cat_walk_flag == 1)
    {
      Lunar_Chassis.Set_Target(0, 0, -cat_walk_speed);
      if(Lunar_Chassis.Current_Pos.yaw - Lunar_Chassis.Zero_Pos.yaw > 25)
      {
        if(_Chassis_Abs(Lunar_Chassis.Current_Pos.yaw - Lunar_Chassis.Zero_Pos.yaw) > 180){
          Lunar_Chassis.Update_ZeroPose();
        }
        cat_walk_flag = 2;
      }
    }
    else if(cat_walk_flag == 2)
    {
      Lunar_Chassis.Set_Target(0, 0, cat_walk_speed);
      if(Lunar_Chassis.Current_Pos.yaw - Lunar_Chassis.Zero_Pos.yaw < -25)
      {
        if(_Chassis_Abs(Lunar_Chassis.Current_Pos.yaw - Lunar_Chassis.Zero_Pos.yaw) > 180){
          Lunar_Chassis.Update_ZeroPose();
        }
        cat_walk_flag = 1;
      }
    }
    else
    {
      Lunar_Chassis.Set_Target(0, 0, 0);
    }
  }
  
  //手动输入可中途退出
  if(Manual_Target.x != 0 || Manual_Target.y != 0 || Manual_Target.z != 0)
  {
    Lunar_Chassis.Set_Target(0, 0, 0);
    Switch_ActionMode(NORMAL);
  }
}

#endif

/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/


