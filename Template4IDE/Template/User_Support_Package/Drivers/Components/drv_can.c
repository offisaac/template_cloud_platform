/**
  ******************************************************************************
  * Copyright (c) 2019 - ~, SCUT-RobotLab Development Team
  * @file    drv_can.c
  * @author  Mentos_Seetoo 1356046979@qq.com
  * @brief   Code for CAN driver in STM32F4 using STM32Cube_FW_F4_V1.24.0.
  * @date    2019-06-12
  * @version 1.0
  * @par Change Log：
  * <table>
  * <tr><th>Date        <th>Version  <th>Author    		<th>Description
  * <tr><td>2019-06-12  <td> 1.0     <td>Mentos Seetoo  <td>Creator
  * </table>
  *
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
	@note
		-# 把要用的CAN宏定义置1，其余置0:\n
		   CAN1_ON_BOARD, CAN2_ON_BOARD
		-# 调用CAN_Init()，初始化CAN。
		-# 调用CAN_rx_callback_register 设置CAN接收处理函数的指针。
		-# 在需要用到发送的部分直接调用CANx_SendData()函数。

	@warning
		-# 本模块只能保存一条来自于同一个FIFO的消息(详细见下方HAL库FIFOx中断的
		   实现),请注意及时读走消息。
		-# 由于本模块运行基于STM32CubeMX生成的代码，所以头文件extern部分常用固定的结构体
		   名称,也方便其他地方调用。

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
#include "drv_can.h"
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static void (*pCAN1_RxCpltCallback)(CAN_RxBuffer *);
static void (*pCAN2_RxCpltCallback)(CAN_RxBuffer *);
/* Private type --------------------------------------------------------------*/

/* Private function declarations ---------------------------------------------*/

/* function prototypes -------------------------------------------------------*/

/**
* @brief  Initialize CAN Bus
* @param  None
* @retval None
*/
void CAN_init(void)
{
  #if (CAN1_ON_BOARD == 1)
    HAL_CAN_Start(&hcan1);
    __HAL_CAN_ENABLE_IT(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);
    __HAL_CAN_ENABLE_IT(&hcan1,CAN_IT_RX_FIFO1_MSG_PENDING);
  #endif
  
  #if (CAN2_ON_BOARD == 1)
    HAL_CAN_Start(&hcan2);
    __HAL_CAN_ENABLE_IT(&hcan2,CAN_IT_RX_FIFO0_MSG_PENDING);
    __HAL_CAN_ENABLE_IT(&hcan2,CAN_IT_RX_FIFO1_MSG_PENDING);
  #endif
}

/**
* @brief  Registered user callback function
* @param  hcan: CANx created by CubeMX.
* @param  pFunc: user callback function
* @retval SUCCESS/ERROR
*/
uint8_t CAN_rx_callback_register(CAN_HandleTypeDef* hcan, void (*pFunc)(CAN_RxBuffer*))
{
#if (CAN1_ON_BOARD == 1)
  if(hcan == &hcan1)
  {
    pCAN1_RxCpltCallback = pFunc;
    return SUCCESS;
  }
#endif
#if (CAN1_ON_BOARD == 2)
  if(hcan == &hcan2)
  {
    pCAN2_RxCpltCallback = pFunc;
    return SUCCESS;
  }
#endif
  return ERROR;
}

/**
 * @brief Can_Filter_Mask_Config 配置CAN的筛选器
 * @details 筛选器:|编号|FIFOx|ID类型|帧类型|ID|屏蔽位(0x3ff,0x1FFFFFFF)| \n
 * 			老队长留下来的遗产，望各位珍惜hhh.
 * @param object_para: |编号|FIFOx|ID类型|帧类型|ID|
 * @param Id: ID
 * @param MaskId: 屏蔽位(0x3ff,0x1FFFFFFF)
 * @retval None
 */
