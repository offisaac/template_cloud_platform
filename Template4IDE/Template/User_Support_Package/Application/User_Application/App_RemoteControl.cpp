/**
  **********************************************************************************
  * @file   : App_RemoteControl.cpp
  * @brief  : This program is for Robomaster Engineer Remote Control
  *           (Robomaster 2019 version 4.0).
  * @author : Mentos_Seetoo
  * @date   : July,2019
  **********************************************************************************
  *   
**/
#include "App_RemoteControl.h"
#include "Service_Communication.h"
#include "Service_Devices.h"
#include "Service_EngineerChassis.h"
#include "Tool_Filter.h"
#include "Filter.h"

/*Task Define---------------------------------------------------------------------*/
TaskHandle_t RemoteControl_Handle;        
static _Body_Operation_t* Body_Status;  //机器人当前动作状态
float  sensitivity_z = 2;               //Z轴灵敏度

/*Function Prototypes-------------------------------------------------------------*/
/*
  这个程序主要调节的运动参数有
  1.加减速度
  2.鼠标Yaw轴灵敏度
*/
/**
* @brief  Task for remote control.
*/
void Task_RemoteControl(void *arg)
{
  /* Cache for Task */
  uint8_t  temp;
  static COB_TypeDef RelayCmdBuf;
  
  /* Pre-Load for task */
  RelayCmdBuf.ID = 0x001;
  RelayCmdBuf.DLC= 8;
  memset(RelayCmdBuf.Data, 0 ,8);
  Engineer_Master.Switch_ActionMode(NORMAL);
  Body_Status = Engineer_Master.IO_Port(BODY_OPERATION_OUT);
  
  /* Infinite loop ----------------------------------------------------------*/
  TickType_t xLastWakeTime_Remote;
  xLastWakeTime_Remote = xTaskGetTickCount();
  for(;;)
  { 
    if(DR16.GetStatus() == Connection_Established)
    { 
      //S2拨到下面为键鼠模式，其他为手柄模式(因为目前用的遥控器S1是坏的~ QAQ )
      if(DR16.GetS2() != DOWN)
          ControllerMode();
      
      else
          KeyBoardMode();

    }
    //掉线保护
    else
    {
      Engineer_Master.Switch_ActionMode(CHASSIS_LOCK);
    }
    
    /* 
      发送继电器板需要的按键信息 
    */
    temp = SuperRelay_Obj.Get_Cmd();
    if(temp != RelayCmdBuf.Data[0])
    {
      RelayCmdBuf.Data[0] = temp;
      if(RelayCmdBuf.Data[0] != 0)
        xQueueSend(CAN2_TxPort,&RelayCmdBuf,0);
    }
    
    SuperRelay_Obj.Clear_Cmd();
   /* Pass control to the next task ------------------------------------------*/
    vTaskDelayUntil(&xLastWakeTime_Remote,10);
  }
}


/*------------------------------------------------------------------------------------------------*/
/**
* @brief   :手柄模式
 * @author :Mentos Seetoo (1356046979@qq.com)
 */
static inline void ControllerMode()
{ 
  Engineer_Master.Switch_ActionMode(NORMAL);
  //后视
  if(DR16.GetS2() == MID)
  {
    Engineer_Master.Manual_XYZ(-DR16.Get_LX_Norm(),
                               -DR16.Get_LY_Norm(),
                               DR16.Get_RX_Norm());
  }
  //正视
  else if(DR16.GetS2() == UP)
  {
    Engineer_Master.Manual_XYZ(DR16.Get_LX_Norm(),
                               DR16.Get_LY_Norm(),
                               DR16.Get_RX_Norm());
  }
}



/**
* @brief   :键鼠模式及其子模式
 * @author :Mentos_Seetoo
 */
