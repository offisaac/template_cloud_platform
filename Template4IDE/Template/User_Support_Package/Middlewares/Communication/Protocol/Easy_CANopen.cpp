/**
  **********************************************************************************
  * @file   : Tool_CANopen.cpp
  * @brief  : CANopen support file.This file is aimed at providing an easy way to 
              contact with devices which are underlying Controller Area Network (CAN) 
              and using the CANopen Protocol on STM32F4.In this file we will provide 
              several middleware classes for master. These classes are based on :
              1.CANopen Application Layer and Communication Profile (CiA Draft Stand-
              ard 301,also known as DS301) 
              2.CANopen Profile for Drives and Motion Control (CiA Draft Standard 
              Proposal 402,also known as DSP402) 
              However, given the limited capacity of individual, the work which I have
              done can only realize specific functions and they may not entirely 
              conform to the protocal.If you find any mistakes, please contact author.
  * @author ：Mentos Seetoo(201730645459@scut.edu.cn)
  * @date   ：February,2019
  * @version：1.0
  **********************************************************************************
  ==================================================================================
                        ##### How to use this tool #####
  ==================================================================================
  1) Instantiation Object:
    Add class 'CANopen::Base' or 'CANopen::MotionControl' to user's objects which ne-
    ed to use CANopen Protocol. Then every object will have a CANopen access port wi-
    th an index number(between 1 and NODE_LIST_MAX).
    
  2) Initialization Network:
    Use 'DeviceInit()' to establish connection with device.In this version, init fu-
    nction does not contain the PDO mapping process, for providing more choices for 
    user.
    
  3) Configuration:
    Use 'Configure_xx()' to configure the specific Operation mode
    1.In Interpolated Position Mode:
      Confiure_DefaultPDO();|Not neccessary.
      Configue_SYNC();      |Not neccessary.
      Configure_IntPosition();
      
  4) Control:
    1. Use 'Set_xx()' to set the target.
    2. Use 'Resolve_xx()' to get message.
    
  # Attention : 
  1) Parameters: Before using this file, user has to configure some marcos in 'User 
     Specific Parameters'
  2) PDO Configuration: In this file, the default PDO mapping will be changed for 
     Interpolated Position Mode. If user need to use custom PDO configuration, 
     remember to rewrite the function 'Resolve_COB_PDO()'.
**/

/* Includes ------------------------------------------------------------------*/ 
#include "Easy_CANopen.h"
#include <string.h>

/* Functions -----------------------------------------------------------------*/
using namespace CANopen;
COB_TypeDef Base::NMTBuild_Cache = {0};
uint8_t Base::SDOBuild_Cache[4] = {0};
Base* Base::Node_List[NODE_LIST_MAX] = {NULL};
uint8_t Base::total_list_num = 0;

Base::Base(uint8_t id)
{
  node_id = id;
  Node_List[total_list_num] = this;
  total_list_num ++;
  node_state = -1;
}

/**
* @brief  Network Management Object build function
* @param  NMTcommand:
          node_id:
* @return None.
*/
void Base::NMT_Build(COB_TypeDef* Cache, uint8_t NMTcommand,uint8_t node_id)
{
  Cache->ID = 0x00;
  Cache->DLC = 2;
  Cache->Data[0] = NMTcommand;
  Cache->Data[1] = node_id;
}


/**
* @brief  Service-Data-Object build function. In this function Send_requireFlag
          and ACK_requireFlag are set to 1, indicating that TX_Cache has a message 
          to send and need ACK.But this file doesn't provide time out detection.
          Usage:1) Set the 'SDOBuild_Cache' array
                2) Call 'SDO_Build()'
                3) Take the target SDO from TX_Cache.
* @param  commandCode:
          index:
          sub_index:
          p_data: 4-byte array
* @return None.
*/
void Base::SDO_Build(uint8_t commandCode, uint16_t index, uint8_t sub_index)
{
  TX_Cache.ID = T_SDO_ID + node_id;
  TX_Cache.DLC = 8;
  TX_Cache.Data[0] = commandCode;
  //Using LSB
  TX_Cache.Data[1] = index & 0xff;
  TX_Cache.Data[2] = (index >> 8);
  TX_Cache.Data[3] = sub_index;
  memcpy(&TX_Cache.Data[4], SDOBuild_Cache, 4);

  //Clear the cache zone.
  memset(SDOBuild_Cache, 0, 4);
  Send_requireFlag = WAIT_TX;
  ACK_requireFlag  = WAIT_ACK;
}

