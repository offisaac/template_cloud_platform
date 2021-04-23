/**
  **********************************************************************************
  * @file   : Engineer_Header.h
  * @brief  : Header for common type define for robot lib.
  * @author : Mentos_Seetoo
  * @date   : July,2019
  * @version: 2.0
  **********************************************************************************
**/
#ifndef _ENGINEER_HEADER_H_
#define _ENGINEER_HEADER_H_
/* Includes ------------------------------------------------------------------*/ 
#include <stdint.h>

#ifdef ARM_MATH_CM4
#include  <arm_math.h>
#endif

#if (defined USE_HAL_DRIVER) && (defined STM32F405xx)
  #define USE_FAULT_ALARM
#endif

#ifdef  __cplusplus
/* Private  ------------------------------------------------------------------*/
#define UNFINISHED     0
#define FINISHED       1
#define OPERATING      2
#define RM_OFFLINE     0
#define RM_ONLINE      1

/* Common type define for robot lib(Not for user)*/
typedef struct Vector_3f
{
  float x;
  float y;
  float z;
}Vector_3f;

typedef struct Euler_3f
{
  float roll;
  float pitch;
  float yaw;
}Euler_3f;

/* Exported ------------------------------------------------------------------*/
/* Common type define for robot lib*/
enum _ActionMode_t
{
  CHASSIS_LOCK = 0U,
  NORMAL,
  GET_BULLET,
  GIVE_BULLET_NORMAL,
  UP_STAIRS,
  RESCUE,
  AUTO_ROTATE,
};

enum _View_t
{
  FORWARD = 0U,
  BACK_UPSTAIRS,
  BACK_BULLET
};

enum _Gear_t
{
  NO_CHANGE = 0U,
  SPEED_UP,
  SLOW_DOWN
};

enum _GetBullet_t
{
  APPROACH = 0U,
  MANUAL,
  ALIGNING_FIRST,
  ALIGNING_AGAIN
};

enum _UpStairs_t
{
  MANUAL_UPSTAIRS = 0U,
  STAGE1_LIFT,
  STAGE1_LAY_DOWN,
  TRANSITION,
  STAGE2_LIFT,
  STAGE2_LAY_DOWN,
  LAND_ON
};

enum _UpStairsTime_t
{
  ONEC = 0U,
  TWICE
};

enum _Rescue_t
{
  ATTACHING = 0U,
  PULLING_OUT
};

enum _Auto_Rotate_t
{
  SINGLE = 0U,
  CONTINUOUS,
  CAT_WALK
};

enum _RobotStatus_t
{
  Power_Disable = 0U,
  Power_Enable,
  Robot_Fault
};

enum _RobotFault_t
{
  BODY_MOTOR_DAMAGE    = (1 << 0),
  BODY_LOST            = (1 << 1),
  CHASSIS_MOTOR_DAMAGE = (1 << 2),
  IMU_LOST             = (1 << 3),
  NUC_LOST             = (1 << 4)
};


enum _IO_Type
{
  VELOCITY_OUT,
  VELOCITY_IN,
  POSITION_IN,
  IMU_IN,
  TORQUE_IN,
  CHASSIS_TEMPRE_IN,
  CHASSIS_MAGNET_IN,
  CHASSIS_PES_IN,
  CONNECTION_STATE_IN,
  BODY_OPERATION_OUT
};

/* 
   Sensor Units & Operation Units that are used in Engineer.
   The sequence of wheels in 'Unit_Type' must be consistent with
   that in '_chassis_WheelType'
*/
enum _Unit_Type
{
  WHEEL_LF = 0U,
  WHEEL_RF,
  WHEEL_RB,
  WHEEL_LB,
  WHEEL_LM,
  WHEEL_RM,
  BODY_MOTORS,
  CHASSIS_PES_PULL,
  CHASSIS_PES_STOP,
  CHASSIS_MAGNET,
  IMU,
  NUC
};

typedef struct _UnitStatus_t
{
  uint8_t chassis_motors;
  uint8_t chassis_Magnet_Lift;
  uint8_t chassis_PES_Pull;
  uint8_t chassis_PES_Stop;
  uint8_t body_motors;
  uint8_t body_photoelectrics;
  uint8_t body_cylinders;
  uint8_t IMU;
  uint8_t NUC;
}_UnitStatus_t;

typedef struct _Body_Operation_t
{
  _GetBullet_t   get_bullet_flag;
  uint8_t        detected_flag;
  uint8_t        send_bullet_flag;
  _Rescue_t      rescue_flag;
  _UpStairs_t    upstairs_flag;
  _Auto_Rotate_t auto_rotate_flag;
}_Body_Operation_t;
#endif
#endif  
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
