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
  * @attention
  * 
  * if you had modified this file, please make sure your code does not have many 
  * bugs, update the version NO., write dowm your name and the date, the most
  * important is make sure the users will have clear and definite understanding 
  * through your new brief.
  ******************************************************************************
  */
#ifndef __REFEREE_H__
#define __REFEREE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "stdint.h"
#include  <string.h>

/* Private define ------------------------------------------------------------*/
#define REFEREE_SOF    0xA5

/* Private variables ---------------------------------------------------------*/
extern float EnergyByRF;
/* Private type --------------------------------------------------------------*/
typedef __packed struct {   //裁判系统数据结构
    uint8_t SOF;
    uint16_t DataLength;
    uint8_t Seq;
    uint8_t CRC8;
    uint16_t CmdID;
} FrameHeader;

/* 裁判系统主控数据接收部分 */
typedef enum {
    GameState_ID                    = 0x0001,
    GameResult_ID                   = 0x0002,
    GameRobotHP_ID                  = 0x0003,
    EventData_ID                    = 0x0101,
    SupplyProjectileAction_ID       = 0x0102,
    SupplyProjectileBooking_ID      = 0x0103,
    RefereeWarning_ID               = 0x0104,
    GameRobotState_ID               = 0x0201,
    PowerHeatData_ID                = 0x0202,
    GameRobotPos_ID                 = 0x0203,
    BuffMusk_ID                     = 0x0204,
    AerialRobotEnergy_ID            = 0x0205,
    RobotHurt_ID                    = 0x0206,
    ShootData_ID                    = 0x0207,
    BulletRemaining_ID              = 0x0208,
    StudentInteractiveHeaderData_ID	= 0x0301,
} RefereeSystemID_t;

typedef __packed struct {   //比赛状态数据：0x0001  1Hz
    uint8_t game_type : 4;
    uint8_t game_progress : 4;
    uint16_t stage_remain_time;
} ext_game_state_t;

typedef __packed struct {   //比赛结果数据：0x0002	比赛结束后发送
    uint8_t winner;
} ext_game_result_t;

typedef __packed struct {   //机器人血量数据：0x0003	1Hz
    uint16_t red_1_robot_HP;
    uint16_t red_2_robot_HP;
    uint16_t red_3_robot_HP;
    uint16_t red_4_robot_HP;
    uint16_t red_5_robot_HP;
    uint16_t red_7_robot_HP;
    uint16_t red_base_HP;
    uint16_t blue_1_robot_HP;
    uint16_t blue_2_robot_HP;
    uint16_t blue_3_robot_HP;
    uint16_t blue_4_robot_HP;
    uint16_t blue_5_robot_HP;
    uint16_t blue_7_robot_HP;
    uint16_t blue_base_HP;
} ext_game_robot_HP_t;

typedef __packed struct {   //场地事件数据：0x0101	事件改变发送
    uint32_t event_type;
} ext_event_data_t;

typedef __packed struct {   //补给站动作标识：0x0102	动作改变后发送
    uint8_t supply_projectile_id;
    uint8_t supply_robot_id;
    uint8_t supply_projectile_step;
} ext_supply_projectile_action_t;

typedef __packed struct {   //补给站预约子弹：0x0103	上限10Hz
    uint8_t supply_projectile_id;
    uint8_t supply_num;
} ext_supply_projectile_booking_t;

typedef __packed struct {   //裁判警告信息：0x0104	警告发生后发送
    uint8_t level;
    uint8_t foul_robot_id;
} ext_referee_warning_t;

typedef __packed struct {   //比赛机器人状态：0x0201	10Hz
    uint8_t robot_id;
    uint8_t robot_level;
    uint16_t remain_HP;
    uint16_t max_HP;
    uint16_t shooter_heat0_cooling_rate;
    uint16_t shooter_heat0_cooling_limit;
    uint16_t shooter_heat1_cooling_rate;
    uint16_t shooter_heat1_cooling_limit;
    uint8_t mains_power_gimbal_output : 1;
    uint8_t mains_power_chassis_output : 1;
    uint8_t mains_power_shooter_output : 1;
} ext_game_robot_state_t;

typedef __packed struct {   //实时功率热量数据：0x0202	50Hz
    uint16_t chassis_volt;
    uint16_t chassis_current;
    float chassis_power;
    uint16_t chassis_power_buffer;
    uint16_t shooter_heat0;
    uint16_t shooter_heat1;
} ext_power_heat_data_t;

