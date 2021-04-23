/**
 ********************************************************
 * Copyright (c) 2019-2019, RobotLab Development Team
  * @file   ： rs_ringbuffer.cpp
  * @brief  ： Single ring buffer for communiction service.
 *  @Changes：
 *  Date           Author   Version    Notes
 *  2019-06-13     Manny    0.0.1      first version
 ********************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "rs_ringbuffer.h"

/* Functions -----------------------------------------------------------------*/
/**
* @brief  写入数据到环形缓冲区
* @param  pBuf: 待写入数据缓冲区
*              len:    待写入数据长度
* @retval 执行状态
*/
bool rs_WriteDataToRingBuffer(ringBufType_t *pRingBuf, uint8_t *pBuf, uint8_t len)
{
    if (pRingBuf == NULL) { return false; }
    if (pBuf == NULL) { return false; }
    if (len <= 0) { return false; }

    if (len > (COUNTOF(pRingBuf->ringBuf) - pRingBuf->dataLen)) { return false; }                   /* 写入会导致缓冲区溢出 */
    if (pRingBuf->frameNum >= COUNTOF(pRingBuf->aFrameLen)) { return false; }                       /* 再写入会导致桢长存储缓冲区溢出 */
    if (pRingBuf->readWriteMutexFlag) { return false; }                                             /* 读写互斥 */

    uint32_t tmp = 0;

    pRingBuf->readWriteMutexFlag = true;                                                            /*打开互斥标志*/
    pRingBuf->aFrameLen[pRingBuf->frameNum] = len;                                                  /*存储桢长*/
    
    /* 剩余缓存足够装载所有内容 */
    if ((COUNTOF(pRingBuf->ringBuf) - pRingBuf->tail) >= pRingBuf->aFrameLen[pRingBuf->frameNum])
    {
        memcpy(&(pRingBuf->ringBuf[pRingBuf->tail]), pBuf, pRingBuf->aFrameLen[pRingBuf->frameNum]);
        pRingBuf->tail += pRingBuf->aFrameLen[pRingBuf->frameNum];
        
        /* 如果满了一圈则回到"头部",这个判断很少生效 */
        if (pRingBuf->tail == COUNTOF(pRingBuf->ringBuf))
        {
            pRingBuf->tail = 0;
        }
    }
    /* 剩余缓存不足以装载所有内容，分开两部分装载 */
    else
    {
        memcpy(&(pRingBuf->ringBuf[pRingBuf->tail]), pBuf, COUNTOF(pRingBuf->ringBuf) - pRingBuf->tail);
        tmp = COUNTOF(pRingBuf->ringBuf) - pRingBuf->tail;

        pRingBuf->tail = 0;
        memcpy(&(pRingBuf->ringBuf[pRingBuf->tail]), &pBuf[tmp], pRingBuf->aFrameLen[pRingBuf->frameNum] - tmp);
        pRingBuf->tail = pRingBuf->aFrameLen[pRingBuf->frameNum] - tmp;
    }
    
    pRingBuf->dataLen += pRingBuf->aFrameLen[pRingBuf->frameNum];                                       /*更新存储数据长度*/
    pRingBuf->frameNum++;                                                                               /*更新帧数*/

    pRingBuf->readWriteMutexFlag = false;/*关闭互斥标志*/

    return true;
}

/**
* @brief  从环形缓冲区读出数据
* @param  pBuf: 存放读出数据缓冲区
*              len:    存放读出数据缓冲区长度
* @retval 实际读出数据量
*/
uint8_t rs_ReadDataFromRingBuffer(ringBufType_t *pRingBuf, uint8_t *pBuf, uint8_t len)
{
    if (pRingBuf == NULL) { return 0x00; }
    if (pBuf == NULL) { return 0x00; }
    
    if (len < pRingBuf->aFrameLen[0]) { return 0x00; }                                                  /*缓冲区过小*/
    if (pRingBuf->dataLen <= 0) { return 0x00; }                                                        /*无数据可读*/
    if (pRingBuf->readWriteMutexFlag) { return 0x00; }                                                  /*读写互斥*/

    uint32_t ret = 0;
    uint32_t tmp = 0;

    pRingBuf->readWriteMutexFlag = true;                                                                /*打开互斥标志*/
    pRingBuf->dataLen -= pRingBuf->aFrameLen[0];                                                        /*更新存储数据长度*/
    ret = pRingBuf->aFrameLen[0];                                                                       /*获取帧长*/

    /* 如果不满一圈则直接读取 */
    if ((COUNTOF(pRingBuf->ringBuf) - pRingBuf->head) >= pRingBuf->aFrameLen[0])
    {
        memcpy(pBuf, &(pRingBuf->ringBuf[pRingBuf->head]), pRingBuf->aFrameLen[0]);
        pRingBuf->head += pRingBuf->aFrameLen[0];
        /*如果满了一圈则回到"尾部"*/
        if (pRingBuf->head == COUNTOF(pRingBuf->ringBuf))
        {
            pRingBuf->head = 0;
        }
    }
    /*满了一圈则分成两部分读取*/
    else
    {
        memcpy(pBuf, &(pRingBuf->ringBuf[pRingBuf->head]), COUNTOF(pRingBuf->ringBuf) - pRingBuf->head);
        tmp = COUNTOF(pRingBuf->ringBuf) - pRingBuf->head;
        pRingBuf->head = 0;
        memcpy(&pBuf[tmp], &(pRingBuf->ringBuf[pRingBuf->head]), pRingBuf->aFrameLen[0] - tmp);
        pRingBuf->head = pRingBuf->aFrameLen[0] - tmp;
    }

    for (uint8_t i = 0; i < (pRingBuf->frameNum - 1); i++)
    {
        /*让未读缓冲区桢长始终在最前面*/
        pRingBuf->aFrameLen[i] = pRingBuf->aFrameLen[i + 1];
    }
    pRingBuf->aFrameLen[pRingBuf->frameNum - 1] = 0x00;/*帧长缓冲区数据左移后补 0*/

    pRingBuf->frameNum--;                                                                               /*读取后帧数减 1*/

    pRingBuf->readWriteMutexFlag = false;                                                               /*关闭互斥标志*/

    return ret;
}

/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
