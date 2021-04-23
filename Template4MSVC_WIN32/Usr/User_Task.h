/**
******************************************************************************
* @file   User_Task.h
* @brief  Header file of User Tasks.
******************************************************************************
* @note
*  - Before running your task, just do what you want ~
*  - More devices or using other classification is decided by yourself ~
*/
#pragma once
/* Includes ------------------------------------------------------------------*/
#include "System_DataPool.h"
/* Private macros ------------------------------------------------------------*/
/* Private type --------------------------------------------------------------*/
enum _Joint_Type
{
  Shoulder = 0U,
  Hip,
  Knee,
  Foot
};

enum _Leg_Type
{
  _LF = 0U,
  _RF,
  _RB,
  _LB
};
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern TaskHandle_t RobotCtrl_Handle;
extern TaskHandle_t DataDisplay_Handle;
/* Exported function declarations --------------------------------------------*/
void User_Tasks_Init(void);
void Task_DataDisplay(void *arg);

/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/

