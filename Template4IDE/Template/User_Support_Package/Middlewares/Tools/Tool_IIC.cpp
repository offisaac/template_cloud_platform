/**
  ******************************************************************************
  * @file   Tool_IIC.cpp
  * @author mannychen (270730634@qq.com)
  * @brief  iic 底层驱动程序
  * @date   2018-10-15
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
  1)包含myiic.h,调用接口函数即可，其中的延时函数用的是阻塞式的延时
  */
/* Includes ------------------------------------------------------------------*/
#include "Tool_IIC.h"
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  iic 初始化 
  * @param  IIC_HandleTypeDef *hiic 连接陀螺仪的iic句柄
  * @retval void
  */
void IIC_Init(IIC_HandleTypeDef *hiic)
{ 
    /* SCL拉高 */  
    hiic->xIIC_SCL(1);
  
    /* SDA拉高 */  
    hiic->xIIC_SDA(1);
}


/**
  * @brief  开始信号
  * @param  IIC_HandleTypeDef *hiic 连接陀螺仪的iic句柄
  * @retval void
  */
void IIC_Start(IIC_HandleTypeDef *hiic)
{  
    hiic->xSDA_OUT();   //sda线输出
    hiic->xIIC_SDA(1);  
    hiic->xIIC_SCL(1);
    delay_us(1);
    hiic->xIIC_SDA(0);  //START:when CLK is high,DATA change form high to low
    delay_us(1);
    hiic->xIIC_SCL(0);  //钳住I2C总线，准备发送或接收数据 
}

/**
  * @brief  停止信号
  * @param  IIC_HandleTypeDef *hiic 连接陀螺仪的iic句柄
  * @retval void
  */
void IIC_Stop(IIC_HandleTypeDef *hiic)
{ 
    hiic->xSDA_OUT();   //sda线输出
    hiic->xIIC_SCL(0);  
    hiic->xIIC_SDA(0);  //STOP:when CLK is high DATA change form low to high
    delay_us(1);
    hiic->xIIC_SCL(1);  
    delay_us(1);
    hiic->xIIC_SDA(1);  //发送I2C总线结束信号
}

/**
  * @brief  等待应答信号
  * @param  IIC_HandleTypeDef *hiic 连接陀螺仪的iic句柄
  * @retval 1，接收应答失败  
  *         0，接收应答成功
  */
unsigned char IIC_Wait_Ack(IIC_HandleTypeDef *hiic)
{
  unsigned char ucErrTime=0;

  hiic->xSDA_IN();   //SDA设置成输入
  hiic->xIIC_SDA(1);
  
  delay_us(1);
  hiic->xIIC_SCL(1);
  delay_us(1);
  
  while(hiic->xREAD_SDA())
  {
    ucErrTime++;
    if(ucErrTime>250)
    {
      IIC_Stop(hiic);
      return 1;
    }
  }
  
  hiic->xIIC_SCL(0);
  return 0;
}

/**
  * @brief  产生ACK应答
  * @param  IIC_HandleTypeDef *hiic 连接陀螺仪的iic句柄
  * @retval void
  */
void IIC_Ack(IIC_HandleTypeDef *hiic)
{
    hiic->xIIC_SCL(0); 
    hiic->xSDA_OUT();   
    hiic->xIIC_SDA(0);  
    delay_us(1);
    hiic->xIIC_SCL(1);  
    delay_us(1);
    hiic->xIIC_SCL(0);  
}

/**
  * @brief  不产生ACK应答
  * @param  IIC_HandleTypeDef *hiic 连接陀螺仪的iic句柄
  * @retval void
  */
void IIC_NAck(IIC_HandleTypeDef *hiic)
{  
  hiic->xIIC_SCL(0); 
  hiic->xSDA_OUT();   
  hiic->xIIC_SDA(1);  
  delay_us(1);
  hiic->xIIC_SCL(1);  
  delay_us(1);
  hiic->xIIC_SCL(0);  
}

/**
  * @brief  IIC发送一个字节
  * @param  IIC_HandleTypeDef *hiic 连接陀螺仪的iic句柄
  *         unsigned char txd       发送内容 
  * @retval 从机有无应答
  *         1，有应答
  *         0，无应答
  */
void IIC_Send_Byte(IIC_HandleTypeDef *hiic,unsigned char txd)
{
  unsigned char t;
  
  hiic->xSDA_OUT();  
  hiic->xIIC_SCL(0);
  for(t=0;t<8;t++)
  {
    hiic->xIIC_SDA((txd&0x80)>>7);
    txd<<=1;
    delay_us(1);   //对TEA5767这三个延时都是必须的
    hiic->xIIC_SCL(1);
    delay_us(1);
    hiic->xIIC_SCL(0);
    delay_us(1);
  }
}

/**
  * @brief  读1个字节，ack=1时，发送ACK，ack=0，发送nACK
  * @param  IIC_HandleTypeDef *hiic 连接陀螺仪的iic句柄
  *         unsigned char ack       发送应答或者不应答，0 不应答；1 应答
  * @retval void
  */
unsigned char IIC_Read_Byte(IIC_HandleTypeDef *hiic,unsigned char ack)
{
  unsigned char i,receive=0;
    
  /* SDA设置成为输入 */
  hiic->xSDA_IN();
  
  for(i=0;i<8;i++ )
  {
    hiic->xIIC_SCL(0);
    delay_us(1);
    hiic->xIIC_SCL(1);
    receive<<=1;
    if(hiic->xREAD_SDA())
      receive++;
    delay_us(1);
  }
  if(!ack)
  {
    /* 发送nACK */
    IIC_NAck(hiic);
  }
  else 
  {
    /* 发送ACK */
    IIC_Ack(hiic);
  }
  /* 返回接收到的字节 */
  return receive;
}