/**
* @brief  Process-Data-Object build function based on the input PDO parameters. The 
          input PDO must be mapped before used.
* @param  mapped_PDO:
* @return 
*/
void Base::PDO_Build(PDO_Parameters* mapped_PDO)
{
  TX_Cache.ID = mapped_PDO->COB_ID + node_id;
#ifdef USE_DEFAULT_PDO
  if(mapped_PDO->MapObj_Address[0] != NULL)
  {
    TX_Cache.DLC = 4;
    TX_Cache.Data[0] = *mapped_PDO->MapObj_Address[0];
    TX_Cache.Data[1] = *mapped_PDO->MapObj_Address[0]>>8;
    TX_Cache.Data[2] = *mapped_PDO->MapObj_Address[0]>>16;
    TX_Cache.Data[3] = *mapped_PDO->MapObj_Address[0]>>24;
  }
#endif
//  //Demo Below
//  uint8_t* temp_ptr = TX_Cache.Data;
//  for(int8_t j = 0,k = 0; j < mapped_PDO->subNum ;j++)
//  {
//    if(mapped_PDO->MapObj_Address[j] != NULL)
//    {
//      //CANopen Using LSB to transmit data.
//      for(k = mapped_PDO->MapObj_Len[j] - 1; k >= 0 ; k--)
//      {
//        *(temp_ptr+k) = *mapped_PDO->MapObj_Address[j]>>(k*8); 
//      }
//    }
//  }
  Send_requireFlag = WAIT_TX;
}

/**
* @brief  Map Process-Data-Object operation function
* @param  Param :
* @return OPERATING(2) : Function need to re-enter.
          ERROR(0) : Error.
          FINISHED(1) : Operation finished.
*/
uint8_t Base::PDO_Map(PDO_Parameters *Param)
{
  static uint8_t i = 0;
  int32_t Map_Value;
  //Waiting for ACK
  if(ACK_requireFlag == WAIT_ACK)
  {
    return OPERATING;
  }
  
  //Main Steps
  else if(ACK_requireFlag == 0)
  {
    switch(step_count)
    {
      //Disable the PDO -----------------------------------
      case IDLE:
        if(i < Param->subNum)i++;
        step_count = START;
        if(i == 1)
        {
          SDOBuild_Cache[0] = 0;
          SDO_Build(WRITE_OBJ, Param->Map_Index, 0);
        }
        else step_count = STEP2;
        return OPERATING;
      
      //Set the communication parameters -------------------
      case STEP1:
        SDOBuild_Cache[0] = Param->transmission_modes;
        SDO_Build(WRITE_OBJ, Param->Com_Index, 2);
        return OPERATING;

      //Map the data. --------------------------------------
      case STEP2:
          Map_Value = Param->MapObj_Index_SubIndex[i-1]<<8|(Param->MapObj_Len[i-1]*8);
          SDOBuild_Cache[0] = Map_Value;
          SDOBuild_Cache[1] = Map_Value>>8;
          SDOBuild_Cache[2] = Map_Value>>16;
          SDOBuild_Cache[3] = Map_Value>>24;
          SDO_Build(WRITE_OBJ, Param->Map_Index, i);
          return OPERATING;

      //Set the number of mapped objects and enable the PDO 
      case STEP3:
        step_count = IDLE;
        if(i == Param->subNum)
        {
          i = 0;
          SDOBuild_Cache[0] = Param->subNum;
          SDO_Build(WRITE_OBJ, Param->Map_Index,0);
          return FINISHED;
        }
        return OPERATING;
      default:
        step_count = IDLE;
        ACK_requireFlag = 0;
        return ERROR;
    }
  }
  
  //Error handle, clear the flags and exit the operation.
  else
  {
    step_count = IDLE;
    ACK_requireFlag = 0;
    return ERROR;
  }
}

/**
* @brief  Build Commonly used Message
* @param  None.
* @return None.
*/
void Base::EnableNode()
{
  NMT_Build(&TX_Cache, START_NODE, node_id);
  Send_requireFlag = WAIT_TX;
}

void Base::ConfigNode()
{
  NMT_Build(&TX_Cache, ENTER_PRE_OPERATION,node_id);
  Send_requireFlag = WAIT_TX;
}

void Base::DisableNode()
{
  NMT_Build(&TX_Cache, STOP_NODE, node_id);
  Send_requireFlag = WAIT_TX;
}

