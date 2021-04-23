/**
 ********************************************************
 * Copyright (c) 2019-2019, RobotLab Development Team
  * @file   ： rs_ringbuffer.h
  * @brief  ： Single ring buffer for communiction service.
 *  @Changes：
 *  Date           Author   Version    Notes
 *  2019-06-13     Manny    0.0.1      first version
 ********************************************************
 */
#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "stdint.h"
#include "string.h"

/* Private  ------------------------------------------------------------------*/
/*数据缓冲区长度(该缓冲区用于通信服务)*/
#define DATA_BUF_LENGTH              512

/*环形缓冲区相关结构体*/
typedef struct {
    volatile uint8_t  head;                                            /* 缓冲区头部 */
    volatile uint8_t  tail;                                            /* 缓冲区尾部 */
    volatile uint8_t  dataLen;                                         /* 缓冲区内数据长度 */
    volatile uint8_t  readWriteMutexFlag;                              /* 读写互斥标志 */
    uint8_t           aFrameLen[40];                                   /* 存储接收帧的长度 */
    volatile uint8_t  frameNum;                                        /* 缓冲区内桢数 */
    uint8_t           ringBuf[DATA_BUF_LENGTH];                        /* 缓冲区 */
}ringBufType_t;

/*多缓冲*/
typedef struct {
    ringBufType_t RingBuf_1;
    ringBufType_t RingBuf_2;
}multRingBufType_t;

#ifndef COUNTOF
    #define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
#endif

/* Exported ------------------------------------------------------------------*/
bool    rs_WriteDataToRingBuffer(ringBufType_t *pRingBuf, uint8_t *pBuf, uint8_t len);
uint8_t rs_ReadDataFromRingBuffer(ringBufType_t *pRingBuf, uint8_t *pBuf, uint8_t len);
  
#ifdef __cplusplus
}
#endif

#endif  

/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
