/**
  ******************************************************************************
  * Copyright (c) 2019 - ~, SCUT-RobotLab Development Team
  * @file   : referee.cpp
  * @author : charlie 602894526@qq.com
  * @brief  : Code for communicating with Referee system of Robomaster 2019.
  * @date   : 2019-03-01
  * @par Change Log：
  *  Date           Author   Version    Notes
  *  2019-03-01     charlie   2.0.0     Creator
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================


  ******************************************************************************
  * @attention:
  * 
  * if you had modified this file, please make sure your code does not have many 
  * bugs, update the version NO., write dowm your name and the date, the most
  * important is make sure the users will have clear and definite understanding 
  * through your new brief.
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "referee.h"

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
int32_t index,buff_read_index;
short CRC16_Function,CRC16_Referee;
uint8_t RFdataBuff[256];
uint8_t byte = 0;
int32_t read_len;
uint16_t data_len;
static uint8_t unpack_step=0;
static uint8_t protocol_packet[PROTOCAL_FRAME_MAX_SIZE];

static uint8_t Temp[50];
static uint8_t se,se2;

static const unsigned char CRC8_TAB[256] = {
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3,
    0xfd, 0x1f, 0x41, 0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01,
    0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe,
    0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62, 0xbe, 0xe0, 0x02, 0x5c,
    0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff, 0x46,
    0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb,
    0x59, 0x07, 0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5,
    0xfb, 0x78, 0x26, 0xc4, 0x9a, 0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6,
    0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24, 0xf8, 0xa6, 0x44, 0x1a, 0x99,
    0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9, 0x8c, 0xd2,
    0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93,
    0xcd, 0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31,
    0xb2, 0xec, 0x0e, 0x50, 0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d,
    0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee, 0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d,
    0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73, 0xca, 0x94, 0x76,
    0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
    0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4,
    0xaa, 0x48, 0x16, 0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75,
    0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8, 0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9,
    0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};
static const uint16_t wCRC_Table[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf, 0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e,
    0xf8f7, 0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e, 0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64,
    0xf9ff, 0xe876, 0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd, 0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e,
    0xfae7, 0xc87c, 0xd9f5, 0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c, 0xbdcb, 0xac42, 0x9ed9, 0x8f50,
    0xfbef, 0xea66, 0xd8fd, 0xc974, 0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb, 0xce4c, 0xdfc5, 0xed5e,
    0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3, 0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a, 0xdecd, 0xcf44,
    0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72, 0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9, 0xef4e,
    0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1, 0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70, 0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e,
    0xf0b7, 0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff, 0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324,
    0xf1bf, 0xe036, 0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e, 0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e,
    0xf2a7, 0xc03c, 0xd1b5, 0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd, 0xb58b, 0xa402, 0x9699, 0x8710,
    0xf3af, 0xe226, 0xd0bd, 0xc134, 0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c, 0xc60c, 0xd785, 0xe51e,
    0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3, 0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb, 0xd68d, 0xc704,
    0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232, 0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a, 0xe70e,
    0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1, 0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330, 0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1,
    0x0f78
};

ext_game_state_t GameState;
ext_game_robot_HP_t GameRobotHP;
ext_event_data_t EventData;
ext_game_robot_state_t GameRobotState;
ext_power_heat_data_t PowerHeatData;
ext_robot_hurt_t RobotHurt;
ext_shoot_data_t ShootData;
ext_bullet_remaining_t BulletRemaining;
ext_student_interactive_header_data_t StudentInteractiveHeaderData;
robot_interactive_data_t RobotInteractiveData;

/* Private type --------------------------------------------------------------*/

/* Private function declarations ---------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief   Referee data check
  * @param   **p: Pointer to checked data
  * @retval  0:wrong 1:ture
  */
uint8_t DataCheck(uint8_t **p)
{
    static uint16_t Length;
    static uint8_t ShortData[256]; //碎片缓冲区
    if((*p)[0] < 44) {
        uint8_t cnt;
        if(((FrameHeader *)&((*p)[1]))->SOF == 0xA5) {
            Length = 0;
            if(((FrameHeader *)&((*p)[1]))->DataLength == (*p)[0] - 9)
                return 1;
        }
        for(cnt = 0; cnt < (*p)[0]; cnt++)
            ShortData[Length + cnt +1] = (*p)[cnt+1];
        Length += (*p)[0];
        if(Length == 44||Length == 56||Length == 69||Length >= 88) {
            ShortData[0] = Length;
            if(Length == 44||Length == 56||Length == 69)
                *p = &ShortData[0];
            Length = 0;
            return 1;
        } else
            return 0;
    } else
        return 1;
}

