/**
  ******************************************************************************
  * Copyright (c) 2019 - ~, SCUT-RobotLab Development Team
  * @file    drv_uart.c
  * @author  LWJ 851756890@qq.com
  * @brief   CCode for UART driver in STM32F4 using STM32Cube_FW_F4_V1.24.0.
  * @date    2019-06-12
  * @version 1.1
  * @par Change Log：
  * <table>
  * <tr><th>Date        <th>Version  <th>Author    		<th>Description
  * <tr><td>2019-06-12  <td> 1.0     <td>charlie  		<td>Creator
  * <tr><td>2019-10-28  <td> 1.1	 <td>LWJ			<td>Remove the precompiled macro\n
  * 														Remove receive buffer\n
  * 														Add user specific buffer.
  * </table>
  *
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
    @note
		-# 调用`Uart_Init()`，传入串口对象句柄地址，缓冲数组首地址，数组长度，初始化\n
			串口。
		-# 调用`Usart_Rx_Callback_Register()`设置串口接收中断回调函数的指针。
		-# 在`stm32f4xx_it.c`对应的串口中断里面加入`Uart_Receive_Handler()`，注意\n
			使用头文件extern的结构体。
    	-# 在需要用到发送的部分直接调用HAL_UART_Transmit_DMA()函数。
    
    @warning:
		-# 用户需要自己定义缓存数组并初始化数组，数组类型为uint8_t。
		-# 添加预编译宏`NDEBUG`可以禁用断言检查。
		
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
/* Includes ------------------------------------------------------------------*/
#include "drv_uart.h"
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

usart_manage_obj_t usart4_manage_obj =
	{
			.call_back_f = NULL};

usart_manage_obj_t usart5_manage_obj =
{
		.call_back_f = NULL};

usart_manage_obj_t usart1_manage_obj =
		{
			.call_back_f = NULL};

usart_manage_obj_t usart2_manage_obj =
		{
			.call_back_f = NULL};

usart_manage_obj_t usart3_manage_obj =
		{
				.call_back_f = NULL};

usart_manage_obj_t usart6_manage_obj =
		{
				.call_back_f = NULL};
/* Private type --------------------------------------------------------------*/

/* Private function declarations ---------------------------------------------*/
static void Uart_Rx_Idle_Callback(usart_manage_obj_t *m_obj);
static void Error_Handler(void);

/* function prototypes -------------------------------------------------------*/
/**
* @brief    initialize uart device
* @param    *huart: uart IRQHandler
* @param	Rxbuffer: user buffer array
* @param	length: the length of array
* @retval   None
*/
void Uart_Init(UART_HandleTypeDef *huart, uint8_t *Rxbuffer, uint32_t length)
{
	if (huart == NULL)
		Error_Handler();
	else {}

	if(huart->Instance == UART4)
	{
		usart4_manage_obj.rx_buffer = Rxbuffer;
		usart4_manage_obj.rx_buffer_size = length;
		usart4_manage_obj.uart_h = huart;
		__HAL_UART_CLEAR_IDLEFLAG(huart);
		__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
		HAL_UART_Receive_DMA(huart, Rxbuffer, length);
	}

	else if (huart->Instance == UART5)
	{
		usart5_manage_obj.rx_buffer = Rxbuffer;
		usart5_manage_obj.rx_buffer_size = length;
		usart5_manage_obj.uart_h = huart;
		__HAL_UART_CLEAR_IDLEFLAG(huart);
		__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
		HAL_UART_Receive_DMA(huart, Rxbuffer, length);
	}
    
	else if (huart->Instance == USART1)
	{
		usart1_manage_obj.rx_buffer = Rxbuffer;
		usart1_manage_obj.rx_buffer_size = length;
		usart1_manage_obj.uart_h = huart;
		__HAL_UART_ENABLE_IT(huart,UART_IT_IDLE);
			__HAL_UART_CLEAR_IDLEFLAG(huart);
		HAL_UART_Receive_DMA(huart, Rxbuffer, length);
	}
    
	else if (huart->Instance == USART2)
	{
		usart2_manage_obj.rx_buffer = Rxbuffer;
		usart2_manage_obj.rx_buffer_size = length;
		usart2_manage_obj.uart_h = huart;
		__HAL_UART_CLEAR_IDLEFLAG(huart);
		__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
		HAL_UART_Receive_DMA(huart, Rxbuffer, length);
	}
   
	else if (huart->Instance == USART3)
	{
		usart3_manage_obj.rx_buffer = Rxbuffer;
		usart3_manage_obj.rx_buffer_size = length;
		usart3_manage_obj.uart_h = huart;
		__HAL_UART_CLEAR_IDLEFLAG(huart);
		__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
		HAL_UART_Receive_DMA(huart, Rxbuffer, length);
	}
   
	else if (huart->Instance == USART6)
	{
		usart6_manage_obj.rx_buffer = Rxbuffer;
		usart6_manage_obj.rx_buffer_size = length;
		usart6_manage_obj.uart_h = huart;
		__HAL_UART_CLEAR_IDLEFLAG(huart);
		__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
		HAL_UART_Receive_DMA(huart, Rxbuffer, length);
	}
	
	else
		Error_Handler();
}

/**
 * @brief   Registered user callback function
 * @param   m_obj: serial port handle
 * @param   fun: user callback function
 * @retval  None
 */
void Usart_Rx_Callback_Register(usart_manage_obj_t *m_obj, usart_call_back fun)
{
  /* Check the parameters */
	assert(fun != NULL);
	assert(m_obj != NULL);
	
	m_obj->call_back_f = fun;
	return;
}

/**
 * @brief   Determine if the idle interrupt is triggered
 * @param   m_obj: serial port handle
 * @retval  None
 */
void Uart_Receive_Handler(usart_manage_obj_t *m_obj)
{
	if(__HAL_UART_GET_FLAG(m_obj->uart_h,UART_FLAG_IDLE)!=RESET)
	{
		Uart_Rx_Idle_Callback(m_obj);
	}
}

/**
 * @brief   clear idle it flag after uart receive a frame data
 * @note    call in uart_receive_handler() function
 * @param   uart IRQHandler id
 * @retval  None
 */
static void Uart_Rx_Idle_Callback(usart_manage_obj_t *m_obj)
{
  /* Check the parameters */
	assert(m_obj != NULL);
	
  /* Private variables */
	static uint16_t usart_rx_num;

  /* clear idle it flag avoid idle interrupt all the time */
	__HAL_UART_CLEAR_IDLEFLAG(m_obj->uart_h);

  /* clear DMA transfer complete flag */
	HAL_UART_DMAStop(m_obj->uart_h);

  /* handle received data in idle interrupt */
	usart_rx_num = m_obj->rx_buffer_size - m_obj->uart_h->hdmarx->Instance->NDTR;
	if(m_obj->call_back_f != NULL)
		m_obj->call_back_f(m_obj->rx_buffer, usart_rx_num);
	
	HAL_UART_Receive_DMA(m_obj->uart_h, m_obj->rx_buffer, m_obj->rx_buffer_size);
  
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
static void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
    
}

/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
