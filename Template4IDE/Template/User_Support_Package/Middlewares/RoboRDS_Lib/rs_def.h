/**
 * Copyright (c) 2019-2019, RobotLab Development Team
 *
 *
 * @Changes :
 * Date           Author   Version    Notes
 * 2019-06-13     Manny     0.0.1     first version
   2019-06-22     Mentos    0.0.2     immigration version
 */
 
#ifndef __RS_DEF_H__
#define __RS_DEF_H__

/* include RSconfig header to impoRS configuration */

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @addtogroup Error
 */

/**@{*/

/* RS-Thread error code definitions */
#define RS_EOK                          0               /**< There is no error */
#define RS_ERROR                        1               /**< A generic error happens */
#define RS_ETIMEOUT                     2               /**< Timed out */
#define RS_EFULL                        3               /**< The resource is full */
#define RS_EEMPTY                       4               /**< The resource is empty */
#define RS_ENOMEM                       5               /**< No memory */
#define RS_ENOSYS                       6               /**< No system */
#define RS_EBUSY                        7               /**< Busy */
#define RS_EIO                          8               /**< IO error */
#define RS_EINTR                        9               /**< Interrupted system call */
#define RS_EINVAL                       10              /**< Invalid argument */

/**@}*/
	
/**
 * @addtogroup 位带操作,实现51类似的GPIO控制功能,具体实现思想,参考<<CM3权威指南>>第五章(87页~92页)
 */

/**@{*/
#if USING_F4xx_

/* IO口操作宏定义 */
#define MEM_ADDR(addr)           *((volatile unsigned long  *)(addr))
#define BITBAND(addr, bitnum)    ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))

/* IO口地址映射 */
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) 								/**<0x4001080C */ 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) 								/**<0x40010C0C */
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) 								/**<0x4001100C */ 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) 								/**<0x4001140C */  
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) 								/**<0x4001180C */  
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) 								/**<0x40011A0C */  
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) 								/**<0x40011E0C */  

#define GPIOA_IDR_Addr    (GPIOA_BASE+8)									/**<0x40010808 */   
#define GPIOB_IDR_Addr    (GPIOB_BASE+8)  								/**<0x40010C08 */   
#define GPIOC_IDR_Addr    (GPIOC_BASE+8)  								/**<0x40011008 */   
#define GPIOD_IDR_Addr    (GPIOD_BASE+8)  								/**<0x40011408 */   
#define GPIOE_IDR_Addr    (GPIOE_BASE+8)  								/**<0x40011808 */   
#define GPIOF_IDR_Addr    (GPIOF_BASE+8)  								/**<0x40011A08 */   
#define GPIOG_IDR_Addr    (GPIOG_BASE+8)  								/**<0x40011E08 */   

/* IO口操作,只对单一的IO口! 确保n的值小于16! */
#define PAout(n)   ((n<16) ? (BIT_ADDR(GPIOA_ODR_Addr,n)) :(void)0)  /**<GPIOA output */
#define PBout(n)   ((n<16) ? (BIT_ADDR(GPIOB_ODR_Addr,n)) :(void)0)  /**<GPIOB output */
#define PCout(n)   ((n<16) ? (BIT_ADDR(GPIOC_ODR_Addr,n)) :(void)0)  /**<GPIOC output */
#define PDout(n)   ((n<16) ? (BIT_ADDR(GPIOD_ODR_Addr,n)) :(void)0)  /**<GPIOD output */
#define PEout(n)   ((n<16) ? (BIT_ADDR(GPIOE_ODR_Addr,n)) :(void)0)  /**<GPIOE output */
#define PFout(n)   ((n<16) ? (BIT_ADDR(GPIOF_ODR_Addr,n)) :(void)0)  /**<GPIOF output */
#define PGout(n)   ((n<16) ? (BIT_ADDR(GPIOG_ODR_Addr,n)) :(void)0)  /**<GPIOG output */

#define PAin(n)    ((n<16) ? (BIT_ADDR(GPIOA_IDR_Addr,n)) :(void)0)  /**<GPIOA input */
#define PBin(n)    ((n<16) ? (BIT_ADDR(GPIOB_IDR_Addr,n)) :(void)0)  /**<GPIOB input */
#define PCin(n)    ((n<16) ? (BIT_ADDR(GPIOC_IDR_Addr,n)) :(void)0)  /**<GPIOC input */
#define PDin(n)    ((n<16) ? (BIT_ADDR(GPIOD_IDR_Addr,n)) :(void)0)  /**<GPIOD input */
#define PEin(n)    ((n<16) ? (BIT_ADDR(GPIOE_IDR_Addr,n)) :(void)0)  /**<GPIOE input */
#define PFin(n)    ((n<16) ? (BIT_ADDR(GPIOF_IDR_Addr,n)) :(void)0)  /**<GPIOF input */
#define PGin(n)    ((n<16) ? (BIT_ADDR(GPIOG_IDR_Addr,n)) :(void)0)  /**<GPIOG input */

#endif
/**@}*/

/**
 * @addtogroup daily funcs
 */

/**@{*/
/**
  * @brief  用于求绝对值
  * @param  x 
  * @retval None
  */
#define rs_myabs(x) ((x)>0? (x):(-(x)))

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed. 
  *         If expr is true, it returns no value.
  * @retval None
  */
#ifdef USE_USER_ASSERT
  #define rs_assert_param(expr) ((expr) ? (void)0U : rs_assert_failed((uint8_t *)__FILE__, __LINE__))
  extern void rs_assert_failed(uint8_t* file, uint32_t line);
#else
  #define rs_assert_param(expr) ((expr) ? (void)0U : rs_assert_failed())
  #define rs_assert_failed() (while(1))
#endif

/**@}*/
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/*Unified Communicaition Object(UCOB)*/
#ifndef __UCOBTypeDef_DEFINED
#define __UCOBTypeDef_DEFINED
typedef struct 
{
  char*         address;
  unsigned int  len;
  unsigned char port_num;
}Unified_COB;
#endif

#ifndef __LinkageStatus_DEFINED
#define __LinkageStatus_DEFINED
enum LinkageStatus_Typedef
{
  Connection_Lost = 0U,
  Connection_Established
};
#endif

/*Device management & Communication management Definition*/
typedef struct 
{
  unsigned short start_ID;
  unsigned short end_ID;
}Com_IDDomain;

enum Msg_Type
{
  No_Message = 0U,
  Get_Message
};
  
enum Com_Type
{
  CAN1_com = 0U,
  CAN2_com,
  USART1_com,
  USART2_com,
  USART3_com,
  USART4_com,
  USART5_com,
  USART6_com
};
#endif

#endif
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