/**
  * @brief   CRC8 data check.
  * @param   *pchMessage:Data to be processed
              dwLength:Length of check data
              ucCRC8:Data after processing
  * @retval  	Gets the CRC8 checksum
  */
unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage,unsigned int dwLength,unsigned char ucCRC8)
{
    unsigned char ucIndex;
    while (dwLength--) {
        ucIndex = ucCRC8^(*pchMessage++);
        ucCRC8 = CRC8_TAB[ucIndex];
    }
    return(ucCRC8);
}
/**
  * @brief   CRC16 data check.
  * @param   *pchMessage:Data to be processed
             dwLength:Length of check data
             ucCRC8:Data after processing
  * @retval  Gets the CRC16 checksum
  */
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC)
{
    uint8_t chData;
    if (pchMessage == NULL) {
        return 0xFFFF;
    }
    while(dwLength--) {
        chData = *pchMessage++;
        (wCRC) = ((uint16_t)(wCRC) >> 8) ^ wCRC_Table[((uint16_t)(wCRC) ^ (uint16_t)(chData)) & 0x00ff];
    }
    return wCRC;
}

///**
//  * @brief  Transfer user system data to the server through huart6
//  * @param  data1,data2,data3,data4:data to send
//  * @retval void
//  */
//void CV_ToServer(UART_HandleTypeDef *huart, float data1,float data2,float data3,uint8_t data4)
//{
//    uint8_t *pMsg=Temp;
//    ((FrameHeader*)pMsg)->SOF = 0xA5;
//    ((FrameHeader*)pMsg)->DataLength = 19;
//    ((FrameHeader*)pMsg)->Seq = se;
//    ((FrameHeader*)pMsg)->CRC8 = Get_CRC8_Check_Sum(Temp,4,0xff);
//    ((FrameHeader*)pMsg)->CmdID = 0x0301;
//    ((client_custom_data_t*)(pMsg+7))->data_cmd_id=0xD180;
//    ((client_custom_data_t*)(pMsg+7))->send_ID=6;
//    ((client_custom_data_t*)(pMsg+7))->receiver_ID=0x0106;
//    ((client_custom_data_t*)(pMsg+7))->data1=data1;
//    ((client_custom_data_t*)(pMsg+7))->data2=data2;
//    ((client_custom_data_t*)(pMsg+7))->data3=data3;
//    ((client_custom_data_t*)(pMsg+7))->masks=data4;

//    *(__packed short *)(&Temp[26]) = Get_CRC16_Check_Sum(Temp,26,0xffff);//默认是小端模式

//    HAL_UART_Transmit_DMA(huart,Temp,28);
//    se++;
//    if(se>255)se = 0;
//}

///**
//  * @brief  Transfer user system data to the server through huart6
//  * @param  data1,data2,data3,data4:data to send
//  * @retval void
//  */
//void CV_ToOtherRobot(UART_HandleTypeDef *huart, uint8_t data1)
//{
//    uint8_t *pMsg = Temp;
//    ((FrameHeader*)pMsg)->SOF = 0xA5;
//    ((FrameHeader*)pMsg)->DataLength = 6 + 1;
//    ((FrameHeader*)pMsg)->Seq = se2;
//    ((FrameHeader*)pMsg)->CRC8 = Get_CRC8_Check_Sum(Temp,4,0xff);
//    ((FrameHeader*)pMsg)->CmdID = 0x0301;
//    ((robot_interactive_data_t*)(pMsg+7))->data_id=0x256;
//  
//    if(GameRobotState.robot_id == 7) {
//        ((robot_interactive_data_t*)(pMsg+7))->send_ID=7;				//红色方
//        ((robot_interactive_data_t*)(pMsg+7))->receiver_ID=6;
//    } else if(GameRobotState.robot_id == 17) {
//        ((robot_interactive_data_t*)(pMsg+7))->send_ID=17;				//蓝色方
//        ((robot_interactive_data_t*)(pMsg+7))->receiver_ID=16;
//    }
//    ((robot_interactive_data_t*)(pMsg+7))->data = data1;

//    *(__packed short *)(&Temp[14]) = Get_CRC16_Check_Sum(Temp,14,0xffff);//默认是小端模式
//    HAL_UART_Transmit_DMA(huart,Temp,16);
//        se2++;
//        if(se2>255)se2 = 0;
//}

