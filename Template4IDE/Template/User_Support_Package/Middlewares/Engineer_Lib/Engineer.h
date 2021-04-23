/**
  **********************************************************************************
  * @file   : Engineer.h
  * @brief  : Header for Engineer(Robomaster 2019 version 4.0) robot lib.
  * @author : Mentos_Seetoo
  * @date   : July,2019
  * @version: 2.0
  **********************************************************************************
**/
#ifndef _ENGINEER_H_
#define _ENGINEER_H_
/* Includes ------------------------------------------------------------------*/ 
#include "Engineer_Header.h"
#include "chassis.h"


#ifdef  __cplusplus
/* Private  ------------------------------------------------------------------*/
#define CAT_WAIT                    0
#define CAT_LEFT                    1
#define CAT_RIGHT                   2
/* Robot Parameter (Units: )*/
#define UP_STAIRS_LIFT_SPEED        (0.90f)
#define UP_STAIRS_MOVE_SPEED        (0.45f)
#define UP_STAIRS_UP_SPEED          (0.75f)
#define UP_STAIRS_STOP_PITCH        (5.0f)
#define ALIGN_Y_SPEED               (0.3f)
#define THRESHOLD_TIME              (350)
/* Exported ------------------------------------------------------------------*/
/*
   User Specific Parameters
*/
#define BODY_MOTORS_NUM         3
#define BODY_PHOTOELE_NUM       8
#define DEBUG                   1


class CEngineer
{
  private:
    uint32_t        nowtime;
    _RobotStatus_t State;
    _ActionMode_t  Mode;
    _UnitStatus_t  Unit_Status;
    Vector_3f      Manual_Target;
    Vector_3f      Auto_Target;
    Vector_3f      CV_data;
    Euler_3f       IMU_data;

    char fault_extMsg;
    char fault_index;
    void SelfCheck_Status();
    void GearBox();
    void controller_set();
    void action_Battle();
    void action_Get_Bullet();
    void action_Give_Bullet();
    void action_Upstairs();
    void action_Rescue();
    void action_auto_rotate();
  
  public:
    CEngineer(){};
    CChassis Lunar_Chassis;     //为方便调试暂时开放访问
    _View_t view_flag;
    _Gear_t gear_flag;
    uint8_t cat_walk_flag;
    uint32_t threshold_time;
    bool    island_flag;
    _Body_Operation_t Operations;
    _UpStairsTime_t up_stairs_time;
    float   up_stairs_manual;
    float   up_stairs_co1;
    float   up_stairs_co2;
    float   up_stairs_pitch;
    float   align_Y_co;
    void    Init();
    void    Reset();
    _RobotStatus_t Central_Control();
    _ActionMode_t  Get_CurrentMode();
    void     Update_NowTime(uint32_t time);
    uint8_t  Switch_ActionMode(_ActionMode_t);
    void     Manual_XYZ(float, float ,float);
    void     Follow_XYZ(float, float, float);
    void     Set_AutoRotate(_Auto_Rotate_t direction);
    int32_t  IO_Port(_Unit_Type UnitToOperate, _IO_Type mode);
    void     IO_Port(_Unit_Type UnitToOperate, _IO_Type mode, int32_t value);
    void     IO_Port(_Unit_Type UnitToOperate, _IO_Type mode, uint8_t value);
    void     IO_Port(_IO_Type mode, float x,float y ,float z);
    _Body_Operation_t* IO_Port(_IO_Type mode);
};


#endif
#endif
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
