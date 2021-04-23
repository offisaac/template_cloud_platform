/**
  **********************************************************************************
  * @file   : Tool_CANopen.h
  * @brief  : Header for Tool_CANopen.cpp
  * @author ：Mentos Seetoo
  * @date   ：February,2019
  * @version：1.0
  **********************************************************************************
**/
#ifndef  _TOOL_CANOPEN_H_
#define  _TOOL_CANOPEN_H_

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>

/* Private  ------------------------------------------------------------------*/
/* Common Data Type Define -----------------------*/
#ifndef __CCOBTypeDef_DEFINED
#define __CCOBTypeDef_DEFINED
/* CAN message data type(Communication Object/标准数据帧) */
typedef struct{
  uint16_t  ID;
  uint8_t   DLC;
  uint8_t   Data[8];
}COB_TypeDef;
#endif

typedef struct{
  int32_t  dat1;
  int32_t  dat2;
}double_int32;

union Eight_Byte
{
  double_int32 data;
  char         array[8];
};

/* Master operation states*/
#define READ_OBJ        0x40
#define WRITE_OBJ       0x22
#define ERROR           0
#define FINISHED        1
#define OPERATING       2
#define WAIT_ACK        1
#define WAIT_TX         1
#define ERROR_OCCURRED  0xFF
#define IDLE            0
  
/* Node States */
#define PRE_OPERATIONAL     0
#define OPERATIONAL         1
#define STOPPED             2

#ifdef  __cplusplus
namespace CANopen
{
enum Device_State
{
  Switch_On_Disabled = 1,
  Ready_to_Switch_On,
  Switched_On,
  Operation_Enable,
  Quick_Stop_Active,
  Fault_Reaction_Active,
  Fault
};

enum Step_State
{
  START = 1,
  STEP1,
  STEP2,
  STEP3,
  STEP4,
  STEP5
};

enum Operation_Mode
{
  PRO_POSITION = 1,
  PRO_VELOCITY = 3,
  PRO_TORQUE = 4,
  HOMEING = 6,
  INTERPLOATED_POSITION = 7,
  CYCLIC_POSITION,
  CYCLIC_VELOCITY,
  CYLIC_TORQUE
};

/* COB-ID List */
#define NMT_ID           0x000
#define NMT_NODE_ID      0x700
#define EMCY_ID          0x080
#define TPDO1_ID         0x180
#define RPDO1_ID         0X200
#define TPDO2_ID         0x280
#define RPDO2_ID         0x300
#define TPDO3_ID         0x380
#define RPDO3_ID         0x400
#define TPDO4_ID         0x480
#define RPDO4_ID         0x500
#define T_SDO_ID         0x600
#define R_SDO_ID         0x580

/* Object Dictionary */
#define NO_SUB_INDEX     0
#define SYNC_ID_MESSAGE               0x1005
#define SYNC_CYCLE_PERIOD             0x1006
#define RPDO1_COMMUNICATION           0x1400
#define RPDO2_COMMUNICATION           0x1401
#define RPDO3_COMMUNICATION           0x1402
#define RPDO4_COMMUNICATION           0x1403
#define RPDO1_MAP_CONTENT             0x1600
#define RPDO2_MAP_CONTENT             0x1601
#define RPDO3_MAP_CONTENT             0x1602
#define RPDO4_MAP_CONTENT             0x1603
#define TPDO1_COMMUNICATION           0x1800
#define TPDO2_COMMUNICATION           0x1801
#define TPDO3_COMMUNICATION           0x1802
#define TPDO4_COMMUNICATION           0x1803
#define TPDO1_MAP_CONTENT             0x1A00
#define TPDO2_MAP_CONTENT             0x1A01
#define TPDO3_MAP_CONTENT             0x1A02
#define TPDO4_MAP_CONTENT             0x1A03
#define CONTROL_WORD                  0x6040
#define STATUS_WORD                   0x6041
#define MODE_OF_OPERATION             0x6060
#define MODE_OF_OPERATION_DISPLAY     0x6061
#define FOLLOWING_ERROR_WINDOW        0x6065
#define POSITION_WINDOW               0x6067
#define POSITION_WINDOW_TIME          0x6068
#define SOFT_LIMIT_POSOSITION         0x607D
#define PROFILE_VELOCITY              0x6081
#define PROFILE_ACCELERATION          0x6083
#define PROFILE_DECELERATION          0x6084
#define SUBMODE_INTPOSITION           0x60C0
#define INTPOSITION_TARET_DATA        0x60C1
#define INTPOSITION_CONST_TIME        0x60C2
#define MOTOR_TYPE                    0x6410

/* NMTcommand */
#define START_NODE          0X01
#define STOP_NODE           0X02
#define ENTER_PRE_OPERATION 0X80
#define RESET_NODE          0X81
#define RESET_COMMUNICATION 0x82
/* Exported ------------------------------------------------------------------*/
/* User Specific Parameters ----------------------*/
#define NODE_LIST_MAX       2
#define PDO_MAX_MapNum      2
#define PDO_LIST_NUM        2
//Use custom PDO or default confiuration, define marco below
#define USE_DEFAULT_PDO

/* PDO Parameters */
typedef struct
{
  //Communication parameters
  uint16_t  Com_Index;
  uint8_t   transmission_modes;
  uint32_t  COB_ID;
  
  //Map Parameters
  uint16_t  Map_Index;
  uint8_t   MapObj_Len[PDO_MAX_MapNum];
  int32_t   MapObj_Index_SubIndex[PDO_MAX_MapNum];
  int32_t*  MapObj_Address[PDO_MAX_MapNum];
  uint8_t   subNum;
}PDO_Parameters;


/* 
   Canopen base class, this class is to provide the
   basic functions for CANopen Protocol.
*/
class Base
{
  protected:
    static COB_TypeDef NMTBuild_Cache;
    static uint8_t     SDOBuild_Cache[4];
    COB_TypeDef        TX_Cache;
    int32_t            LastSDO_ErrorCode;
    uint8_t            ACK_requireFlag;
    uint8_t            step_count;
    int8_t             node_state;
    PDO_Parameters     PDO_List[PDO_LIST_NUM];
    static void  NMT_Build(COB_TypeDef* NMT_Cache, uint8_t NMTcommand,uint8_t node_id);
    void         SDO_Build(uint8_t commandCode, uint16_t index, uint8_t sub_index);
    void         PDO_Build(PDO_Parameters* mapped_PDO);
  public:
    static uint8_t     total_list_num;
    static Base *Node_List[NODE_LIST_MAX];
    Base(uint8_t id);
    uint8_t      node_id;
    //When this flag turns to 'WAIT_TX', it means 
    //TX_Cache has message to send.
    uint8_t      Send_requireFlag;
    uint8_t      PDO_Map(PDO_Parameters *Param);
  
