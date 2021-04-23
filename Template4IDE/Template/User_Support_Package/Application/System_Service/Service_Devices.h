/**
 ********************************************************
 * Copyright (c) 2019-2019, RobotLab Development Team
  * @file   ： rs_device.h
  * @brief  ： Template class for device object.
 *  @Changes：
 *  Date           Author   Version    Notes
 *  2019-06-22     Mentos    0.0.1     first version
 ********************************************************
 */
#ifndef  _SERVICE_DEVICES_H_
#define  _SERVICE_DEVICES_H_
#include "System_DataPool.h"
#include "rs_def.h"
#include "rs_conf.h"
#include "main.h"
#ifdef  __cplusplus
extern ESC_C620 C620_LF;
extern ESC_C620 C620_RF;
extern ESC_C620 C620_RM;
extern ESC_C620 C620_RB;
extern ESC_C620 C620_LB;
extern ESC_C620 C620_LM;
extern DR16_Classdef DR16;
extern Gyro_Classdef IMU_Obj;
extern VisionAlign_Classdef NUC_Obj;
extern CSuperRelay SuperRelay_Obj;
#endif

#ifdef  __cplusplus
extern "C"{
#endif
/*------------------------------ System Handlers ------------------------------*/
extern TaskHandle_t DeviceESC_Handle;
extern TaskHandle_t DeviceDR16_Handle;
extern TaskHandle_t DeviceSensors_Handle;

/*------------------------------Function prototypes ---------------------------*/
void Service_Devices_Init(void);
void Device_ESC(void *arg);
void Device_DR16(void *arg);
void Device_Sensors(void *arg);
  
#ifdef  __cplusplus
}
#endif

#endif  

/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/

