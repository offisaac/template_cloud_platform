/**
******************************************************************************
* @file   User_Task.cpp
* @brief  User task running file. Please add your task in this file, and create
*         it in "System_Config.cpp"
******************************************************************************
* @note
*  - Before running your task, just do what you want ~ 
*  - More devices or using other classification is decided by yourself ~ 
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
#include "iostream"
#include "inputInterface.h"
#include "User_Task.h"
#include "simulation.h"
#include "UpperMonitor.h"
#include "win32_support.h"

/* Private define ------------------------------------------------------------*/
TaskHandle_t RobotCtrl_Handle;
TaskHandle_t DataDisplay_Handle;

/* Private variables ---------------------------------------------------------*/
/* Private function declarations ---------------------------------------------*/
void Tsak_RobotCtrl(void *arg);
void Task_DataDisplay(void *arg);

/* Exported devices ----------------------------------------------------------*/
/* Motor & ESC & Other actuators*/

/* Remote control */

/* IMU & NUC & Other sensors */

/* Other boards */

/* Function prototypes -------------------------------------------------------*/
/**
* @brief  Initialization of Tasks
* @param  None.
* @return None.
*/
void User_Tasks_Init(void)
{
  xTaskCreate(Tsak_RobotCtrl, "Robot Control", Huge_Stack_Size, NULL, PrioritySuperHigh, &RobotCtrl_Handle);
  xTaskCreate(Task_DataDisplay, "Data Display", Huge_Stack_Size, NULL, PrioritySuperHigh, &DataDisplay_Handle);
}


/**
* @brief  User can run all your robot control code in this task.
* @param  None.
* @return None.
*/
void Tsak_RobotCtrl(void *arg)
{ 
  /* Cache for Task */
  TickType_t xLastWakeTime_t;
  const TickType_t xBlockTime = pdMS_TO_TICKS(5);
  /* Pre-Load for task */
  xLastWakeTime_t     = xTaskGetTickCount();
  (void)arg;

  /* Infinite loop */
  for (;;)
  {
    //if (Body != NULL)
    //  SysLog.Record(_INFO_, "CoppeliaSim", "Orientation_3f: %f,%f,%f", Body->obj_Data.orientation_3f[0], Body->obj_Data.orientation_3f[1], Body->obj_Data.orientation_3f[2]);
    //else {};

    //if( Signal != NULL)
    //  SysLog.Record(_INFO_, "CoppeliaSim", "Signal: %f", Signal->data);
   mouse.resolveVelocity(5);
   mouse.setExitFlag(false);
   std:: cout << "Mouse speed :" << mouse.velocity.vx << "," << mouse.velocity.vy << std::endl;
    /* Pass control to the next task */
    vTaskDelayUntil(&xLastWakeTime_t, xBlockTime);
  }
}


/**
* @brief  User can collect and show datas in a lower frequency.
* @param  None.
* @return None.
*/
void Task_DataDisplay(void *arg)
{
  /* Cache for Task */
  _Unified_COB UpperMonitor_COB = {NULL,39};
  const TickType_t xBlockTime = pdMS_TO_TICKS(10);
  TickType_t xLastWakeTime_t;

  /* Pre-Load for task */
  xLastWakeTime_t = xTaskGetTickCount();
  (void)arg;

  /* Infinite loop */
  for (;;)
  {
    UpperMonitor_COB.pData = Get_SendPackage();

    if(UpperMonitor_COB.pData != NULL)
      while (!SerialPort.BuffSend(UpperMonitor_COB.pData, UpperMonitor_COB.len)) {};
    /* Pass control to the next task */
    vTaskDelayUntil(&xLastWakeTime_t, xBlockTime);
  }
}
/* User Code End Here ---------------------------------*/

/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
