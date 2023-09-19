/**
  ******************************************************************************
  * @file   System_DataPool.cpp
  * @brief  All used resources are contained in this file.
  ******************************************************************************
  * @note
  *  - User can define datas including variables ,structs ,and arrays in
  *    this file, which are used in deffrient tasks or services.
**/
#include "internal.h"

/* RTOS Resources ------------------------------------------------------------*/
/* Queues */
QueueHandle_t USART_TxPort;					//	串口发送队列
QueueHandle_t USART_RxPort;					//串口接收队列
QueueHandle_t CAN1_TxPort;					//	can1 发送队列
QueueHandle_t CAN1_RxPort;					//	can1 接收队列
QueueHandle_t CAN2_TxPort;					//	can2 发送队列
QueueHandle_t CAN2_RxPort;					//	can2 接收队列
QueueHandle_t DR16_QueueHandle;			//	dr16（串口） 接收队列

/* Semaphores */

/* Mutexes */
SemaphoreHandle_t DR16_mutex;				//	dr16互斥量

/* Notifications */

/* Other Resources -----------------------------------------------------------*/
uint8_t Uart1_Rx_Buff[USART1_RX_BUFFER_SIZE]; /*!< Receive buffer for Uart1 */
uint8_t Uart2_Rx_Buff[USART2_RX_BUFFER_SIZE]; /*!< Receive buffer for Uart2 */
uint8_t Uart3_Rx_Buff[USART3_RX_BUFFER_SIZE]; /*!< Receive buffer for Uart2 */
uint8_t Uart4_Rx_Buff[USART4_RX_BUFFER_SIZE]; /*!< Receive buffer for Uart4 */
uint8_t Uart5_Rx_Buff[USART5_RX_BUFFER_SIZE]; /*!< Receive buffer for Uart5 */
uint8_t Uart6_Rx_Buff[USART6_RX_BUFFER_SIZE];/*!< Receive buffer for Uart6 */

#if USE_SRML_MPU6050
mpu_rec_s mpu_receive; //mpu6050数据
#endif

DR16_Classdef DR16;		//遥控器DR16类

Motor_GM6020 Yaw(5);
Motor_GM6020 Pitch(2);
Motor_C610 Dial(1);//c610 ID1
Motor_C620 R_Fric(5);//c620 ID5 
Motor_C620 L_Fric(3);//c620 ID3

myPID PID_L_Fric_Speed(50,0,0,10000,10000);
myPID PID_R_Fric_Speed(50,0,0,10000,10000);//已经调好

myPID PID_Dial_Speed(20,0,0,500,10000);
myPID PID_Dial_Angle(4.5,0,0,500,10000);//已经调好

myPID PID_Yaw_Speed(800,0,0,2500,30000);
myPID PID_Yaw_Angle(1.5,0,0,2500,30000);//未调好

myPID PID_Pitch_Speed(500,0,0,3000,30000);
myPID PID_Pitch_Angle(4.6,0,0,3000,30000);//未调好

int angle_set=-1296;
uint8_t change_flag=0;//判断是否切换按键用
uint8_t fre=20;
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/



