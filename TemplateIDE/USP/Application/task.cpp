 /**
  ******************************************************************************
  * @file   task.cpp
  * @brief  freertos task running file.
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
#include "internal.h"

extern mpu_rec_s mpu_receive;
extern DR16_Classdef DR16;

/* Functions ------------------------------------------------------------------*/

/**
 * @brief 	任务清单
 *
 * @task4	tskCAN2Transmit	|	CAN2发送			|	-all	任意模式下
 * @task5	tskCAN2Receive	|	CAN2接收			|	-all	任意模式下
 * @task6	tskDR16			|	DR16接收			|	-0  	比赛模式下
 * @task8	tskAsuwave		|	网页版上位机		|	-debug	debug模式下
 */

/**
 * @brief <freertos> 大疆电机控制任务
 */
void tskDjiMotor(void *arg){
	/*	pre load for task	*/
	static Motor_CAN_COB Tx_Buff;
	for(;;){
		/* wait for next circle */
		vTaskDelay(1);
		/*	电机控制	*/

		/*	将电机输出数据打包成can消息队列	*/

//		Tx_Buff =  MotorMsgPack(Tx_Buff,pitchmotor,		//	pitch轴电机
//										yawmotor);		//	yaw轴电机

		//	发送can队列，根据电机的发射帧id选择需要发送的数据包
		xQueueSend(CAN2_TxPort,&Tx_Buff.Id200,0);
		xQueueSend(CAN2_TxPort,&Tx_Buff.Id1ff,0);
	}
}


/**
 * @brief MPU6050读取数据
 */
void tskIMU(void *arg){
	/* Pre-Load for task */
	  TickType_t xLastWakeTime_t;
	  xLastWakeTime_t = xTaskGetTickCount();
	for(;;){
		/* wait for next circle */
		vTaskDelayUntil(&xLastWakeTime_t, 5);
			/*	读取MPU6050数据	*/
			dmp_read_data(&mpu_receive);
	}
}


/*
 * can2 transmit
 */
void tskCAN2Transmit(void *arg)
{
  /* Cache for Task */
  uint8_t free_can_mailbox;
  static CAN_COB CAN_TxMsg;
  /* Pre-Load for task */

  /* Infinite loop */

  for (;;)
  {
    /* CAN1 Send Port */
    if (xQueueReceive(CAN2_TxPort, &CAN_TxMsg, portMAX_DELAY) == pdPASS)
    {
      do
      {
        free_can_mailbox = HAL_CAN_GetTxMailboxesFreeLevel(&hcan2);
      } while (free_can_mailbox == 0);
      CANx_SendData(&hcan2, CAN_TxMsg.ID, CAN_TxMsg.Data, CAN_TxMsg.DLC);
    }
  }
}


/*
 * can2 receive
 */
void tskCAN2Receive(void *arg)
{
  static CAN_COB CAN_RxCOB;

  /* Infinite loop */
  for (;;)
  {
    /* update motor data from CAN1_RxPort */
    if (xQueueReceive(CAN2_RxPort, &CAN_RxCOB, portMAX_DELAY) == pdPASS)
    {
    	//跟新电机数据，如
//    	if (this->pitchmotor.CheckID(CAN_RxCOB->ID))
//    	{
//    		this->pitchmotor.update(CAN_RxCOB->Data);
//    	}
//    	else if (this->yawmotor.CheckID(CAN_RxCOB->ID))
//    	{
//    		this->yawmotor.update(CAN_RxCOB->Data);
//    	}
    }
  }
}


void tskDR16(void *arg)
{
  /* Cache for Task */
  static USART_COB Rx_Package;
  /* Pre-Load for task */
  DR16.Check_Link(xTaskGetTickCount());
  /* Infinite loop */
  for (;;)
  {
    /* Enter critical */
    xSemaphoreTake(DR16_mutex, portMAX_DELAY);
    /*	等待数据	*/
	if (xQueueReceive(DR16_QueueHandle, &Rx_Package, 100) == pdPASS)
	{
	  // Read Message
	  DR16.DataCapture((DR16_DataPack_Typedef*) Rx_Package.address);
	}
	/*	检测遥控器连接 */
    DR16.Check_Link(xTaskGetTickCount());
    /*	判断是否连接 	 */
    if(DR16.GetStatus() != DR16_ESTABLISHED )
    {
    	/**
		 * lost the remote control
		 */

    	/* Leave critical */
    	xSemaphoreGive(DR16_mutex);
		continue;
    }
    /*	更新遥控器控制	*/

    /* Leave critical */
    xSemaphoreGive(DR16_mutex);
  }
}


/**
  * @brief  debug task
  */
void tskAsuwave(void *arg)
{
  /* Cache for Task */

  /* Pre-Load for task */
  TickType_t xLastWakeTime_t;
  xLastWakeTime_t = xTaskGetTickCount();

  /* Infinite loop */
  for (;;)
  {
    /* Wait for the next cycle */
    vTaskDelayUntil(&xLastWakeTime_t, 10);

    asuwave_subscribe();
  }
}

