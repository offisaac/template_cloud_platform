/**
  **********************************************************************************
  * Copyright (c) 2019-~, RobotLab Development Team
  * @file   ： System_config.c
  * @brief  ： Configure and deploy tasks and service in this file.
  * @author ： Mentos Seetoo
  **********************************************************************************
  *  1)Before running your Task you should first include your headers and load 
       used resources in "System_Resource_Init()", this function will be called 
       before RTOS Kernel Start.
  *  2)All tasks should be created in "System_Tasks_Init()", this function will
       be called in 'defaultTask()'.

  =================================================================================
                            ##### Task List #####
  =================================================================================
  |Task_Name               Priority              Frequency/Hz       Stack/Byte
  |———————————————————————————————————————
  |Robo.Engineer Ctrl ---- SuperHigh --------------  1000 -----------  64
  |Dev.Sensors ----------- High -------------------  200 ------------  64
  |Dev.DR16 -------------- High -------------------  200 ------------  64
  |Dev.C620 -------------- High -------------------  1000 -----------  64
  |Com.CAN Manager ------- High ------------------ unknown ----------  64
  |App.Remote Ctrl ------- Above Normal -----------  1000  ----------  64
  **/
/* Includes ------------------------------------------------------------------*/
#include <System_Config.h>
#include "rs_conf.h"
#include "rs_def.h"
#include "drv_can.h"
#include "drv_uart.h"
#include "drv_timer.h"

/* User applications */
#include "App_CeremonyDancing.h"
/*Founctions------------------------------------------------------------------*/
/**
* @brief  Load drivers ,modules, and datas for tasks.
*/
void System_Resource_Init(void)
{
  /********** Drivers Init **********/
  /* Timer */
  Drv_Timer_Start();
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
  
  /* CAN */
  Drv_CAN_Start(&hcan1);
  Drv_CAN_Start(&hcan2);
  /* USART */
  uart_init();

  /********** Module Init **********/

  
  /********** Data pool Init **********/
  SysDataPool_Init();
}  


/**
* @brief  Load and start User Tasks.
          Edit this fonction to add tasks into the activated tasks list.
*/
void System_Tasks_Init(void)
{ 
  /************ System Service Init ************/
  Service_Debug_Init();
  Service_Communication_Init();
  Service_Devices_Init();
  Service_EngineerChassis_Init();
  
  /************ User Application Init ************/
  xTaskCreate(Task_DanceInRemote, "App.Remote Ctrl", Tiny_Stack_Size, NULL, PrioritySuperHigh, &DanceCtrl_Handle);
}


/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/

