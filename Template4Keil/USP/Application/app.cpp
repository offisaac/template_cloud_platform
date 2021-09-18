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


/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

mpu_rec_s mpu_receive; //mpu6050数据
//	构造类对象 ------------------------------------------------------
DR16_Classdef DR16;		//遥控器DR16类


//	串口缓存区 ------------------------------------------------------
uint8_t Uart1_Rx_Buff[USART1_RX_BUFFER_SIZE]; /*!< Receive buffer for Uart1 */
uint8_t Uart2_Rx_Buff[USART2_RX_BUFFER_SIZE]; /*!< Receive buffer for Uart2 */

//	<FreeRTOS> 队列定义	--------------------
QueueHandle_t USART_TxPort;					//	串口发送队列
QueueHandle_t CAN2_TxPort;					//	can2 发送队列
QueueHandle_t CAN2_RxPort;					//	can2 接收队列
QueueHandle_t DR16_QueueHandle;				//	dr16（串口） 接收队列

//	<FreeRTOS> 信号量定义 -----------------------------------

//	<FreeRTOS> 互斥量定义	--------------------
SemaphoreHandle_t DR16_mutex;				//	dr16互斥量

//	<FreeRTOS> 任务句柄定义 -------------------------------
TaskHandle_t DjiMotor_Handle;		// 电机控制任务
TaskHandle_t IMU_Handle;			//	MPU6050数据读取任务句柄
/*	通信任务	*/
TaskHandle_t CAN2Transmit_Handle;	// can2 发送任务
TaskHandle_t CAN2Receive_Handle;	// can2 接收任务

TaskHandle_t DR16_Handle;			// 遥控器dr16接收任务

/*	debug 任务	*/
TaskHandle_t Asuwave_Handle;		//网页版上位机任务

/* Function declarations -------------------------------------------------------*/

static void APP_MPU_Init();						//	MPU6050初始化

// <FreeRTOS> 任务声明 ---------------------------
void tskDjiMotor(void *arg);		//大疆电机控制任务函数
void tskIMU(void *arg);				//	MPU6050数据读取任务函数
/*	通信任务函数	*/
void tskCAN2Transmit(void *arg);	// can2发送任务函数
void tskCAN2Receive(void *arg);		// can2接收任务函数
void tskDR16(void *arg);			// dr16（串口）接收信息处理任务函数


// debug task
void tskAsuwave(void *arg);			//网页版上位机任务函数


//	注册回调函数声明	-----------------------------------------------------------
void   	 User_CAN2_RxCpltCallback(CAN_RxBuffer *CAN_RxMessage);						//can2接收回调函数
uint32_t User_UART2_RxCpltCallback(uint8_t *Recv_Data, uint16_t ReceiveLen);	//串口2接收回调函数

/* Function prototypes -------------------------------------------------------*/

/**
  * @brief  application device initialization task
  */
void App_Device_Init(void)
{
  /* Drivers Init */
	// timer init
  Timer_Init(&htim4, USE_HAL_DELAY);
  	// can init
  CAN_Init(&hcan2, User_CAN2_RxCpltCallback);
  	  	//CAN2 motor1
  	CAN_Filter_Mask_Config(&hcan2,
		CanFilter_14 | CanFifo_0 | Can_STDID | Can_DataType, 0x201, 0x3ff);
  		//CAN2 motor2
	CAN_Filter_Mask_Config(&hcan2,
		CanFilter_15 | CanFifo_0 | Can_STDID | Can_DataType, 0x202, 0x3ff);
		//CAN2 motor3
	CAN_Filter_Mask_Config(&hcan2,
		CanFilter_16 | CanFifo_0 | Can_STDID | Can_DataType, 0x205, 0x3ff);
	// uart init
  Uart_Init(&huart1, Uart1_Rx_Buff, USART1_RX_BUFFER_SIZE, asuwave_callback);
  Uart_Init(&huart2, Uart2_Rx_Buff, USART2_RX_BUFFER_SIZE, User_UART2_RxCpltCallback);


  /* Modules Init */
  APP_MPU_Init();
  	  //pid timer init
  myPIDTimer::getMicroTick_regist (Get_SystemTimer);
  	  //网页版上位机初始化
  asuwave_init(&huart1, xTaskGetTickCount);

}


/**
 * @brief application freertos init function.
 */
void App_Rtos_Init(void)
{
  /* Queue Init */
  CAN2_TxPort 		= xQueueCreate(4, sizeof(CAN_COB));
  CAN2_RxPort 		= xQueueCreate(4, sizeof(CAN_COB));
  USART_TxPort 		= xQueueCreate(2, sizeof(USART_COB));
  DR16_QueueHandle 	= xQueueCreate(2, sizeof(USART_COB));
  /* Semaphore Init */
  /* Mutex Init */
  DR16_mutex = xSemaphoreCreateMutex();

  /* Task Init */
  xTaskCreate(tskDjiMotor, 		"App.Motor",   Small_Stack_Size, NULL, PriorityAboveNormal, &DjiMotor_Handle);
  xTaskCreate(tskIMU,			"App.IMU",	   Small_Stack_Size, NULL, PriorityNormal,      &IMU_Handle);
  xTaskCreate(tskCAN2Transmit, 	"Com.CAN2_Tx", Small_Stack_Size, NULL, PriorityHigh, 		&CAN2Transmit_Handle);
  xTaskCreate(tskCAN2Receive, 	"Com.CAN2_Rx", Small_Stack_Size, NULL, PriorityHigh, 		&CAN2Receive_Handle);
  xTaskCreate(tskDR16, 			"App.DR16",    Small_Stack_Size, NULL, PriorityAboveNormal, &DR16_Handle);
  xTaskCreate(tskAsuwave,		"App.Asuwave", Small_Stack_Size, NULL, PriorityBelowNormal,  &Asuwave_Handle);

}

/**
 * @brief 		mpu6050初始化函数
 * @function	确定mpu6050 i2c 通信引脚；
 * @function	mpu6050按给定参数初始化
 */
static void APP_MPU_Init(){
	/*	配置MPU6050 I2C 引脚	*/
	MPU6050_Config_Pin(GPIOB,GPIO_PIN_6,GPIO_PIN_7);
	/*	配置MPU6050 参数  完成初始化	*/
	MPU6050_Init(&mpu_config,&dmp_config);
	/*	运行陀螺仪自检	*/
	MPU6050_run_self_test(0);
}
