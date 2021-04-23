/*
 * Copyright (c) 2019-2019, RobotLab Development Team
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-06-15     Manny        the first version
 */
 
#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__

#ifdef __cplusplus
 extern "C" {
#endif 
  
/**
 * @addtogroup include
 */
	
/**@{*/
	 
/* Standard Lib */	 
#include "stdint.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "cmsis_os.h"
#include <assert.h>
#include "arm_math.h"
#include "string.h"
	 
#include "rs_conf.h"
#include "rs_def.h"

/* MCU */
#if USING_F1xx_
/* for F1 */
#include "stm32f1xx_hal.h"
#include "stm32f103xe.h"
#esleif USING_F4xx_
/* for F4 */
#include "stm32f4xx_hal.h"
#include "stm32f405xx.h"
#elseif USING_H7xx_
/* for H7 */
#include "stm32h7xx_hal.h"
#include "stm32h743xx.h"
#endif

/* Drivers/BSP/Components */  
#include "drv_uart.h"
#include "usart.h"
#include "dma.h"
#include "myiic.h"
#include "iic.h"
#include "can.h"

/* Drivers/BSP/Boards */
#include "Systemtimer.h"

/* Middlewares */
#include "rs_ringbuffer.h"
#include "rs_printf.h"

/* Applications */

/**@}*/

/**
 * @addtogroup CubeMX生成的一些句柄的全局
 */
	
/**@{*/

/* hw resources */
extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim4;
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart3_tx;
extern DMA_HandleTypeDef hdma_usart6_rx;
extern DMA_HandleTypeDef hdma_usart6_tx;

/* thread ID */
extern osThreadId DisplayTaskHandle;
extern osThreadId CAN1Data0RecTaskHandle;
extern osThreadId CAN1Data1RecTaskHandle;
extern osThreadId CAN2Data0RecTaskHandle;
extern osThreadId ChasisTaskHandle;
extern osThreadId DR16TaskHandle;
extern osThreadId PitchYawTaskHandle;

/* thread func */
extern void DisplayTask(void const * argument);
extern void CAN1Data0RecTask(void const * argument);
extern void CAN1Data1RecTask(void const * argument);
extern void CAN2Data0RecTask(void const * argument);
extern void ChasisTask(void const * argument);
extern void IMUTask(void const * argument);
extern void DR16Task(void const * argument);
extern void PitchYaw(void const * arguement);

/* message queue */
extern QueueHandle_t Queue_dr16_rx;
extern QueueHandle_t Queue_Uart5_rx;
extern QueueHandle_t Queue_Uart6_rx;
extern QueueHandle_t Queue_can1_rx0;
extern QueueHandle_t Queue_can1_rx1; 
extern QueueHandle_t Queue_can2_rx0; 
extern QueueHandle_t Queue_can1_rx0; 
extern QueueHandle_t Queue_can2_rx1;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
