 /*
 * internal.h
 *
 *  Created on: Apr 7, 2021
 *      Author: M3chD09
 */

#ifndef _INTERNAL_H_
#define _INTERNAL_H_

#include "SRML.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

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

/* HAL Handlers --------------------------------------------------------------*/
extern CAN_HandleTypeDef hcan2;

extern TIM_HandleTypeDef htim4;


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

/* RTOS Resources ------------------------------------------------------------*/
/* Task */
// robot control task
// communicatin task
extern TaskHandle_t CAN2Transmit_Handle;
extern TaskHandle_t CAN2Receive_Handle;
//remote control task
extern TaskHandle_t DR16_Handle;
// device task
// debug task



/* Queues */
extern QueueHandle_t USART_TxPort;
extern QueueHandle_t CAN2_TxPort;
extern QueueHandle_t CAN2_RxPort;

extern QueueHandle_t DR16_QueueHandle;
/* Semaphores */
 /* mutex */
extern SemaphoreHandle_t DR16_mutex;
/* Mutexes */
/* Notifications */
/* Other Resources -----------------------------------------------------------*/
#define USART1_RX_BUFFER_SIZE 32
#define USART2_RX_BUFFER_SIZE 64

extern uint8_t Uart1_Rx_Buff[USART1_RX_BUFFER_SIZE];
extern uint8_t Uart2_Rx_Buff[USART2_RX_BUFFER_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* _INTERNAL_H_ */
