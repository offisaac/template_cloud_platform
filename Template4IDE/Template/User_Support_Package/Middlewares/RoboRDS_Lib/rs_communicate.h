/**
 ********************************************************
 * Copyright (c) 2019-2019, RobotLab Development Team
  * @file   ： rs_communicate.h
  * @brief  ： Manage the communication of applications
               or specific objects.
 *  @Changes：
 *  Date           Author   Version    Notes
 *  2019-06-22     Mentos    0.0.1     origin version
 ********************************************************
 */
 
#ifndef __RS_COMMUNICATE_H__
#define __RS_COMMUNICATE_H__

/* Includes ------------------------------------------------------------------*/
#include "rs_def.h"
#include "rs_conf.h"
#include <string>
#include <vector>

#ifdef FreeRTOS
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#else
      #error RTOS support error!
#endif

#ifdef __cplusplus
/* Private  ------------------------------------------------------------------*/



/* Exported ------------------------------------------------------------------*/

#endif
#endif
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
