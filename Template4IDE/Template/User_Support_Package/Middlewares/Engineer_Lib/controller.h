/**
  **********************************************************************************
  * @file   : controller.h
  * @brief  : Header for controller(Robomaster 2019 Engineer version 4.0) robot lib.
  * @author : Mentos_Seetoo
  * @date   : July,2019
  * @version: 1.0
  **********************************************************************************
**/

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "chassis.h"
#include "Tool_PID.h"
#ifdef  __cplusplus
/* Private  ------------------------------------------------------------------*/

/* Exported ------------------------------------------------------------------*/
extern FUZZYPID_without_gyroscope Pose_XAxis, Pose_YAxis, Pose_Yaw;
void Controller_Init(void);
_chassis_Velocity* Controller_ChassisAttitudeLoop(const _chassis_GlobalPos* CurrentPose, const _chassis_GlobalPos* TargetPose);
_chassis_Velocity* Controller_ChassisPositionLoop(const _chassis_GlobalPos* CurrentPose, const _chassis_GlobalPos* TargetPose);
int32_t* Controller_ChassisSpeedLoop(const int16_t* currentSpeed, const int16_t* targetSpeed);
void Controller_SetMaxAdjustSpeed(float max_adjust_speed);

#endif
#endif
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
