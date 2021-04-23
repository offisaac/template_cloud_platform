#pragma once

#ifdef __cplusplus

template<typename Type>
Type myConstrain(Type input, Type min, Type max)
{
  if (input <= min)
    return min;
  else if (input >= max)
    return max;
  else
    return input;
}

class Filter
{
  public:
    virtual ~Filter(){}
    virtual float filter(float new_val) = 0;
};

class LowPassFilter: Filter
{
  public:
    LowPassFilter(float factor = 0) : Factor(factor){}
    float Factor;	// 范围0-1,
    // Factor越大效果越明显。
    virtual float filter(float new_val) override
    {
      value = (1.0f - Factor) * new_val + Factor * value;
      return value;
    }
  private:
    float value = 0;
};

#endif