void Base::EnableNodes_All(COB_TypeDef* ObjToSend)
{
  NMT_Build(ObjToSend, START_NODE,0);
}

void Base::ConfigNodes_All(COB_TypeDef* ObjToSend)
{
  NMT_Build(ObjToSend, ENTER_PRE_OPERATION,0);
}  

void Base::DisableNodes_All(COB_TypeDef* ObjToSend)
{
  NMT_Build(ObjToSend, STOP_NODE,0);
}

/**
* @brief  Read the Node state in local
* @param  None.
* @return node_state.
*/
int8_t Base::NodeState()
{
  return node_state;
}


/**
* @brief  Configure the SYNC message. It's used to synchronize multiple
          devices on a CANopen network. Units:microseconds.
          # Attention: There should be only one SYNC producer in a CANopen
          network!
* @param  microseconds: the unit is 'us' not 'ms'
* @return OPERATING(2) : Function need to re-enter.
          ERROR(0) : Error.
          FINISHED(1) : Operation finished.
*/
uint8_t Base::Configue_SYNC(uint32_t microseconds)
{
  //Waiting for ACK
  if(ACK_requireFlag == WAIT_ACK)
  {
    return OPERATING;
  }
  //Main Steps
  else if(ACK_requireFlag == 0)
  {
    switch(step_count)
    {
      //Shut down the SYNC generator
      case IDLE:
        step_count = START;
        SDOBuild_Cache[0] = 0x80;
        SDO_Build(WRITE_OBJ, SYNC_ID_MESSAGE, 0);
        return OPERATING;
      //Set the period time
      case STEP1:
        SDOBuild_Cache[0] = microseconds;
        SDOBuild_Cache[1] = microseconds>>8;
        SDOBuild_Cache[2] = microseconds>>16;
        SDOBuild_Cache[3] = microseconds>>24;
        SDO_Build(WRITE_OBJ, SYNC_CYCLE_PERIOD, 0);
        return OPERATING;
      //Restart the SYNC generator
      case STEP2:
        SDOBuild_Cache[0] = 0x80;
        SDOBuild_Cache[3] = 0x40;
        SDO_Build(WRITE_OBJ, SYNC_ID_MESSAGE, 0);
        return OPERATING;
      case STEP3:
        step_count = IDLE;
      return FINISHED;
      
      default:
        step_count = IDLE;
        ACK_requireFlag = 0;
        return ERROR;
    }
  }
  
  //Error handle, clear the flags and exit the operation.
  else
  {
    step_count = IDLE;
    ACK_requireFlag = 0;
    return ERROR;
  }
}


/**
* @brief  Get the builded COB to send
* @param  None.
* @return NULL:No message to send 
          Address:Get message to send.
*/
COB_TypeDef* Base::getTX_COB()
{
  if(Send_requireFlag == 1)
  {
    Send_requireFlag = 0;
    return &TX_Cache;
  }
  else return NULL;
}


/**
* @brief  Resolve the RX_COB to get message form node.
* @param  RX_Buff:
* @return 0: No Mseeage/Error
          1: Get Meaasge
*/
uint8_t Base::Resolve_COB(COB_TypeDef* RX_Buff)
{
  union Eight_Byte Recv_Cache;
  switch(RX_Buff->ID - node_id)
  {
    /* NMT Frame */
    case NMT_NODE_ID:
    {
      switch(RX_Buff->Data[0])
      {
        case 0:
          //If there is no error, the device will automatically enter 
          //'PRE_OPERATIONAL' state and send a 'Boot up' NMT Object 
          ACK_requireFlag = 0;
          node_state = PRE_OPERATIONAL;
          break;
        case 4:
          node_state = STOPPED;
        break;
        case 5:
          node_state = OPERATIONAL;
        break;
        case 127:
          node_state = PRE_OPERATIONAL;
        break;
        default:break;
      }
    }
    break;
    
    /* SDO Frame */
    case R_SDO_ID:
    {
      switch(RX_Buff->Data[0])
      {
        //ACK Frame
        case 0x60:
          if(ACK_requireFlag == WAIT_ACK) 
          {
            if(step_count != IDLE) step_count ++;
            ACK_requireFlag = 0;
          }
          else return ERROR;
          break;
        //Error Report Frame
        case 0x80:
          memcpy(&Recv_Cache.data.dat2, &RX_Buff->Data[4],4);
          LastSDO_ErrorCode = Recv_Cache.data.dat2;
          ACK_requireFlag = 2;
          break;
        default: return ERROR;
      }
    }
    break;
    
    /*PDO Frame*/
    default:
#ifdef USE_DEFAULT_PDO
      if((RX_Buff->ID & 0x3F0) == PDO_List[0].COB_ID)
      {
        memcpy(&Recv_Cache,RX_Buff->Data,8);
        if(PDO_List[0].MapObj_Address[0] != NULL)
        {
          *PDO_List[0].MapObj_Address[0] = Recv_Cache.data.dat1;
        }
        if(PDO_List[0].MapObj_Address[1] != NULL)
        {
          *PDO_List[0].MapObj_Address[1] = Recv_Cache.data.dat2;
          return 1;
        }
      }
#endif
      return 0;
  }
  return 1;
}

