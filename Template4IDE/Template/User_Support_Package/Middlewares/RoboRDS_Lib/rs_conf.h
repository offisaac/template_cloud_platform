/**
 ********************************************************
 * Copyright (c) 2019-2019, RobotLab Development Team
  * @file   ： rs_conf.c
  * @brief  ： Configure and deploy hardware in this file.
 *  @Changes：
 *  Date           Author   Version    Notes
 *  2019-06-13     Manny    0.0.1      origin version
 *  2019-06-22     Mentos   0.0.2      immigration version
 ********************************************************
 */
 
#ifndef __RS_CONF_H__
#define __RS_CONF_H__

/* include rs_config header to import configuration */

#ifdef __cplusplus
extern "C" {
#endif

/* USING Cortex_MX */
#define USING_F1xx_ 0
#define USING_F4xx_ 1
#define USING_H7xx_ 0
  
/* USING BOARD VER */
#define UART4_ON_BOARD          1
#define UART5_ON_BOARD          1
#define USART1_ON_BOARD         1
#define USART2_ON_BOARD         1
#define USART3_ON_BOARD         1
#define USART6_ON_BOARD         0
  
/* USE_USER_ASSERT */
//#define USE_USER_ASSERT         0

/* Third Party RTOS*/
#define FreeRTOS                1
  
  
#ifdef __cplusplus
}
#endif

#endif
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