/**
  * @brief  Receive and handle referee system data
  * @param  void
  * @retval void
  */
void RefereeHandle(uint8_t *data_buf)
{
    static uint16_t shoot_count = 0;
//    if(data_buf[48] == 0xA5 && data_buf[53] == 01 && data_buf[54] == 02) {
//        GameRobotState = *(ext_game_robot_state_t*)(&data_buf[55]);
//    }
    switch(((FrameHeader *)data_buf)->CmdID) {
    case GameRobotState_ID: {
        GameRobotState = *(ext_game_robot_state_t*)(&data_buf[7]);
    }
    break;
    case PowerHeatData_ID:{
        PowerHeatData = *(ext_power_heat_data_t*)(&data_buf[7]);
        //Send_HeatData();                //Not used in Engineer
    }
    break;
    case RobotHurt_ID: {    //0x0206
        RobotHurt = *(ext_robot_hurt_t*)(&data_buf[7]);
    }
    break;
    case ShootData_ID: {    //0x0207
        ShootData = *(ext_shoot_data_t*)(&data_buf[7]);
        shoot_count++;
        //Send_ShootData(shoot_count);    //Not used in Engineer
    }
    break;
    case StudentInteractiveHeaderData_ID: {
        RobotInteractiveData = *(robot_interactive_data_t*)(&data_buf[7]);
    }
    break;
    default:
        break;
    }
}

void unPackDataFromRF(uint8_t *data_buf, uint32_t length)
{
    /*初始化读取状态*/
    buff_read_index = 0;
    memcpy(RFdataBuff,data_buf,length);
    /*从头开始读取 */
    read_len=length;

    while (read_len--) {
        byte = RFdataBuff[buff_read_index++];
        switch(unpack_step) {
        case STEP_HEADER_SOF: {
            if(byte == START_ID) {
                unpack_step = STEP_LENGTH_LOW;
                protocol_packet[index++] = byte;
            } else {
                index = 0;
            }
        }
        break;

        case STEP_LENGTH_LOW: {
            data_len = byte;
            protocol_packet[index++] = byte;
            unpack_step = STEP_LENGTH_HIGH;
        }
        break;

        case STEP_LENGTH_HIGH: {
            data_len |= (byte << 8);
            protocol_packet[index++] = byte;
            if(data_len < (PROTOCAL_FRAME_MAX_SIZE - HEADER_LEN - CRC_ALL_LEN)) {
                unpack_step = STEP_FRAME_SEQ;
            } else {
                unpack_step = STEP_HEADER_SOF;
                index = 0;
            }
        }
        break;

        case STEP_FRAME_SEQ: {
            protocol_packet[index++] = byte;
            unpack_step = STEP_HEADER_CRC8;
        }
        break;

        case STEP_HEADER_CRC8: {
            protocol_packet[index++] = byte;

            if (index == HEADER_LEN+1) {
                if ( Get_CRC8_Check_Sum(protocol_packet, HEADER_LEN,0xff)== protocol_packet[HEADER_LEN]) {
                    unpack_step = STEP_DATA_CRC16;
                } else {
                    unpack_step = STEP_HEADER_SOF;
                    index = 0;
                }
            }
        }
        break;

        case STEP_DATA_CRC16: {
            if (index < (HEADER_LEN + CMD_LEN + data_len + CRC_ALL_LEN)) {
                protocol_packet[index++] = byte;
            }
            if (index >= (HEADER_LEN + CMD_LEN + data_len + CRC_ALL_LEN)) {

                CRC16_Function=Get_CRC16_Check_Sum(protocol_packet, HEADER_LEN + CMD_LEN + data_len +CRC_8_LEN,0xffff);
                CRC16_Referee=* (__packed short *)(&protocol_packet[index-2]);
                if ( CRC16_Function==CRC16_Referee) {
                    RefereeHandle(protocol_packet);
                }
                unpack_step = STEP_HEADER_SOF;
                index = 0;
            }
        }
        break;

        default: {
            unpack_step = STEP_HEADER_SOF;
            index = 0;
        }
        break;
        }
    }
}

uint8_t RobotInteractiveHandle(robot_interactive_data_t* RobotInteractiveData_t)
{
    if(GameRobotState.robot_id == RobotInteractiveData_t->receiver_ID && GameRobotState.robot_id != 0) {
        if(RobotInteractiveData_t->data_id == 0x255) {
            return RobotInteractiveData_t->data;
        }
        else return 0;
    } else return 0;
}

/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