static inline void KeyBoardMode()
{
  static float MouseX = 0;
  static float MouseX_SliderFliter[4] = {0};
  static LowPassFilter MouseX_LowPassFliter(0.7);
  static uint8_t Filter_index = 0;
  
  /*鼠标移动信息*/
  MouseX = MouseX_LowPassFliter.filter(MouseX);
  MouseX = Slider_Filter(MouseX_SliderFliter,&Filter_index,4,DR16.Get_MouseX_Norm());

  /*  
    鼠标点击信息 
  */
  if(DR16.IsKeyPress(_Mouse_L))
    KeyBoardMode_Mouse_Left();
  if(DR16.IsKeyPress(_Mouse_R))
    KeyBoardMode_Mouse_Right();
  else{
    if(Engineer_Master.Get_CurrentMode() == AUTO_ROTATE && Engineer_Master.Operations.auto_rotate_flag == CAT_WALK)
      Engineer_Master.Switch_ActionMode(NORMAL);
  }
  
  /* 
    按键信息 ：运动按键
  */
  static float temp_X_Co = 0, temp_Y_Co = 0;
  if(DR16.IsKeyPress(_W)) temp_Y_Co ++;
  if(DR16.IsKeyPress(_A)) temp_X_Co -= 0.8f;
  if(DR16.IsKeyPress(_S)) temp_Y_Co --;
  if(DR16.IsKeyPress(_D)) temp_X_Co += 0.8f;
  //设置手动输入目标
  Engineer_Master.Manual_XYZ(temp_X_Co, temp_Y_Co, sensitivity_z*MouseX);
  temp_Y_Co = temp_X_Co = 0;
  
  /* 
    按键信息 ：功能按键
  */
  //Shift + Crtl
  if(DR16.IsKeyPress(_SHIFT) && DR16.IsKeyPress(_CTRL))
      KeyBoardMode_Shift_Ctrl();
  //Shift
  else if(DR16.IsKeyPress(_SHIFT))
      KeyBoardMode_Shift();
  //Ctrl
  else if(DR16.IsKeyPress(_CTRL))
      KeyBoardMode_Ctrl();
  else
      KeyBoardMode_Normal();
 
}

/*------------------------------------------------------------------------------------------------*/
/* 鼠标点击 */
 uint32_t last_active_time, this_time;
static inline void KeyBoardMode_Mouse_Left()
{
  //用于视角切换延时

  this_time = xTaskGetTickCount();
  
  SuperRelay_Obj.Write_Cmd(Mouse_Left);
  if(Engineer_Master.Get_CurrentMode() == RESCUE){
    NUC_Obj.SetTargetMode(WATCH_FORWARD);
    Body_Status->rescue_flag = PULLING_OUT;
  }
  
  if(Engineer_Master.Get_CurrentMode() == NORMAL || Engineer_Master.Get_CurrentMode() == UP_STAIRS){
    //鼠标切换视角，连续两次操作时间间隔大于500ms
    if(this_time - last_active_time > 500)
    {
      last_active_time = this_time;
      NUC_Obj.SetTargetMode(WATCH_FORWARD);
      if(Engineer_Master.view_flag == FORWARD)Engineer_Master.view_flag = BACK_UPSTAIRS;
      else Engineer_Master.view_flag = FORWARD;
    }
    else
    {
      //忽略此次操作
    }
  }
  
  if(Engineer_Master.Get_CurrentMode() == GET_BULLET){
    //防止取弹过程没手动打断还在自动模式就取弹
    Body_Status->get_bullet_flag = MANUAL;
  }
  
  if(Engineer_Master.Get_CurrentMode() == AUTO_ROTATE)
  {
    //左键打断旋转
    Engineer_Master.Switch_ActionMode(NORMAL);
  }
}

static inline void KeyBoardMode_Mouse_Right()
{
  SuperRelay_Obj.Write_Cmd(Mouse_Right);
  if(Engineer_Master.Get_CurrentMode() == NORMAL){
    Engineer_Master.Switch_ActionMode(AUTO_ROTATE);
    Engineer_Master.Operations.auto_rotate_flag = CAT_WALK;
    Engineer_Master.cat_walk_flag = CAT_LEFT;
  }
  
  if(Engineer_Master.Get_CurrentMode() == GET_BULLET){
    //防止取弹过程没手动打断还在自动模式就取弹
    Body_Status->get_bullet_flag = MANUAL;
  }
}

