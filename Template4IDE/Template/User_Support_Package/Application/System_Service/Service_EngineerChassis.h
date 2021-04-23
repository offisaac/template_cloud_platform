/**
  **************************************************************************************
  * @file   : Service_CrossSlider.h
  * @brief  : Header for Service_CrossSlider.cpp
  * @author ：Mentos_Seetoo
  * @date   ：February,2019
  * @version：1.0
  ***************************************************************************************
**/

#ifndef  _SERVICE_CROSS_SLIDER_H_
#define  _SERVICE_CROSS_SLIDER_H_
#include <System_DataPool.h>
#include "Engineer.h"
/*------------------------------ System Handlers ------------------------------*/
#ifdef  __cplusplus
extern CEngineer Engineer_Master;
#endif

#ifdef  __cplusplus
extern "C"{
#endif
  
/* Center Control ------------------------*/
extern TaskHandle_t EngineerControl_Handle;
void   Task_EngineerControl(void *arg);

/*------------------------------Function prototypes ---------------------------*/
#ifdef BACK_UP_VERSION
  #define Look_Down()     {TIM3->CCR3 = 1750;}          //1400,1720   新车，旧车
  #define Look_Forward()  {TIM3->CCR3 = 2470;}          //2350,2420   新车，旧车
#else 
  #define Look_Down()     {TIM3->CCR3 = 1250;}          //1400,1720   新车，旧车
  #define Look_Forward()  {TIM3->CCR3 = 2350;}          //2350,2420   新车，旧车
#endif
void Service_EngineerChassis_Init(void);
  
#ifdef  __cplusplus
}
#endif
#endif  

/************************* COPYRIGHT SCUT-ROBOTLAB *****END OF FILE****************/
