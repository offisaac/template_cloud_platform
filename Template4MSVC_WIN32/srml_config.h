#ifndef __SRML_CONFIG_H__
#define __SRML_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Drivers ----------------------------------------------------*/
#define USE_SRML_REFEREE                  0
#define USE_SRML_MOTOR                    0
#define USE_SRML_DR16                     0
#define USE_SRML_BMX055                   0
#define USE_SRML_MPU6050                  0
  
#define USE_SRML_I2C                      0
#define USE_SRML_CAN                      0
#define USE_SRML_UART                     0
#define USE_SRML_TIMER                    0

/* Middlewares -----------------------------------------------*/
#define USE_SRML_PID                      1
#define USE_SRML_FILTER                   1
#define USE_SRML_MYMAT                    0
#define USE_SRML_KALMAN                   0
#define USE_SRML_TRACK_GENERATOR          0

#define USE_SRML_SERIAL_LINE_IP           1
  
#define USE_SRML_MYASSERT                 0
#define USE_SRML_LIST                     1
#define USE_SRML_SYSLOG                   1

#define USE_SRML_GIMBAL                   0
#define USE_SRML_CHASSIS                  1
#define USE_SRML_POW_CTRL                 0


#ifdef __cplusplus
}
#endif

#endif
/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
