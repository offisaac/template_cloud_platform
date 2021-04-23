#pragma once

#ifdef __cplusplus
#include "drv_timer.h"
#include <arm_math.h> /* ffabs */
#include <stdint.h>
#include "Filter.h" /* LowPassFilter */

class OpenLoop
{
public:
    OpenLoop(float gain = 1.0f) : Gain(gain) {}

    float Adjust()
    {
        Out = Gain * Target;
        return Out;
    }
    float Target = 0, Current = 0;
    float Out = 0;

    float Gain = 1.0f;
};

class myPIDTimer
{
protected:
    float dt;
    uint32_t last_time; //上次系统时间记录
    uint8_t UpdataTimeStamp(void);
};

class myPID : public myPIDTimer
{
public:
    myPID() {}
    myPID(float _Kp, float _Ki, float _Kd) : Kp(_Kp), Ki(_Ki), Kd(_Kd){}
    void SetPIDParam(float _Kp, float _Ki, float _Kd)
    {
        Kp = _Kp;
        Ki = _Ki;
        Kd = _Kd;
    };
    float Adjust();
    float Target = 0, Current = 0;
    float Out = 0;

    float Kp = 0, Ki = 0, Kd = 0;

    float I_SeparThresh = 400; //积分分离阈值，需为正数，
                                 //fabs(error)大于该阈值取消积分作用。

    float VarSpeed_I_A = 65535; //变速积分 A，需为正数，
    float VarSpeed_I_B = 65535; //变速积分 B，需为正数，
                                //在 error<=B 的区间内，为普通积分效果，
                                //在 B<error<=A+B 的区间内，为变速积分效果，
                                //在 A+B<error 的区间内，不继续积分。

    float DeadZone = 0; //死区，需为整数，
                        //fabs(error)小于DeadZone时，输出为0；

    LowPassFilter LowPass_error = LowPassFilter(0);
    LowPassFilter LowPass_d_err = LowPassFilter(0); //不完全微分

    bool D_of_Current = false; //启用微分先行
                               //文献中Current多译作Process Variable(PV)

private:
    float pre_error = 0;
    float integral_e = 0;

    float pre_Current = 0;
};

class FuzzyPD : public myPIDTimer
{
public:
    float Adjust();
    float Target = 0, Current = 0;
    float Out = 0;

    float Table[3][3] = {
        /*				d_err_N, d_err_Z, d_err_P */
        /* error_N */ { -200,    -100,    0       },
        /* error_Z */ { -100,    0,       100     },
        /* error_P */ { 0,       100,     200     }
        };

    // N(负数, Negative)
    // Z(零, Zero)
    // P(正数, Positive)
    float error_N = -65535, error_P = 65535;
    float d_err_N = -65535, d_err_P = 65535;

    LowPassFilter LowPass_d_err = LowPassFilter(0);

private:
    float pre_error = 0;
    float Membership(float x, float Negative, float Positive, int POS);
    float Linear(float X, float X0, float X1)
    {
        return (X - X0) / (X1 - X0);
    }
};

#endif
