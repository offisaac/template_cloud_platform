/**
  ******************************************************************************
  * Copyright (c) 2019 - ~, SCUT-RobotLab Development Team
  * @file    drv_uart.h
  * @author  LWJ 851756890@qq.com
  * @brief   Code for UART driver in STM32F4 using STM32Cube_FW_F4_V1.24.0.
  ******************************************************************************
  * @attention
  * 
  * if you had modified this file, please make sure your code does not have many 
  * bugs, update the version Number, write dowm your name and the date, the most
  * important is make sure the users will have clear and definite understanding 
  * through your new brief.
  *
  * <h2><center>&copy; Copyright (c) 2019 - ~, SCUT-RobotLab Development Team.
  * All rights reserved.</center></h2>
  ******************************************************************************
  */
#ifndef __DRV_UART_H__
#define __DRV_UART_H__
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>
#include <assert.h>
/* Private macros ------------------------------------------------------------*/
/* Private type --------------------------------------------------------------*/
typedef uint32_t (*usart_call_back)(uint8_t *buf, uint16_t len);

/** 
* @brief Contain uart control info.
*/
typedef struct
{
  UART_HandleTypeDef *uart_h;
  uint16_t rx_buffer_size;
  uint8_t *rx_buffer;
  usart_call_back call_back_f;
} usart_manage_obj_t;

/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported function declarations --------------------------------------------*/
void Uart_Init(UART_HandleTypeDef *huart, uint8_t *Rxbuffer, uint32_t length);
void Usart_Rx_Callback_Register(usart_manage_obj_t *m_obj, usart_call_back fun);
void Uart_Receive_Handler(usart_manage_obj_t *m_obj);

/* Exported variables ---------------------------------------------------------*/
extern usart_manage_obj_t usart4_manage_obj;
extern usart_manage_obj_t usart5_manage_obj;
extern usart_manage_obj_t usart1_manage_obj;
extern usart_manage_obj_t usart2_manage_obj;
extern usart_manage_obj_t usart3_manage_obj;
extern usart_manage_obj_t usart6_manage_obj;

#ifdef __cplusplus
}
#endif
#endif

/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