/**
* @brief  Resolve the PDO. If you need this function run faster, you
          can edit this function according to your custom PDO configu-
          ration. Here I give a common resolve demo in annotation. 
* @param  RX_Buff:make sure the buff wouldn't change during resolving
* @return 0: No Mseeage/Error
          1: Get Meaasge
*/
uint8_t Base::Resolve_COB_PDO(COB_TypeDef* RX_Buff)
{
  //Demo Below, it can resolve any PDO accodring to the 'PDO_List'.
  //If you need a faster function, use union to read.
  uint8_t* temp_ptr= RX_Buff->Data;
  for(int8_t i = 0; i < PDO_LIST_NUM ; i++)
  {
    if((RX_Buff->ID & 0x3F0) == PDO_List[i].COB_ID)
    {
      for(int8_t j=0,k = 0; j < PDO_List[i].subNum ;j++)
      {
        if(PDO_List[i].MapObj_Address[j] != NULL)
        {
          temp_ptr += PDO_List[i].MapObj_Len[j];
          for(k = PDO_List[i].MapObj_Len[j] - 1; k >= 0 ; k--)
          {
            *PDO_List[i].MapObj_Address[j] += (*(temp_ptr+k) << (k*8));
          }
        }
        return ERROR;
      }
      return 1;
    }
  }
  return 0;
}


/* Constructor Function*/
MotionControl::MotionControl(uint8_t id):Base(id)
{
#ifdef USE_DEFAULT_PDO
  PDO_Parameters temp[PDO_LIST_NUM] =
    {
     {TPDO3_COMMUNICATION, 1, TPDO3_ID, TPDO3_MAP_CONTENT,{4,4},{0x606400, 0x607700},{0,0}, 2},
     {RPDO3_COMMUNICATION, 255, RPDO3_ID, RPDO3_MAP_CONTENT,{4,0},{0x60C101, 0},       {0,0}, 1}
    };
    memcpy(PDO_List,temp,sizeof(PDO_Parameters)*PDO_LIST_NUM);
#endif
  //If there is no error, the device automatically enter 'Switch_On_Disabled' state
  device_state = Switch_On_Disabled;
}

/**
* @brief  Initialize the motion control device 
* @param  None.
* @return OPERATING(2) : Function need to re-enter.
          ERROR(0) : Error.
          FINISHED(1) : Operation finished.
*/
uint8_t MotionControl::DeviceInit()
{
  //Waiting for ACK
  if(ACK_requireFlag == WAIT_ACK)
  {
    return OPERATING;
  }
  
  //Main Steps
  else if(ACK_requireFlag == 0)
  {
    switch(step_count)
    {
      case IDLE:
        if(node_state == -1)
          ConfigNode();
        step_count = START;
        step_count = STEP1;
        return OPERATING;
      /*Booting the device into 'Operation_Enable' Mode*/
      case STEP1:
        SwitchState(Ready_to_Switch_On);
        return OPERATING;
      
      case STEP2:
        device_state = Ready_to_Switch_On;
        SwitchState(Switched_On);
        return OPERATING;
      
      case STEP3:
        device_state = Switched_On;
        SwitchState(Operation_Enable);
        return OPERATING;
      
      case STEP4:
        device_state = Operation_Enable;
        step_count = IDLE;
        return FINISHED;
      default: 
        step_count = IDLE;
        ACK_requireFlag = 0;
        return ERROR;
    }
  }
  
  //Error handle, clear the flags and exit the operation.
  else
  {
    step_count = IDLE;
    ACK_requireFlag = 0;
    return ERROR;
  }
}