/* Shift + Ctrl */
static inline void KeyBoardMode_Shift_Ctrl()
{
  static bool reboot_flag = 0;
  if(DR16.IsKeyPress(_G)){
    Engineer_Master.Switch_ActionMode(NORMAL);         //给弹动作结束
    SuperRelay_Obj.Write_Cmd(Ctrl_Shift_G);
  }
  
  if(DR16.IsKeyPress(_X)){
    Engineer_Master.Switch_ActionMode(GET_BULLET);     //取弹模式（岛上3个）
    NUC_Obj.SetTargetMode(ISLAND_LEFT);
    SuperRelay_Obj.Write_Cmd(Ctrl_Shift_X);
  }
  
  if(DR16.IsKeyPress(_B)){
      reboot_flag = 1;
    }
  else
  {
    if(reboot_flag == 1) 
    {
      Engineer_Master.Switch_ActionMode(NORMAL);         //复位
      NUC_Obj.SetTargetMode(RERUN);
      Engineer_Master.view_flag = FORWARD;
      SuperRelay_Obj.Write_Cmd(B);
      Engineer_Master.island_flag = 0;
      reboot_flag = 0;
    }
  }
}

static inline void KeyBoardMode_Shift()
{ 
  //shift按下时都为组合键
  Engineer_Master.gear_flag = SPEED_UP;                //加速
  
  if(DR16.IsKeyPress(_X)){
    Engineer_Master.Switch_ActionMode(GET_BULLET);     //取弹动作系列，单个对准
    SuperRelay_Obj.Write_Cmd(Shift_X);
    if(Engineer_Master.island_flag == 0)
      NUC_Obj.SetTargetMode(GROUND_MID);
    else
      NUC_Obj.SetTargetMode(ISLAND_MID);
  }
  if(DR16.IsKeyPress(_V)){                             //放卡弹药箱
    SuperRelay_Obj.Write_Cmd(Shift_V);
  }
  if(DR16.IsKeyPress(_G)){
    SuperRelay_Obj.Write_Cmd(Shift_G);                 //给弹动作退出
  }
  
  if(DR16.IsKeyPress(_Z)){
    if(Engineer_Master.Get_CurrentMode() == RESCUE)
    {
      Engineer_Master.Switch_ActionMode(NORMAL);         //救援模式结束
      SuperRelay_Obj.Write_Cmd(Shift_Z);
    }
  }
  
  if(DR16.IsKeyPress(_R)){
    Engineer_Master.Switch_ActionMode(UP_STAIRS);         //上一阶台阶
    Engineer_Master.up_stairs_time = ONEC;
  }
}

static inline void KeyBoardMode_Ctrl()
{ 
  //ctrl按下时都为组合键
  Engineer_Master.gear_flag = SLOW_DOWN;                //减速
  
  if(DR16.IsKeyPress(_C)){
    Engineer_Master.Switch_ActionMode(GET_BULLET);      //取弹模式手动上层动作
    SuperRelay_Obj.Write_Cmd(Ctrl_C);
  }
  if(DR16.IsKeyPress(_V)){
    Engineer_Master.Switch_ActionMode(GET_BULLET);      //取弹模式手动上层动作
    SuperRelay_Obj.Write_Cmd(Ctrl_V);
  }
  if(DR16.IsKeyPress(_B)){
    Engineer_Master.Switch_ActionMode(GET_BULLET);      //取弹模式手动上层动作
    SuperRelay_Obj.Write_Cmd(Ctrl_B);
  }
  if(DR16.IsKeyPress(_F)){
    Engineer_Master.Switch_ActionMode(GET_BULLET);      //取弹模式手动上层动作
    SuperRelay_Obj.Write_Cmd(Ctrl_F);
  }
  if(DR16.IsKeyPress(_G)){
    Engineer_Master.Switch_ActionMode(GET_BULLET);      //取弹模式手动上层动作
    SuperRelay_Obj.Write_Cmd(Ctrl_G);
  }
  
  if(Engineer_Master.Get_CurrentMode() == GET_BULLET)
  {
    //防止取弹过程没手动打断还在自动模式就取弹
    Body_Status->get_bullet_flag = MANUAL;
  }
}

