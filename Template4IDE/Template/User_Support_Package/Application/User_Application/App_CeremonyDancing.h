/**
  **********************************************************************************
  * @file   : App_CeremonyDancing.h
  * @brief  : Header for App_CeremonyDancing.cpp.
  * @author : Mentos_Seetoo
  * @date   : September,2019
  **********************************************************************************
  *   
**/
#ifndef  _DANCE_CONTROL_H_
#define  _DANCE_CONTROL_H_
#include <System_DataPool.h>

#ifdef  __cplusplus

#endif
#ifdef  __cplusplus
extern "C"{
#endif


/*------------------------------ System Handlers ------------------------------*/
extern TaskHandle_t DanceCtrl_Handle;
enum _TRANS_STEP_
{
    TRANS_WAIT = 0U,
    TRANS_STEP1,
    TRANS_STEP2,
    TRANS_STEP3,
    TRANS_STEP4,
    TRANS_STEP5,
    TRANS_END
};

enum _DANCE_STEP
{
    DANCE_WAIT = 0U,
    DANCE_STEP1,
    DANCE_STEP2,
    DANCE_STEP3,
    DANCE_STEP4,
    DANCE_STEP5,
    DANCE_STEP6,
    DANCE_STEP7,
    DANCE_STEP8,
    DANCE_STEP9,
    DANCE_STEP10,
    DANCE_END
};

#define ROUND_POSITIVE  (1.0f)
#define ROUND_NEGATIVE  (-1.0f)
/*------------------------------Function prototypes ---------------------------*/
void DancingMode(void);
void ControllerMode(void);
void Task_DanceInRemote(void *arg);

#ifdef  __cplusplus
}
#endif
#endif 
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