/**
* @brief  Read the recorded device state in local
* @param  None.
* @return Device_State.
*/
Device_State MotionControl::DeviceState()
{
  return device_state;
}

/**
* @brief  Read the recorded operation mode in local
* @param  None.
* @return Operation_Mode.
*/
Operation_Mode MotionControl::OperationMode()
{
  return operation_mode;
}


/**
* @brief  Switching state is based on the 'State Machine'
          from the 'DSP402'
* @param  targetState:
* @return 1:Success
          0:Error Target State(Cannot directly switch)
*/
uint8_t MotionControl::SwitchState(Device_State targetState)
{
  switch(device_state)
  {
    case Switch_On_Disabled:
      switch(targetState)
      {
        case Switch_On_Disabled:
          break;
        case Ready_to_Switch_On://2
          SDOBuild_Cache[0] = 0x06;
          break;
        default:
          return ERROR;
      }
      break;
      
    //--------------------------------------
    case Ready_to_Switch_On:
      switch(targetState)
      {
        case Ready_to_Switch_On:
          break;
        case Switched_On://3
          SDOBuild_Cache[0] = 0x07;
          break;
        case Switch_On_Disabled://7
          SDOBuild_Cache[0] = 0x00;
          break;
        default:
          return ERROR;
      }
      break;
    
    //---------------------------------------
    case Switched_On:
      switch(targetState)
      {
        case Switched_On:
          break;
        case Ready_to_Switch_On://6
          SDOBuild_Cache[0] = 0x06;
          break;
        case Operation_Enable://4
          SDOBuild_Cache[0] = 0x0F;
          break;
        default:
          return ERROR;
      }
      break;
      
    //----------------------------------------
    case Operation_Enable:
      switch(targetState)
      {
        case Operation_Enable:
          break;
        case Switch_On_Disabled://9
          SDOBuild_Cache[0] = 0x00;
          break;
        case Ready_to_Switch_On://8
          SDOBuild_Cache[0] = 0x06;
          break;
        case Switched_On://5
          SDOBuild_Cache[0] = 0x07;
          break;
        case Quick_Stop_Active://11
          SDOBuild_Cache[0] = 0x02;
          break;
        default:
          return ERROR;
      }
      break;
      
    //----------------------------------------
    case Quick_Stop_Active:
      switch(targetState)
      {
        case Quick_Stop_Active:
          break;
        case Switch_On_Disabled://12
          //No need command;
          break;
        case Operation_Enable://16
          SDOBuild_Cache[0] = 0x0F;
          break;
        default:
          return ERROR;
      }
      break;
      
    //-----------------------------------------
    case Fault_Reaction_Active:
      switch(targetState)
      {
        case Switch_On_Disabled://15
          SDOBuild_Cache[0] = 0x80;
          break;
        default:
          return ERROR;
      }
      break;
    
    //----------------------------------------
    case Fault:
      break;
  }
  
  SDO_Build(WRITE_OBJ, CONTROL_WORD, NO_SUB_INDEX);
  
  return FINISHED;
}

#ifdef USE_DEFAULT_PDO
/**
* @brief  Confiure the default PDO that used in Interpolated Position Mode.
          When mapping the PDO, this function will first enter the config-
          mode of the node, then back to operation-mode.
* @param  None.
* @return None.
*/
uint8_t MotionControl::Confiure_DefaultPDO(int32_t* positionToSend,int32_t* positionToRead,int32_t* torqueToRead)
{
  switch(PDOConfigFlag)
  {
    //Set the node mode:'Pre-Operation'
    case 0:
      ConfigNode();
      PDOConfigFlag = 1;
    return OPERATING;
    
    //Configure a Transmit PDO to read actual value
    case 1:
      if(PDO_Map(&PDO_List[0])==FINISHED) 
      {
        PDO_List[0].MapObj_Address[0] = positionToRead;
        PDO_List[0].MapObj_Address[1] = torqueToRead;
        PDOConfigFlag = 2;
      }
      return OPERATING;
    
    //Configure a Receive PDO to set target.
    case 2:
      if(PDO_Map(&PDO_List[1]) == FINISHED)
      {
        PDO_List[1].MapObj_Address[0] = positionToSend;
        PDOConfigFlag = 3;
      }
      return OPERATING;
    
    //Set the node mode:'Operation mode'
    case 3:
      EnableNode();
      PDOConfigFlag = 0;
      return FINISHED;

    default: 
      Error_Handle();
      return ERROR;
  }
}

