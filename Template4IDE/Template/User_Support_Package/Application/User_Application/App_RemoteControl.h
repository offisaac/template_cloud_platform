/**
  **********************************************************************************
  * @file   : App_RemoteControl.h
  * @brief  : Header for App_RemoteControl.h.
  * @author : Mentos_Seetoo
  * @date   : July,2019
  **********************************************************************************
  *   
**/
#ifndef  _REMOTE_CONTROL_H_
#define  _REMOTE_CONTROL_H_
#include <System_DataPool.h>

#ifdef  __cplusplus

#endif
#ifdef  __cplusplus
extern "C"{
#endif

/*------------------------------ System Handlers ------------------------------*/
extern TaskHandle_t RemoteControl_Handle;

/*------------------------------Function prototypes ---------------------------*/
static inline void ControllerMode(void);
static inline void KeyBoardMode(void);
static inline void KeyBoardMode_Mouse_Left(void);
static inline void KeyBoardMode_Mouse_Right(void);
static inline void KeyBoardMode_Shift_Ctrl(void);
static inline void KeyBoardMode_Shift(void);
static inline void KeyBoardMode_Ctrl(void);
static inline void KeyBoardMode_Normal(void);
void Task_RemoteControl(void *arg);

#ifdef  __cplusplus
}
#endif
#endif 
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