typedef __packed struct {   //机器人位置：0x0203	10Hz
    float x;
    float y;
    float z;
    float yaw;
} ext_game_robot_pos_t;

typedef __packed struct {   //机器人增益：0x0204	状态改变后发送
    uint8_t power_rune_buff;
} ext_buff_musk_t;

typedef __packed struct {   //空中机器人能量状态：0x0205	10Hz
    uint8_t energy_point;
    uint8_t attack_time;
} aerial_robot_energy_t;

typedef __packed struct {   //伤害状态：0x0206	收到伤害后发送
    uint8_t armor_id : 4;
    uint8_t hurt_type : 4;
} ext_robot_hurt_t;

typedef __packed struct {   //实时射击信息：0x0207	射击后发送
    uint8_t bullet_type;
    uint8_t bullet_freq;
    float bullet_speed;
} ext_shoot_data_t;

typedef __packed struct {   //子弹剩余发射数：0x0208	1Hz
    uint16_t bullet_remaining_num;
} ext_bullet_remaining_t;

/* 裁判系统客户端交互部分 */
typedef __packed struct {   //交互数据接收信息：0x0301	10Hz
    uint16_t data_cmd_id;
    uint16_t send_ID;
    uint16_t receiver_ID;
} ext_student_interactive_header_data_t;

typedef __packed struct {   //客户端自定义数据：0x0301	内容ID：0xD180 10Hz
    uint16_t data_cmd_id;   //数据段的内容 ID：0xD180
    uint16_t send_ID;       //发送者ID
    uint16_t receiver_ID;   //接收者的 ID
    float data1;
    float data2;
    float data3;
    uint8_t masks;
} client_custom_data_t;

typedef __packed struct {   //学生机器人间通信：0x0301	内容ID：0x0201~0x02FF	10Hz
    uint16_t data_id;       //数据段的内容 ID：0x0200-0x02FF
    uint16_t send_ID;       //发送者ID
    uint16_t receiver_ID;   //接收者的 ID
    uint8_t data;           //长度需要小于113个字节
} robot_interactive_data_t;

typedef __packed struct {
    uint16_t data_id;       //数据段的内容 ID：0x0100
    uint16_t send_ID;       //发送者ID
    uint16_t receiver_ID;   //接收者的ID
    uint8_t operate_tpye;
    uint8_t graphic_tpye;
    uint8_t graphic_name[5];
    uint8_t layer;
    uint8_t color;
    uint8_t width;
    uint16_t start_x;
    uint16_t start_y;
    uint16_t radius;
    uint16_t end_x;
    uint16_t end_y;
    int16_t start_angle;
    int16_t end_angle;
    uint8_t text_lenght;
    uint8_t text[30];
} ext_client_graphic_draw_t;

/* 解包用的协议 */
#define START_ID	0XA5
#define PROTOCAL_FRAME_MAX_SIZE	25
#define HEADER_LEN 	4
#define CRC_ALL_LEN	3
#define CRC_8_LEN	1
#define CMD_LEN	2
typedef enum {
    STEP_HEADER_SOF=0,
    STEP_LENGTH_LOW,
    STEP_LENGTH_HIGH,
    STEP_FRAME_SEQ,
    STEP_HEADER_CRC8,
    STEP_DATA_CRC16
} unPackStep_e;


/* Private function declarations --------------------------------------------*/
uint8_t lightSendToRF(void);
void unPackDataFromRF(uint8_t *data_buf, uint32_t length);
uint8_t RobotInteractiveHandle(robot_interactive_data_t* RobotInteractiveData_t);
//void CV_ToOtherRobot(UART_HandleTypeDef *huart, uint8_t data1);

/* Private function prototypes -----------------------------------------------*/
extern ext_game_state_t GameState;
extern ext_game_robot_HP_t GameRobotHP;
extern ext_event_data_t EventData;
extern ext_game_robot_state_t GameRobotState;
extern ext_power_heat_data_t PowerHeatData;
extern ext_robot_hurt_t RobotHurt;
extern ext_shoot_data_t ShootData;
extern ext_bullet_remaining_t BulletRemaining;
extern ext_student_interactive_header_data_t StudentInteractiveHeaderData;
extern robot_interactive_data_t RobotInteractiveData;

#ifdef __cplusplus
}
#endif


#endif
/************************ COPYRIGHT SCUT-ROBOTLAB *****END OF FILE*************/