/**
* @brief  Set the Taret Value. Using the default PDO.
* @param  None.
* @return None.
*/
void MotionControl::Set_TargetByPDO()
{
  TX_Cache.ID = PDO_List[1].COB_ID + node_id;

  if(PDO_List[1].MapObj_Address[0] != NULL)
  {
    TX_Cache.DLC = 4;
    *TX_Cache.Data = *PDO_List[1].MapObj_Address[0];
    *(TX_Cache.Data+1) = *PDO_List[1].MapObj_Address[0]>>8;
    *(TX_Cache.Data+2) = *PDO_List[1].MapObj_Address[0]>>16;
    *(TX_Cache.Data+3) = *PDO_List[1].MapObj_Address[0]>>24;
  }
  
  Send_requireFlag = WAIT_TX;
}
#endif

/**
* @brief  Switch operation mode, no limitation. When switching mode,
          previous command would be discard.
* @param  targetMode:
* @return 
*/
void MotionControl::SwitchMode(Operation_Mode targetMode)
{
  SDOBuild_Cache[0] = (uint8_t)targetMode;
  
  SDO_Build(WRITE_OBJ, MODE_OF_OPERATION, NO_SUB_INDEX);
}


/**
* @brief  Selsect the submode of Interpolated Position Mode
* @param  subMode: 0 ---> Linear interpolation with a constant time.
                  -1 ---> Linear interpolation with variable time.
                  -2 ---> Cubic polynomial interpolation (PVT).
* @return 0:Error submode
          1:Build success
*/
uint8_t  MotionControl::Select_IntPositionSubMode(int16_t subMode)
{
  if(subMode>0 || subMode < -2) return ERROR;
  
  SDOBuild_Cache[0] = subMode ;
  SDOBuild_Cache[1] = subMode>>8;
  
  SDO_Build(WRITE_OBJ, SUBMODE_INTPOSITION, NO_SUB_INDEX);
  return FINISHED;
}


/**
* @brief  Configure and enter the Interpolated Position Mode.
* @param  subMode: 0 ---> Linear interpolation with a constant time.
                  -1 ---> Linear interpolation with variable time.
                  -2 ---> Cubic polynomial interpolation (PVT).
          interval_time: only used in Linear interpolation Mode
* @return OPERATING(2) : Function need to re-enter.
          ERROR(0) : Error.
          FINISHED(1) : Operation finished.
*/
uint8_t  MotionControl::Configure_IntPosition(int16_t subMode,uint8_t interval_time = 0)
{
  //Waiting for ACK
  if(ACK_requireFlag == WAIT_ACK)return OPERATING;
  
  //Main Steps
  else if(ACK_requireFlag == 0)
  {
    switch(step_count)
    {
      //Set operation mode -----------------------------------
      case IDLE:
        SwitchMode(INTERPLOATED_POSITION);
        step_count = START;
        return OPERATING;
      //Set sub mode -----------------------------------------
      case STEP1:
        operation_mode = INTERPLOATED_POSITION;
        if(!Select_IntPositionSubMode(subMode))return ERROR;
        return OPERATING;
      //Set the interval time --------------------------------
      case STEP2:
        switch(subMode)
        {
          case 0:
            SDOBuild_Cache[0] = interval_time;
            SDO_Build(WRITE_OBJ, INTPOSITION_CONST_TIME, 1);
            break;
          case -1:
            //Reserved for future.
            break;
          case -2:
            //Reserved for future.
            break;
          default:return ERROR;
        }
        return OPERATING;
      //Activate Interpolated Position Mode -----------------
      case STEP3:
        SDOBuild_Cache[0] = 0x1F;
        SDO_Build(WRITE_OBJ, CONTROL_WORD, NO_SUB_INDEX);
        return OPERATING;
      //Finish Configure, ready to move ---------------------
      case STEP4:
        step_count = IDLE;
        return FINISHED;
      default:
        step_count = IDLE;
        ACK_requireFlag = 0;
        return ERROR;
    }
  }
  
  //Error handle, clear the flags and exit the operation.
  else
  {
    step_count = IDLE;
    ACK_requireFlag = 0;
    return ERROR;
  }
}

/**
* @brief  Normally the program would not reach here unless an 
          unexpected error has occured in CANopen Tool.
*/
void CANopen::Error_Handle()
{
  while(1){};
}
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