static inline void KeyBoardMode_Normal()
{
  Engineer_Master.gear_flag = NO_CHANGE;
  
  if(DR16.IsKeyPress(_X)){
    Engineer_Master.Switch_ActionMode(GET_BULLET);      //取弹动作系列，对准（岛下5个以上）
    //切换进入视觉对准，第一次对左边
    if(Body_Status->get_bullet_flag == APPROACH)
      NUC_Obj.SetTargetMode(GROUND_LEFT);
    //第二次对准(第一次对准中断后进入手动对准，手动模式之后再开就是对右边)
    else if(Body_Status->get_bullet_flag == MANUAL)
    {
      NUC_Obj.SetTargetMode(GROUND_LEFT);
      Body_Status->get_bullet_flag = ALIGNING_AGAIN;
    }
    SuperRelay_Obj.Write_Cmd(X);
  }
  
  if(DR16.IsKeyPress(_V)){                              //取弹动作系列，取弹
    SuperRelay_Obj.Write_Cmd(V);
    if(Engineer_Master.Get_CurrentMode() == GET_BULLET)
    {
      //防止取弹过程没手动打断还在自动模式就取弹
      Body_Status->get_bullet_flag = MANUAL;
    }
  }
  
  if(DR16.IsKeyPress(_R)){                      
    Engineer_Master.Switch_ActionMode(UP_STAIRS);       //自动上岛模式
    Engineer_Master.up_stairs_time = TWICE;
    Engineer_Master.island_flag = 1;
  }
  
  if(DR16.IsKeyPress(_Q)){
    Engineer_Master.Switch_ActionMode(AUTO_ROTATE);      //自动向左旋转180°
    Engineer_Master.Set_AutoRotate(SINGLE);
  }
  
  if(DR16.IsKeyPress(_E)){
    Engineer_Master.Switch_ActionMode(AUTO_ROTATE);      //长按自动向右高速自旋
    Engineer_Master.Set_AutoRotate(CONTINUOUS);
  }
  else
    { //不按就退出自旋
    if(Engineer_Master.Get_CurrentMode() == AUTO_ROTATE && Body_Status->auto_rotate_flag == CONTINUOUS)
    Engineer_Master.Switch_ActionMode(NORMAL);
  }
  
  if(DR16.IsKeyPress(_B)){
    Engineer_Master.Switch_ActionMode(NORMAL);          //复位为普通模式
    NUC_Obj.SetTargetMode(WATCH_FORWARD);
    Engineer_Master.view_flag = FORWARD;
    SuperRelay_Obj.Write_Cmd(B);
    Engineer_Master.island_flag = 0;
  }
  
  if(DR16.IsKeyPress(_Z)){
    Engineer_Master.Switch_ActionMode(RESCUE);          //救援模式
    SuperRelay_Obj.Write_Cmd(Z);
  }
  
  if(DR16.IsKeyPress(_G)){                              //给弹动作系列
    //第一次抬升，第二次开仓
    SuperRelay_Obj.Write_Cmd(G);
    Engineer_Master.Switch_ActionMode(GIVE_BULLET_NORMAL);
  }

  if(DR16.IsKeyPress(_F)){
    Engineer_Master.Switch_ActionMode(UP_STAIRS);       //手动上岛模式
    Engineer_Master.Operations.upstairs_flag = MANUAL_UPSTAIRS;
    Engineer_Master.island_flag = 1;
  }
}
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
