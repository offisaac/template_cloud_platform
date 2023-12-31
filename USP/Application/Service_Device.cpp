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
#include "UpperMonitor.h"
/* Private define ------------------------------------------------------------*/
TaskHandle_t DjiMotor_Handle;		
TaskHandle_t IMU_Handle;		
TaskHandle_t DR16_Handle;
TaskHandle_t UpperMonitor_Handle;
TaskHandle_t CloudPlatform_Handle;
TaskHandle_t Dial_Handle;
TaskHandle_t Dial_Con_Send_Handle;
TaskHandle_t Fric_Handle;
/* Private function declarations ---------------------------------------------*/
void tskDjiMotor(void *arg);
void tskIMU(void *arg);
void tskDR16(void *arg);
void tskUpperMonitor(void *arg);
void tskCloudPlatform(void *arg);
void tskDial(void *arg);
void tskDial_Con_Send(void *arg);
void tskFric(void *arg);
/* Function prototypes -------------------------------------------------------*/
/**
* @brief  Initialization of device management service
* @param  None.
* @return None.
*/

void Service_Devices_Init(void)
{
  xTaskCreate(tskDjiMotor, 	"App.Motor",   Small_Stack_Size, NULL, PriorityAboveNormal, &DjiMotor_Handle);
	#if  USE_SRML_MPU6050
  xTaskCreate(tskIMU,				"App.IMU",	   Small_Stack_Size, NULL, PriorityNormal,      &IMU_Handle);
	#endif
  xTaskCreate(tskDR16, 			"App.DR16",    Small_Stack_Size, NULL, PriorityAboveNormal, &DR16_Handle);
	xTaskCreate(tskUpperMonitor, 	"App.UpperMonitor",    Small_Stack_Size, NULL, PriorityAboveNormal, &UpperMonitor_Handle);
	xTaskCreate(tskCloudPlatform, 			"App.CloudPlatform",    Small_Stack_Size, NULL, PriorityAboveNormal, &CloudPlatform_Handle);
	xTaskCreate(tskDial, 			"App.Dial",    Small_Stack_Size, NULL, PriorityAboveNormal, &Dial_Handle);
	xTaskCreate(tskDial_Con_Send, 	"App.Dial_Con_Send",   Small_Stack_Size, NULL, PriorityAboveNormal, &Dial_Con_Send_Handle);
	xTaskCreate(tskFric, 			"App.Fric",    Small_Stack_Size, NULL, PriorityAboveNormal, &Fric_Handle);
}
void tskUpperMonitor(void *arg)
{
	/* Pre-Load for task */
	for(;;){
vTaskDelay(1);
Sent_Contorl(&huart4);
	}
}

void tskCloudPlatform(void *arg)
{
	/* Pre-Load for task */
	for(;;){
vTaskDelay(1);
static Motor_CAN_COB Tx_Buff;
if(DR16.GetStatus())
		{
//		 PID_Yaw_Angle.Target=180*DR16.GetCh2();
//		 PID_Yaw_Angle.Current=Yaw.getAngle();
//		 PID_Yaw_Speed.Target=PID_Yaw_Angle.Adjust();
//		 PID_Yaw_Speed.Current=Yaw.getSpeed();
//		 Yaw.Out=PID_Yaw_Speed.Adjust();
//		 PID_Pitch_Angle.Target=-25;
//		 PID_Pitch_Angle.Current=Pitch.getAngle();
//		 PID_Pitch_Speed.Target=PID_Pitch_Angle.Adjust();
//		 PID_Pitch_Speed.Current=Pitch.getSpeed();
		 Pitch.Out=30000;
		}
     else
		{
		 Yaw.Out=0;
		 Pitch.Out=0;
		}
     Tx_Buff = MotorMsgPack(Tx_Buff,Yaw);
		xQueueSend(CAN2_TxPort,&Tx_Buff.Id2ff,0);//Yaw
		Tx_Buff = MotorMsgPack(Tx_Buff,Pitch);
		xQueueSend(CAN1_TxPort,&Tx_Buff.Id1ff,0);//Pitch
	}
}

