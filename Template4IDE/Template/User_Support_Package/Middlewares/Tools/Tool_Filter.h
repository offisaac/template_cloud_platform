#ifndef _TOOL_FILTER_H_
#define _TOOL_FILTER_H_


#ifdef __cplusplus

#define FLOAT_FILTER 1
#define INT_FILTER 0
int Middle_filter(int *temp,unsigned char *i, unsigned char num,int current);
short Middle_filter(short *temp,unsigned char *i, unsigned char num,short current);
float Middle_filter(float *temp,unsigned char *i, unsigned char num,float current);

int Slider_Filter(int *temp,unsigned char *i, unsigned char num,int current);
short Slider_Filter(short *temp,unsigned char *i, unsigned char num,short current);
float Slider_Filter(float *temp,unsigned char *i, unsigned char num,float current);

template<class Type>
Type Constrain(Type input,Type min,Type max){
if (input <= min)
	return min;
else if(input >= max)
	return max;
else return input;
}

template<class T>
void bubble_sort(T a[], int n)
{
    int i,j;
	T temp;
    for (j = 0; j < n - 1; j++)
	{
        for (i = 0; i < n - 1 - j; i++)
        {
            if(a[i] > a[i + 1])
            {
                temp = a[i];
                a[i] = a[i + 1];
                a[i + 1] = temp;
            }
        }
	}
}

/*以下为19年新增段落*/
#define Degree2Radian(angle) (angle*PI/180.0)
#define Redian2Degree(angle) (angle*180.0/PI)


class medianFilter  //中值滤波，队列长度默认为5
{
    public:
        medianFilter();
        void operator << (float input_num);
        void operator >> (float& output_num);

    private:
        bool init_flag;
        float list[7];
        int length;
};

class meanFilter    //均值滤波
{
    public:
        meanFilter();
        void operator << (const float& input_num);
        void operator >> (float& output_num);
    private:
        bool init_flag;
        int length;
        float list[10];
        int index_current;
};

#endif

#endif
