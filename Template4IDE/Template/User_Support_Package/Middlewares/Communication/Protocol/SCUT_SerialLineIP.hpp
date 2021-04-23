/**
  **********************************************************************************
  * @file   : SerialLineIP.hpp
  * @brief  : SerialLineIP is an easy serial protocol for Data-Link-Layer.This file
  *           provide functions for framing and transparent transmission. 
  * @warning:  STANDARD C++03 REQUIRED!
  * @see    : [RFC-1055: SLIP 协议文档](https://tools.ietf.org/html/rfc1055)
  * @author : BigeYoung (SCUT.BigeYoung@gmail.com)
  * @date   : 2018-12-24
  * @version: 1.0
  **********************************************************************************
**/
#ifndef SERIAL_LINE_IP_H
#define SERIAL_LINE_IP_H
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h> 
#include <assert.h>
#include <vector>

/* Private  ------------------------------------------------------------------*/
/* Exported ------------------------------------------------------------------*/
#ifdef  __cplusplus
namespace SerialLineIP
{
/* SLIP special character codes */
const uint8_t END = 0xC0;     /* indicates end of packet */
const uint8_t ESC = 0xDB;     /* indicates byte stuffing */
const uint8_t ESC_END = 0xDC; /* ESC ESC_END means END data byte */
const uint8_t ESC_ESC = 0xDD; /* ESC ESC_ESC means ESC data byte */

/**
 * @brief Serial Line IP PACK
 * @param p_PDU 起始位置指针
 * @param PDU_len PDU 字节长度
 * @return std::vector<uint8_t> SDU
 * @note Service Data Unit (SDU) 指本层封包后产生的数据单元
 *       Protocol Data Unit (PDU) 指上层协议数据单元
 */
std::vector<uint8_t> Pack(const void *const p_PDU, int PDU_len);

/**
 * @brief Serial Line IP UNPACK
 * @param p_SDU 起始位置指针
 * @param SDU_len SDU 字节长度
 * @return std::vector<uint8_t> PDU
 * @note Service Data Unit (SDU) 指本层解包前的数据单元
 *       Protocol Data Unit (PDU) 指上层协议数据单元
 */
std::vector<uint8_t> Unpack(const void *const p_SDU, int SDU_len);
} 
#endif
#endif
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
