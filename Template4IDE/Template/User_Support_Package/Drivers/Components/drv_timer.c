/**
  ******************************************************************************
  * Copyright (c) 2019 - ~, SCUT-RobotLab Development Team
  * @file   : drv_timer.c
  * @author : Mentos_Seetoo 1356046979@qq.com
  * @brief  : Code for Timer Management in STM32F4 using STM32Cube_FW_F4_V1.23.0.
  * @date   : 2019-06-12
  * @Changes：
  *  Date           Author         Version    Notes
  *  2019-06-12     Mentos Seetoo   1.0.0     Creator
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
    @Config:
    1.配置TIM4自增时间为1us，在TIM4中断中加入SystemTimerCnt++
    2.可用宏定义常用的delay_ms()为其他函数而使用本模块的函数实现，如HAL_Delay()等
    
    @Usage:
    1.调用相关延时函数
    
    @warning:
    1.配合实验室硬件设计,一般使用TIM4为其他任务提供精确定时(us,ms)，如遇到特殊情况
      需要更改配置，请自行更改有关内容。
    2.由于本模块运行基于STM32CubeMX生成的代码，所以头文件extern部分常用固定的结构体
      名称,也方便其他地方调用。如果需要更改配置的定时器，请自行在头文件extern相应的
      结构体,并添加相应的宏定义。
  ******************************************************************************
  * @attention
  * 
  * if you had modified this file, please make sure your code does not have many 
  * bugs, update the version NO., write dowm your name and the date, the most
  * important is make sure the users will have clear and definite understanding 
  * through your new brief.
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "assert.h"
#include "drv_timer.h"
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
volatile uint32_t SystemTimerCnt;

/* Private type --------------------------------------------------------------*/

/* Private function declarations ---------------------------------------------*/

/* function prototypes -------------------------------------------------------*/
/**
* @brief  Initialize Timer
* @param  None.
* @return None.
*/
void timer_init(void)
{
#ifdef USE_DEFAULT_DELAY_TIMER
  if(HAL_TIM_Base_Start_IT(&htim4)!=HAL_OK)__nop();
  if(HAL_TIM_Base_Start(&htim4)!= HAL_OK)__nop();
  /* 
    Normally won't run here, unless user doed not config a timer for this module.
  */
#else
#error "No Timer is configured for this module!"
#endif
}

/**
* @brief  Get the system tick from timer.
* @param  None.
* @return current tick.
*/
uint32_t Get_SysterTimer(void)
{
#ifdef USE_DEFAULT_DELAY_TIMER
  return TIM4->CNT + SystemTimerCnt * 0xffff;
  /* 
    Normally won't run here, unless user doed not config a timer for this module.
  */
#else
#error "No Timer is configured for this module!"
#endif
}

/**
  * @brief  Delay for us
  * @param  cnt : microsecond to delay 
  * @retval None.
  */
void delay_us_nos(uint32_t cnt)
{
  uint32_t temp = cnt  + micros();

  while(temp >= micros());
}

/**
  * @brief  Delay for ms
  * @param  cnt : millisecond to delay
  * @retval void
  */
void delay_ms_nos(uint32_t cnt)
{
  uint32_t temp = cnt * 1000 + micros();

  while(temp >= micros());
}
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
