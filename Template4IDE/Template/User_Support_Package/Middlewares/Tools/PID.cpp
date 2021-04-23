#include "PID.h"

uint8_t myPIDTimer::UpdataTimeStamp(void)
{
  uint32_t now_time;
  //系统时间的获取
  if (last_time == 0)
  {
    last_time = micros();
    return 1;
  }
  now_time = micros();

  if (now_time < last_time)
    dt = (float)(now_time + (0xFFFFFFFF - last_time));
  else
    dt = (float)(now_time - last_time);

  last_time = now_time;

  dt /= 1000000.0f;

  return 0;
}

float myPID::Adjust()
{
  if (UpdataTimeStamp())
    return 0;

  float error = Target - Current;

  if (fabs(error) < DeadZone)
  {
    Out = 0;
    return Out;
  }

  // 低通滤波
  error = LowPass_error.filter(error);

  float P_Term = Kp * error;

  float I_Term = 0;
  // 积分分离判断
  if (fabs(error) < I_SeparThresh)
  {
    // 变速积分系数f
    float I_VarSpeedf = 0;
    if (fabs(error) <= VarSpeed_I_B)
      I_VarSpeedf = 1;
    else if (fabs(error) <= double(VarSpeed_I_A) + VarSpeed_I_B)
      I_VarSpeedf = (VarSpeed_I_A - (fabs(error)) + VarSpeed_I_B) / VarSpeed_I_A;
    
    if(Ki != 0){
      integral_e += I_VarSpeedf * error * dt;
    }
    //if(integral_e * error < 0 ) integral_e = 0;
    
    I_Term = Ki * integral_e;
  }

  float d_err = 0;
  if (D_of_Current)
    d_err = (Current - pre_Current) / dt;
  else
    d_err = (error - pre_error) / dt;
  d_err = LowPass_d_err.filter(d_err);
  float D_Term = Kd * d_err;

  pre_error = error;

  Out = P_Term + I_Term + D_Term;
  // 低通滤波
  return Out;
}

float FuzzyPD::Adjust()
{
  if (UpdataTimeStamp())
    return 0;

  float error = Target - Current;
  float d_err = (error - pre_error) / dt;
  pre_error = error;
  d_err = LowPass_d_err.filter(d_err);

  Out = 0;
  float ms_sum = 0;
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      float ms_error = Membership(error, error_N, error_P, i);
      float ms_d_err = Membership(d_err, d_err_N, d_err_P, j);
      float ms = ms_error * ms_d_err;
      ms_sum += ms;
      Out += ms * Table[i][j];
    }
  }
  Out /= ms_sum;
  return Out;
}

float FuzzyPD::Membership(float x, float Negative, float Positive, int POS)
{
  if (x < Negative)
  {
    if (POS == 0)
      return 1;
    else
      return 0;
  }
  else if (x < 0)
  {
    if (POS == 0)
      return (1 - Linear(x, Negative, 0));
    else if (POS == 1)
      return Linear(x, Negative, 0);
    else
      return 0;
  }
  else if (x < Positive)
  {
    if (POS == 1)
      return (1 - Linear(x, 0, Positive));
    else if (POS == 2)
      return Linear(x, 0, Positive);
    else
      return 0;
  }
  else
  {
    if (POS == 2)
      return 1;
    else
      return 0;
  }
}