    static void  EnableNodes_All(COB_TypeDef* ObjToSend);
    static void  DisableNodes_All(COB_TypeDef* ObjToSend);
    static void  ConfigNodes_All(COB_TypeDef* ObjToSend);
    void         EnableNode();
    void         ConfigNode();
    void         DisableNode();
    int8_t       NodeState();
  
    uint8_t      Configue_SYNC(uint32_t microseconds);
    uint8_t      Resolve_COB(COB_TypeDef* RX_Buff);
    uint8_t      Resolve_COB_PDO(COB_TypeDef* RX_Buff);
    COB_TypeDef* getTX_COB();
};

/* 
   CANopen motion control class
*/
class MotionControl: public Base
{
  private:
    Device_State    device_state;
    Operation_Mode  operation_mode;
  public:
    MotionControl(uint8_t id);
    Device_State   DeviceState();
    Operation_Mode OperationMode();
  
    uint8_t  DeviceInit();
    uint8_t  SwitchState(Device_State targetState);
    void     SwitchMode(Operation_Mode targetMode);
    uint8_t  Configure_ControlLoop();
  
    //Configure specific Operation Mode
    //1) Homing Mode 
    //2) Profile Position Mode
    uint8_t Configure_ProPosition();
    //3) Profile Velocity Mode
    uint8_t Configure_ProVelocity();
    //4) Profile Torque Mode

    //5) Interpolated Position Mode(Denote by 'IntPosition')
    int16_t  IntPosition_subMode;
    uint8_t  PDOConfigFlag;
    uint8_t  Select_IntPositionSubMode(int16_t subMode);
    uint8_t  Configure_IntPosition(int16_t subMode, uint8_t interval_time);
    void     Set_Target(int32_t position, uint8_t time, int32_t velocity);
#ifdef USE_DEFAULT_PDO
    uint8_t  Confiure_DefaultPDO(int32_t* positionToSend,int32_t* positionToRead,int32_t* torqueToRead);
    void     Set_TargetByPDO();
#endif
};

inline void Error_Handle();
}

#endif
#endif
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
