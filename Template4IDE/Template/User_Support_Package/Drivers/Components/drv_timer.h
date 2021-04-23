/**
  ******************************************************************************
  * Copyright (c) 2019 - ~, SCUT-RobotLab Development Team
  * @file   : drv_timer.h
  * @author : Mentos_Seetoo 1356046979@qq.com
  * @brief  : Code for CAN driver in STM32F4 using STM32Cube_FW_F4_V1.23.0.
  * @date   : 2019-06-12
  * @Changes：
  *  Date           Author         Version    Notes
  *  2019-06-12     Mentos Seetoo   1.0.0     Creator
  ******************************************************************************
  * @attention
  * 
  * if you had modified this file, please make sure your code does not have many 
  * bugs, update the version NO., write dowm your name and the date, the most
  * important is make sure the users will have clear and definite understanding 
  * through your new brief.
  ******************************************************************************
  */
#ifndef  _DRV_TIMER_H
#define  _DRV_TIMER_H

#ifdef  __cplusplus
extern "C"{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal_tim.h>
  
/* Private define ------------------------------------------------------------*/
#define micros()    Get_SysterTimer()
#define delay_ms    HAL_Delay
#define delay_us    delay_us_nos
  
/* Private variables ---------------------------------------------------------*/
#ifdef USE_DEFAULT_DELAY_TIMER
extern TIM_HandleTypeDef htim4;
#else
#error "No Timer is configured for this module!"
#endif

/* Private type --------------------------------------------------------------*/
typedef struct{
  uint32_t last_time;
  float dt;
}TimeStamp;

/* Exported function declarations --------------------------------------------*/
extern volatile uint32_t SystemTimerCnt;
void timer_init(void);
uint32_t Get_SysterTimer(void);
void delay_us(uint8_t cnt);
void delay_ms_nos(uint32_t cnt);
void delay_us_nos(uint32_t cnt);
#ifdef  __cplusplus
}
#endif

#endif 
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
