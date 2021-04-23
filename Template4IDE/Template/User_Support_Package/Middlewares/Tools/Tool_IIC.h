/**
  ******************************************************************************
  * @file   Tool_IIC.h
  * @author mannychen (270730634@qq.com)
  * @brief  Header file of Tool_IIC.cpp.需要位带操作的宏定义
  ******************************************************************************
  */

#ifndef _TOOL_IIC_H
#define _TOOL_IIC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/  
#include "includes.h"
     
/* Private  ------------------------------------------------------------------*/  
#define HAL_LIB
#define delay_us delay_us_nos

/* Exported ------------------------------------------------------------------*/   
typedef struct 
{
  GPIO_PinState (* xREAD_SDA)(void);      /* 读SDA电平 */
  void (* xSDA_IN)();                     /* SDA io口输入设置 */
  void (* xSDA_OUT)();                    /* SDA io口输入设置 */
  void (* xIIC_SCL)(const char x);        /* SCL io输出高低电平 */
  void (* xIIC_SDA)(const char x);        /* SDA io输出高低电平 */
}IIC_HandleTypeDef;

//IIC所有操作函数
void IIC_Init(IIC_HandleTypeDef *hiic);                                           
void IIC_Start(IIC_HandleTypeDef *hiic);                                 //发送IIC开始信号
void IIC_Stop(IIC_HandleTypeDef *hiic);	                                 //发送IIC停止信号
void IIC_Send_Byte(IIC_HandleTypeDef *hiic,unsigned char txd);           //IIC发送一个字节
unsigned char IIC_Read_Byte(IIC_HandleTypeDef *hiic,unsigned char ack);  //IIC读取一个字节
unsigned char IIC_Wait_Ack(IIC_HandleTypeDef *hiic);                     //IIC等待ACK信号
void IIC_Ack(IIC_HandleTypeDef *hiic);                                   //IIC发送ACK信号
void IIC_NAck(IIC_HandleTypeDef *hiic);                                  //IIC不发送ACK信号

  
#ifdef __cplusplus
}
#endif

#endif
/************************ COPYRIGHT SCUT-ROBOTLAB *****END OF FILE*************/
