void CAN_Filter_Mask_Config(CAN_HandleTypeDef *_hcan, uint8_t object_para,uint32_t Id,uint32_t MaskId)
{
  CAN_FilterTypeDef  CAN_FilterInitStructure;
  /*判断是否为数据帧*/
  if( (object_para&0x02))
  {
    CAN_FilterInitStructure.FilterIdHigh         = Id<<3<<16;                       /* 掩码后ID的高16bit */
    CAN_FilterInitStructure.FilterIdLow          = Id<<3| ((object_para&0x03)<<1);  /* 掩码后ID的低16bit */
    CAN_FilterInitStructure.FilterMaskIdHigh     = MaskId<<3<<16;                   /* ID掩码值高16bit */
    CAN_FilterInitStructure.FilterMaskIdLow      = MaskId<<3| ((object_para&0x03)<<1);;   /* ID掩码值低16bit */
  }
  else
  {
    CAN_FilterInitStructure.FilterIdHigh         = Id<<5;                           /* 掩码后ID的高16bit */
    CAN_FilterInitStructure.FilterIdLow          = ((object_para&0x03)<<1);         /* 掩码后ID的低16bit */
    CAN_FilterInitStructure.FilterMaskIdHigh     = MaskId<<5;                       /* ID掩码值高16bit */
    CAN_FilterInitStructure.FilterMaskIdLow      = ((object_para&0x03)<<1);;        /* ID掩码值低16bit */
  }
  CAN_FilterInitStructure.FilterBank           = object_para>>3;                    /* 滤波器序号，0-13，共14个滤波器 */
  CAN_FilterInitStructure.FilterFIFOAssignment = (object_para>>2)&0x01;             /* 滤波器绑定FIFO 0 */
  CAN_FilterInitStructure.FilterActivation     = ENABLE;                            /* 使能滤波器 */
  CAN_FilterInitStructure.FilterMode           = CAN_FILTERMODE_IDMASK;             /* 滤波器模式，设置ID掩码模式 */
  CAN_FilterInitStructure.FilterScale          = CAN_FILTERSCALE_32BIT;             /* 32位滤波 */
  CAN_FilterInitStructure.SlaveStartFilterBank = 14;                                /*Select the start bank filter for slave CAN*/
  
  HAL_CAN_ConfigFilter(_hcan, &CAN_FilterInitStructure);
}


/**
* @brief  通过CAN发送消息(数据帧)
  @param  hcan		  :使用的CAN线
  @param  ID		  :发送者ID
  @param  *pData	  :发送的数据
  @param  Len		  :数据长度
  @retval CAN_SUCCESS :发送成功
  @retval CAN_LINE_BUSY :发送失败
  @author Mentos
*/
uint8_t CANx_SendData(CAN_HandleTypeDef* hcan,uint16_t ID,uint8_t *pData,uint16_t Len)
{
  static CAN_TxHeaderTypeDef   Tx_Header;

  Tx_Header.StdId=ID;
  Tx_Header.ExtId=0;
  Tx_Header.IDE=0;
  Tx_Header.RTR=0;
  Tx_Header.DLC=Len;

  if(HAL_CAN_AddTxMessage(hcan,&Tx_Header,pData,(uint32_t*)CAN_TX_MAILBOX0)!=HAL_OK)
  {
    if(HAL_CAN_AddTxMessage(hcan,&Tx_Header,pData,(uint32_t*)CAN_TX_MAILBOX1)!=HAL_OK)
    {
      if(HAL_CAN_AddTxMessage(hcan,&Tx_Header,pData,(uint32_t*)CAN_TX_MAILBOX2)!=HAL_OK)
        return CAN_LINE_BUSY;
    }
  }
  return CAN_SUCCESS;
}


/*HAL库FIFO0中断*/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /*CAN接收缓冲区*/
  static CAN_RxBuffer CAN_RxBuffer;
  /* 转到用户回调函数 */
  if(hcan == &hcan1)
  {
    if(HAL_CAN_GetRxMessage(hcan,CAN_FILTER_FIFO0,&CAN_RxBuffer.header,CAN_RxBuffer.data)==HAL_ERROR){};
    pCAN1_RxCpltCallback(&CAN_RxBuffer);
  }
  else if(hcan == &hcan2)
  {
    if(HAL_CAN_GetRxMessage(hcan,CAN_FILTER_FIFO0,&CAN_RxBuffer.header,CAN_RxBuffer.data)==HAL_ERROR){};
    pCAN2_RxCpltCallback(&CAN_RxBuffer);
  }
}

/*HAL库FIFO1中断*/
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /*CAN接收缓冲区*/
  static CAN_RxBuffer CAN_RxBuffer;
  /* 转到用户回调函数 */
  if(hcan == &hcan1)
  {
    if(HAL_CAN_GetRxMessage(hcan,CAN_FILTER_FIFO1,&CAN_RxBuffer.header,CAN_RxBuffer.data)==HAL_ERROR){};
    pCAN1_RxCpltCallback(&CAN_RxBuffer);
    
  }
  else if(hcan == &hcan2)
  {
    if(HAL_CAN_GetRxMessage(hcan,CAN_FILTER_FIFO1,&CAN_RxBuffer.header,CAN_RxBuffer.data)==HAL_ERROR){};
    pCAN2_RxCpltCallback(&CAN_RxBuffer);
  }
}


/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
