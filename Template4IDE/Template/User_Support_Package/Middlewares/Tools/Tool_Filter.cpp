#include "Tool_Filter.h"
#include <stm32f4xx.h>
#include <arm_math.h>
int Middle_filter(int *temp,unsigned char *k, unsigned char num,int current)
{
	q31_t TempBuf[50];
	
	temp[*k] = current;
	(*k)++;
	if ( *k == num  ) *k = 0;

	arm_copy_q31(temp,TempBuf,num);

	bubble_sort(TempBuf,num);
	return TempBuf[(num-1)/2];
}

short Middle_filter(short *temp,unsigned char *k, unsigned char num,short current)
{
	q15_t TempBuf[50];
	temp[*k] = current;
	(*k)++;
	if ( *k == num  ) *k = 0;

	arm_copy_q15(temp,TempBuf,num);

	bubble_sort(TempBuf,num);
	
	return TempBuf[(num-1)/2];
}

float Middle_filter(float *temp,unsigned char *k, unsigned char num,float current)
{
	float32_t TempBuf[50];

	temp[*k] = current;
	(*k)++;
	if ( *k == num  ) *k = 0;

	arm_copy_f32(temp,TempBuf,num);

	bubble_sort(TempBuf,num);
	return TempBuf[(num-1)/2];
}


int Slider_Filter(int *temp,unsigned char *i, unsigned char num,int current)
{
	q31_t sum=0;

	temp[*i] = current;
	(*i)++;
	if ( *i == num  ) *i = 0;

	arm_mean_q31(temp, num, &sum);
	return sum;
}


short Slider_Filter(short *temp,unsigned char *i, unsigned char num,short current)
{
  q15_t sum=0;

  temp[*i] = current;
  (*i)++;
  if ( *i == num  ) *i = 0;

  arm_mean_q15(temp, num, &sum);
  return sum;
}
float Slider_Filter(float *temp,unsigned char *i, unsigned char num,float current)
{
	float32_t sum=0;

	temp[*i] = current;
	(*i)++;
	if ( *i == num  ) *i = 0;

	arm_mean_f32(temp, num, &sum);
	return sum;
}

/*------------------------------------ 以下为19年新增段落 ------------------------------*/
/**********************均值滤波*************************/

meanFilter::meanFilter():
    init_flag(false),
    length(10),
    index_current(0)
{
}

void meanFilter::operator<< (const float& input_num)
{
    if (this->init_flag)
    {
        list[index_current] = input_num;
        index_current = (index_current+1)%length;
    }
    else
    {
        init_flag = true;
        for (int i = 0; i < length; i++)
        {
            this->list[i] = input_num;
        }
    }
    
}

void meanFilter::operator>> (float& output_num)
{
    double sum = 0;
    for (int i = 0; i < length; i++)
    {
        sum += this->list[i];
    }
    output_num = sum / length;
}

/**********************中值滤波*************************/

medianFilter::medianFilter():
    length(7),
    init_flag(false)
{
    for (int i = 0; i < length; i++)
    {
        list[i] = 0;
    }
}

void medianFilter::operator << (float input_num)
{
    if (init_flag)
    {
        if (input_num < list[(length + 1)/2])
        {
            list[length - 1] = input_num; 
            for (int i = length-1; i >= 1; i--)
            {
                if (list[i] < list[i-1])
                {
                    double temp = list[i-1];
                    list[i-1] = list[i];
                    list[i] = temp;
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            list[0] = input_num;
            for (int i = 0; i < length-1; i++)
            {
                if (list[i] > list[i+1])
                {
                    double temp = list[i];
                    list[i] = list[i+1];
                    list[i+1] = temp;
                }
                else
                {
                    break;
                }
            }
        }
    }
    else
    {
        init_flag = true;
        for (int i = 0; i < length; i++)
        {
            list[i] = input_num;
        }
    }
    
}

void medianFilter::operator >> (float& output_num)
{
    output_num = list[(length + 1)/2];
}

/************************ COPYRIGHT SCUT-ROBOTLAB *****END OF FILE*************/
