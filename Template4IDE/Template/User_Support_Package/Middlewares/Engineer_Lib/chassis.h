/**
  ******************************************************************************
 * Copyright (c) 2019 - ~, RobotLab Development Team
  * @file   ：Chassis.h
  * @brief  ：Header of Chassis.cpp
  * @version：0.0.1
  * @changes：
 *  Date           Author   Version    Notes
 *  2019-06-22     Mentos    0.0.1     first version
  ******************************************************************************
  */
#ifndef _CHASSIS_H
#define _CHASSIS_H

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <assert.h>

/* Private  ------------------------------------------------------------------*/
#define LUNAR_CHASSIS           1
#define CHASSIS_FAILED          0
#define CHASSIS_FINISHED        1
#define CHASSIS_OPERATING       2
#define OFF                     0
#define ON                      1

#ifdef __cplusplus
#ifdef LUNAR_CHASSIS
  #define WHEEL_NUM             6
#else
  #define WHEEL_NUM             4
#endif

typedef  float          _ch_f32t;
typedef  int            _ch_32t;
typedef  unsigned int   _ch_U32t;
typedef  short int      _ch_16t;
typedef  unsigned short _ch_U16t;
typedef  unsigned char  _ch_U8t;
typedef  bool           _ch_flag;


#ifdef LUNAR_CHASSIS
  enum _chassis_WheelType
  {
    LF = 0U,
    RF,
    RB,
    LB,
    LM,
    RM
  };
  
  enum _chassis_Mode
  {
    Normal_Speed = 0U,
    Normal_Position,
    Follow_Position,
    Halt,
    Transform
  };
#else
  enum _chassis_Mode
  {
    Normal_Speed = 0U,
    Normal_Position,
    Follow_Position,
    Halt
  };
  enum _chassis_WheelType
  {
    LF = 0U,
    RF,
    RB,
    LB
  };
#endif
  
enum _chassis_SpeedGears
{
  SLOW_GEAR = 0U,
  NORMAL_GEAR,
  FAST_GEAR
};

typedef struct _chassis_GlobalPos
{
  _ch_f32t x;
  _ch_f32t y;
  _ch_f32t roll;
  _ch_f32t pitch;
  _ch_f32t yaw;

  _chassis_GlobalPos& operator = (_chassis_GlobalPos& value)
  {
    x = value.x ; y = value.y; roll = value.roll; pitch = value.pitch; yaw = value.yaw;
    return *this;
  }
  
  _chassis_GlobalPos& operator - (_chassis_GlobalPos& value){
    x -= value.x;
    y -= value.y;
    roll -= value.roll ;
    pitch -= value.pitch;
    yaw -= value.yaw;
    return *this;
  }
}_chassis_GlobalPos;

typedef struct _chassis_Velocity
{
  _ch_f32t x_speed;
  _ch_f32t y_speed;
  _ch_f32t z_speed;
  
  _chassis_Velocity& operator = (_chassis_Velocity& value){
    x_speed = value.x_speed; y_speed = value.y_speed; z_speed = value.z_speed;
    return *this;
  }
  
  _chassis_Velocity& operator + (_chassis_Velocity& value){
    x_speed += value.x_speed;
    y_speed += value.y_speed;
    z_speed += value.z_speed;
    return *this;
  }
}_chassis_Velocity;

typedef struct 
{
  _ch_flag  torqueOptimize_flag;
  _ch_flag  attitudeOptimize_flag;
  _ch_16t   wheel_max_speed;
  
  /*Time interval for speed resolve.*/
  _ch_f32t  task_run_interval;
  
  /*Acceleration for different stages(Unit: rpm/s^2)*/
  _ch_32t   max_launch_acceleration;
  _ch_32t   max_normal_acceleration;
  _ch_32t   max_brake_acceleration;
  _ch_16t   launch_speed;
  
  /*Coefficients for different gears(Max_gear_speed = Coefficients*wheel_max_speed).(Between 0 to 1)*/
  _ch_f32t  coefficient_slow;
  _ch_f32t  coefficient_normal;
  _ch_f32t  coefficient_fast;
  _ch_f32t  coefficient_z;
}_chassis_Parameter;


template<typename Type>
Type _Chassis_Constrain(Type input,Type min,Type max){
  if (input <= min)
    return min;
  else if(input >= max)
    return max;
  else return input;
}

template<typename Type>
void _Chassis_BubbleSort(Type a[], int n)
{
  int i,j;
  Type temp;
  for (j = 0; j < n - 1; j++){
    for (i = 0; i < n - 1 - j; i++){
      if(a[i] > a[i + 1]){
          temp = a[i];
          a[i] = a[i + 1];
          a[i + 1] = temp;
      }
    }
  }
}

template<typename Type> 
Type _Chassis_Abs(Type x) {return ((x > 0) ? x : -x);}

