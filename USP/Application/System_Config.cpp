/**
  ******************************************************************************
  * @file   APP.cpp
  * @brief  Devices Application running file.
  ******************************************************************************
  * @note
  *  - Before running your devices, just do what you want ~ !
  *  - More devices or using other classification is decided by yourself ~ !
  ===============================================================================
                                    Task List
  ===============================================================================
  * <table>
  * <tr><th>Task Name     <th>Priority          <th>Frequency/Hz    <th>Stack/Byte
  * <tr><td>              <td>                  <td>                <td>
  * </table>
  *
 */

/* Includes ------------------------------------------------------------------*/
#include "app.h"
#include "internal.h"
#include <Middlewares/UpperMonitor/UpperMonitor.h>
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*Private Function declarations -------------------------------------------------------*/
#if USE_SRML_MPU6050
static void System_Mpu_Init();				
#endif
/* Function prototypes -------------------------------------------------------*/

/**
  * @brief  application device initialization task
  */
void System_Device_Init(void)
{
  /* Drivers Init */
	// timer init
	Timer_Init(&htim4, USE_MODULE_DELAY);
	
	////此设置语句使得delay_ms_nos()函数调用的是HAL_Delay(),容易造成FreeRTOS进程紊乱
  //Timer_Init(&htim4, USE_HAL_DELAY);
	
  // can init
	CAN_Init(&hcan1, User_CAN1_RxCpltCallback);
  CAN_Init(&hcan2, User_CAN2_RxCpltCallback);
	
	CAN_Filter_Mask_Config(&hcan1,CanFilter_1 | CanFifo_0 | Can_STDID | Can_DataType, 0x201, 0x300);
	
  CAN_Filter_Mask_Config(&hcan2,CanFilter_14 | CanFifo_0 | Can_STDID | Can_DataType, 0x201, 0x300);
  
	// uart init
  Uart_Init(&huart1, Uart1_Rx_Buff, USART1_RX_BUFFER_SIZE, RecHandle);
  Uart_Init(&huart2, Uart2_Rx_Buff, USART2_RX_BUFFER_SIZE, User_UART2_RxCpltCallback);
	Uart_Init(&huart3, Uart3_Rx_Buff, USART3_RX_BUFFER_SIZE, RecHandle);
	Uart_Init(&huart4, Uart4_Rx_Buff, USART4_RX_BUFFER_SIZE, RecHandle);
	Uart_Init(&huart5, Uart5_Rx_Buff, USART5_RX_BUFFER_SIZE, RecHandle);
	Uart_Init(&huart6, Uart6_Rx_Buff, USART6_RX_BUFFER_SIZE, RecHandle);
  /* Modules Init */
	#if  USE_SRML_MPU6050
  System_Mpu_Init();
	#endif
	
  myPIDTimer::getMicroTick_regist (Get_SystemTimer);
  asuwave_init(&huart1, xTaskGetTickCount);

}


/**
 * @brief application freertos init function.
 */
void System_Task_Init(void)
{
  /* Queue Init */
	CAN1_TxPort 		= xQueueCreate(4, sizeof(CAN_COB));
  CAN1_RxPort 		= xQueueCreate(4, sizeof(CAN_COB));
  CAN2_TxPort 		= xQueueCreate(4, sizeof(CAN_COB));
  CAN2_RxPort 		= xQueueCreate(4, sizeof(CAN_COB));
  USART_TxPort 		= xQueueCreate(2, sizeof(USART_COB));
	USART_RxPort 		= xQueueCreate(2, sizeof(USART_COB));
  DR16_QueueHandle 	= xQueueCreate(2, sizeof(USART_COB));
  /* Semaphore Init */
  /* Mutex Init */
  DR16_mutex = xSemaphoreCreateMutex();
  /* Task Init */
	Service_Communication_Init();
	Service_Devices_Init();
	Service_Debug_Init();
}

	#if  USE_SRML_MPU6050
/**
 * @brief 		mpu6050初始化函数
 * @function	确定mpu6050 i2c 通信引脚；
 * @function	mpu6050按给定参数初始化
 */
static void System_Mpu_Init(){
	/*	配置MPU6050 I2C 引脚	*/
	MPU6050_Config_Pin(GPIOB,GPIO_PIN_6,GPIO_PIN_7);
	/*	配置MPU6050 参数  完成初始化	*/
	MPU6050_Init(&mpu_config,&dmp_config);
	/*	运行陀螺仪自检	*/
	MPU6050_run_self_test(0);
}
	#endif
