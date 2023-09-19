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

#define USART1_RX_BUFFER_SIZE 128
#define USART2_RX_BUFFER_SIZE 128
#define USART3_RX_BUFFER_SIZE 128
#define USART4_RX_BUFFER_SIZE 128
#define USART5_RX_BUFFER_SIZE 128
#define USART6_RX_BUFFER_SIZE 256

/* HAL Handlers --------------------------------------------------------------*/
extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

extern TIM_HandleTypeDef htim4;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart6;

/* RTOS Resources ------------------------------------------------------------*/
/* Task */
extern TaskHandle_t CAN1SendPort_Handle;
extern TaskHandle_t CAN2SendPort_Handle;
extern TaskHandle_t CAN1ReceivePort_Handle;
extern TaskHandle_t CAN2ReceivePort_Handle;
extern TaskHandle_t UartTransmitPort_Handle;
extern TaskHandle_t UartReceivePort_Handle;

extern TaskHandle_t DR16_Handle;

extern TaskHandle_t Debug_Handle;

/* Queues */
extern QueueHandle_t USART_TxPort;
extern QueueHandle_t USART_RxPort;
extern QueueHandle_t CAN1_TxPort;				
extern QueueHandle_t CAN1_RxPort;		
extern QueueHandle_t CAN2_TxPort;
extern QueueHandle_t CAN2_RxPort;

extern QueueHandle_t DR16_QueueHandle;
/* Semaphores */
 /* mutex */
extern SemaphoreHandle_t DR16_mutex;
/* Mutexes */
/* Notifications */
/* Other Resources -----------------------------------------------------------*/
extern uint8_t Uart1_Rx_Buff[USART1_RX_BUFFER_SIZE];
extern uint8_t Uart2_Rx_Buff[USART2_RX_BUFFER_SIZE];
extern uint8_t Uart3_Rx_Buff[USART3_RX_BUFFER_SIZE];
extern uint8_t Uart4_Rx_Buff[USART4_RX_BUFFER_SIZE];
extern uint8_t Uart5_Rx_Buff[USART5_RX_BUFFER_SIZE];
extern uint8_t Uart6_Rx_Buff[USART6_RX_BUFFER_SIZE];

#if  USE_SRML_MPU6050
extern mpu_rec_s mpu_receive; //mpu6050����
#endif

extern DR16_Classdef DR16;		//ң����DR16��


extern Motor_GM6020 Yaw;
extern Motor_GM6020 Pitch;
extern Motor_C610 Dial;
extern Motor_C620 R_Fric;
extern Motor_C620 L_Fric;
/* Exported function declarations --------------------------------------------*/
void Service_Debug_Init(void);
void Service_Communication_Init(void);
void Service_Devices_Init(void);

void User_CAN1_RxCpltCallback(CAN_RxBuffer *CAN_RxMessage);
void User_CAN2_RxCpltCallback(CAN_RxBuffer *CAN_RxMessage);

uint32_t User_UART1_RxCpltCallback(uint8_t *Recv_Data, uint16_t ReceiveLen);
uint32_t User_UART2_RxCpltCallback(uint8_t *Recv_Data, uint16_t ReceiveLen);
uint32_t User_UART2_RxCpltCallback(uint8_t *Recv_Data, uint16_t ReceiveLen);
uint32_t User_UART3_RxCpltCallback(uint8_t* Recv_Data, uint16_t ReceiveLen);
uint32_t User_UART4_RxCpltCallback(uint8_t* Recv_Data, uint16_t ReceiveLen);
uint32_t User_UART5_RxCpltCallback(uint8_t* Recv_Data, uint16_t ReceiveLen);
uint32_t User_UART6_RxCpltCallback(uint8_t* Recv_Data, uint16_t ReceiveLen);

#ifdef __cplusplus
}
#endif

#endif /* _INTERNAL_H_ */