void tskDial_Con_Send(void *arg)
{
	/* Pre-Load for task */
	for(;;){
vTaskDelay(1000/fre);
static Motor_CAN_COB Tx_Buff;
if(DR16.GetStatus())
		{
		if(DR16.GetS1()==2)//上单发 下连发 均开启pid
			{
				angle_set-=1296;
			}
		}
     Tx_Buff = MotorMsgPack(Tx_Buff,Dial);
		xQueueSend(CAN1_TxPort,&Tx_Buff.Id200,0);
	}
}
void tskDial(void *arg)
{
	/* Pre-Load for task */
	for(;;){
vTaskDelay(1);
static Motor_CAN_COB Tx_Buff;
if(DR16.GetStatus())
		{
			if((DR16.GetS1()==1)||(DR16.GetS1()==2))//上单发 下连发 均开启pid
			{
		 PID_Dial_Angle.Target=angle_set;
		 PID_Dial_Angle.Current=Dial.getAngle();
		 PID_Dial_Speed.Target=PID_Dial_Angle.Adjust();
		 PID_Dial_Speed.Current=Dial.getSpeed();
		 Dial.Out=PID_Dial_Speed.Adjust();
		 change_flag=1;
			}
			else//值为3 即中间时
		{
			if(change_flag==1){//从上拨到下时才会更改
				change_flag=0;
			  angle_set-=1296;
			}
		 PID_Dial_Speed.Target=0;
		 PID_Dial_Speed.Current=Dial.getSpeed();
		 Dial.Out=PID_Dial_Speed.Adjust();
		}
		}
		else//关闭遥控
		{
		 PID_Dial_Speed.Target=0;
		 PID_Dial_Speed.Current=Dial.getSpeed();
		 Dial.Out=PID_Dial_Speed.Adjust();
		}
     Tx_Buff = MotorMsgPack(Tx_Buff,Dial);
		xQueueSend(CAN1_TxPort,&Tx_Buff.Id200,0);
	}
}
void tskFric(void *arg)
{
	/* Pre-Load for task */
	for(;;){
vTaskDelay(1);
static Motor_CAN_COB Tx_Buff;
if(DR16.GetStatus())
		{
		if(DR16.GetS2()==1)
		{
		 PID_L_Fric_Speed.Target=6000;PID_R_Fric_Speed.Target=6000;
		 PID_L_Fric_Speed.Current=L_Fric.getSpeed();PID_R_Fric_Speed.Current=R_Fric.getSpeed();
		 L_Fric.Out=PID_L_Fric_Speed.Adjust();
		 R_Fric.Out=PID_R_Fric_Speed.Adjust();//左右轮的物理性质差别很大 给同一个电压值时差别超过20%
		}
		else//遥控器开启状态下 关闭摩擦轮
		{
		 PID_L_Fric_Speed.Target=0;PID_R_Fric_Speed.Target=0;
		 PID_L_Fric_Speed.Current=L_Fric.getSpeed();PID_R_Fric_Speed.Current=R_Fric.getSpeed();
		 L_Fric.Out=PID_L_Fric_Speed.Adjust();
		 R_Fric.Out=PID_R_Fric_Speed.Adjust();
		}
		}
     else//遥控器断开 关闭摩擦轮
		{
		 PID_L_Fric_Speed.Target=0;PID_R_Fric_Speed.Target=0;
		 PID_L_Fric_Speed.Current=L_Fric.getSpeed();PID_R_Fric_Speed.Current=R_Fric.getSpeed();
		 L_Fric.Out=PID_L_Fric_Speed.Adjust();
		 R_Fric.Out=PID_R_Fric_Speed.Adjust();
		}
     Tx_Buff = MotorMsgPack(Tx_Buff,R_Fric,L_Fric);
		xQueueSend(CAN1_TxPort,&Tx_Buff.Id200,0);//左摩擦轮
		xQueueSend(CAN1_TxPort,&Tx_Buff.Id1ff,0);//右摩擦轮
	}
}
/**
 * @brief <freertos> 大疆电机控制任务
 */
void tskDjiMotor(void *arg)
{
	/*	pre load for task	*/
	for(;;){
		/* wait for next circle */
		vTaskDelay(1);
		/*	电机控制	*/

		/*	将电机输出数据打包成can消息队列	*/
		
		//	发送can队列，根据电机的发射帧id选择需要发送的数据包
//		xQueueSend(CAN2_TxPort,&Tx_Buff.Id1ff,0);
//		xQueueSend(CAN2_TxPort,&Tx_Buff.Id200,0);
//		xQueueSend(CAN2_TxPort,&Tx_Buff.Id2ff,0);
//		xQueueSend(CAN1_TxPort,&Tx_Buff.Id1ff,0);
//		xQueueSend(CAN1_TxPort,&Tx_Buff.Id200,0);
//		xQueueSend(CAN1_TxPort,&Tx_Buff.Id2ff,0);

//    MotorMsgSend(&hcan2, Yaw);这个是直接使用包装的can函数 跳过队列
	}
}

#if  USE_SRML_MPU6050
/**
 * @brief MPU6050读取数据
 */
void tskIMU(void *arg)
{
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
#endif

/**
	*	@brief	Dr16 data receive task
	*/
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