static inline void  X_Accum(_ch_16t* wheels_speed, _ch_16t X_Value);
static inline void  Y_Accum(_ch_16t* wheels_speed, _ch_16t Y_Value);
static inline void  Z_Accum(_ch_16t* wheels_speed, _ch_16t Z_Value);
/* Exported ------------------------------------------------------------------*/
/*
   User Specific Parameters
*/
#define CHASSIS_WHEEL_TRACK        (535.0f)    // Unit: mm
#define CHASSIS_WHEEL_BASE         (620.0f)    // Unit: mm
#define WHEEL_RADIUS               (76.0f)     // Unit: mm
#define MAX_MOTOR_OUTPUT           (16000)     // Unit: None
#define WHEEL_MAX_SPEED            (9500)      // Unit: rpm
#define RUN_INTERVAL               (0.001f)    // Unit: s


/*
   User Interface
*/
class CChassis
{
  private:
    _chassis_Mode       Mode;
    _chassis_Parameter  Param;
    _chassis_SpeedGears current_gear;
  
    void position_control();
    void speed_control();
    void torque_optimization();
    void attitude_control();
    _chassis_Velocity*(*attitude_controller)(const _chassis_GlobalPos* Current, const _chassis_GlobalPos* Target);
    _chassis_Velocity*(*position_controller)(const _chassis_GlobalPos* Current, const _chassis_GlobalPos* Target);
    _ch_32t*(*speed_controller)(const _ch_16t* current, const _ch_16t* target);
  
  public://(为方便调试，暂时开放访问)
    _ch_U8t              Temperature[WHEEL_NUM];   //轮子驱动单元温度
    _chassis_GlobalPos   Zero_Pos;                 //零位姿点
    _chassis_GlobalPos   Command_Pos;              //指令位姿
    _chassis_Velocity    Command_Velocity;         //指令速度矢量
    _chassis_Velocity    Target_Velocity;          //优化后输出目标速度矢量
    _ch_16t              wheel_rpmOut[WHEEL_NUM];  //轮子解算后转速

    _chassis_GlobalPos   Current_Pos;              //当前位姿
    _ch_16t              wheel_rpm[WHEEL_NUM];     //轮子转速
    _ch_16t              wheel_torque[WHEEL_NUM];  //轮子转矩（电流）
    _ch_32t              wheel_Out[WHEEL_NUM];     //控制器作用后输出
    /* 设置初始参数 */
    CChassis(_ch_flag optimize_flag = 1, _ch_f32t run_interval = RUN_INTERVAL, _ch_16t wheel_max_speed = WHEEL_MAX_SPEED){
      Param.torqueOptimize_flag   = optimize_flag;
      Param.attitudeOptimize_flag = 0;
      Param.task_run_interval = run_interval;
      Param.wheel_max_speed   = wheel_max_speed;
      Param.max_launch_acceleration = 9000;
      Param.max_normal_acceleration = 28000;
      Param.max_brake_acceleration  = 50000;
      Param.launch_speed      = 0.32f*WHEEL_MAX_SPEED;
      Param.coefficient_slow  = 0.25;
      Param.coefficient_normal= 0.6;
      Param.coefficient_fast  = 1.0;
      Param.coefficient_z     = 0.8f;
    }
    /*
      设置目标值：
      1.速度模式 --> 设置三轴运动速度比例系数(X,Y,Rotate)
      2.位置模式 --> 设置目标位姿（X,Y,Rotate）
    */
    void Set_Target(_ch_f32t, _ch_f32t, _ch_f32t);
  
    /* 设置所有模式运动速度档位（不同档位速度最大值不同）*/
    void Set_SpeedGear(_chassis_SpeedGears);
    
    /* 设置底盘运动参数(后续有需要可继续添加修改其他参数的接口) */
    void Set_SpeedParam(_ch_f32t slow, _ch_f32t normal, _ch_f32t fast, _ch_f32t z);
    void Set_AccelerationParam(_ch_16t launch, _ch_16t normal);
    void Set_TorqueOptimizeFlag(_ch_flag flag);
    void Set_AttitudeOptimizeFlag(_ch_flag flag);
    
    /* 从外部加载位置环、速度环控制器，参数列表见函数注释*/
    void Load_AttitudeController(_chassis_Velocity*(*pFunc)(const _chassis_GlobalPos*, const _chassis_GlobalPos*));
    void Load_PositionController(_chassis_Velocity*(*pFunc)(const _chassis_GlobalPos*, const _chassis_GlobalPos*));
    void Load_SpeedController(_ch_32t*(*pFunc)(const _ch_16t*, const _ch_16t*));
    
    /* 更新普通位置模式下的零位姿点为当前位姿点*/
    void Update_ZeroPose();
  
    /* 更新当前位姿（X,Y,Roll,Pitch,Yaw）*/
    void Update_CurrentPosition(_ch_f32t, _ch_f32t);
    void Update_CurrentAttitude(_ch_f32t, _ch_f32t, _ch_f32t);
  
   /* 切换底盘运行状态 */
    _chassis_Mode Get_Mode();
    _ch_U8t Switch_Mode(_chassis_Mode target_mode);
    
   /* 控制主函数 */
    _chassis_Mode Chassis_Control();
};
#endif
#endif
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
