/**
  ******************************************************************************
  * @file   : System_config.h
  * @brief  : Header for System_config.c
  * @author : Mentos Seetoo
  ****************************************************************************** 
**/

#ifndef SYS_CONFIG_H
#define SYS_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include <FreeRTOS.h>
#include "Service_Communication.h"
#include "Service_EngineerChassis.h"
#include "Service_Devices.h"
#include "Service_Debug.h"
#include "System_DataPool.h"


/* Macro Definitions ---------------------------------------------------------*/
#define Tiny_Stack_Size       64
#define Small_Stack_Size      128
#define Normal_Stack_Size     256
#define Large_Stack_Size      512
#define Huge_Stack_Size       1024
#define PriorityVeryLow       1
#define PriorityLow           2
#define PriorityBelowNormal   3
#define PriorityNormal        4
#define PriorityAboveNormal   5
#define PriorityHigh          6
#define PrioritySuperHigh     7
#define PriorityRealtime      8

/* Function Prototypes--------------------------------------------------------*/
void System_Tasks_Init(void);                                       //Founction to load and start the tasks.
void System_Resource_Init(void);                                    //Founction to initializa Drivers and Module data resources.


#endif
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
