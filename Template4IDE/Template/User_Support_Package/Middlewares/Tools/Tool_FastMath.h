/**
  **********************************************************************************
  * @file   : Tool_FastMath.h
  * @brief  : Header for Tool_FastMath.cpp
  * @author ：Manny Chen
  * @date   ：February,2019
  * @version：1.0
  **********************************************************************************
**/
#ifndef  _TOOL_FASTMATH_H_
#define  _TOOL_FASTMATH_H_

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>

#ifdef  __cplusplus
extern "C"{
#endif
/* Private  ------------------------------------------------------------------*/
static float half_interval_search(float *m_data_buf, uint16_t m_len, float m_data);
  
/* Exported ------------------------------------------------------------------*/
float FastMath_Arctan(float data);
  
  
  
#ifdef  __cplusplus
}
#endif
#endif
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
