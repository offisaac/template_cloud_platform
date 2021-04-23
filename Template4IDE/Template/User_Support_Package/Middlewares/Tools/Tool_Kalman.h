#ifndef _TOOL_Kalman_H_
/* second-order kalman filter on stm32 */
#include <algorithm>
#include "stm32f4xx_hal.h"
#include "arm_math.h"

/**
 * @brief Mat 矩阵类
 * 模仿OpenCV写法，将arm中自带的矩阵结构体封装成C++类
 */
class Mat : public arm_matrix_instance_f32
{
  public:
    Mat()  { numRows=0; numCols=0; pCount=new uint16_t(1); pData=NULL; }
    Mat(const Mat &obj) {
      this->pCount = obj.pCount;
      (*pCount)++;
      arm_mat_init_f32(this, obj.numCols, obj.numRows, obj.pData);
    }
    Mat(uint16_t rows, uint16_t cols, float32_t* _pData) {
      pCount = new uint16_t(1);
      pData = new float32_t[rows*cols];
      memcpy(pData, _pData, rows*cols*4);
      arm_mat_init_f32(this, rows, cols, pData);
    }
    Mat(uint16_t rows, uint16_t cols, float32_t data) { 
      pCount = new uint16_t(1);
      pData = new float32_t[rows*cols];
      for(uint16_t i = 0; i < rows*cols; i++)
        pData[i] = data;
      arm_mat_init_f32(this, rows, cols, pData);
    }
    ~Mat() {
      if ((--(*pCount))==0 && pData!=NULL)
        delete[] pData;
      delete pCount;
    }

    static Mat zeros(uint16_t rows, uint16_t cols) { return Mat(rows,cols,0.0f); }
    static Mat eye(uint16_t rows, uint16_t cols, float32_t num = 1) {
      Mat m = Mat(rows,cols,0.0f);
      for(int i = 0; i < std::max(rows,cols); i++)
        m[i*cols+i] = num;
      return m;
    }
    static Mat diag(float32_t* pData, uint16_t length) {
      Mat m = Mat(length,length,0.0f);
      for(int i = 0; i < length; i++)
        m[i*length+i] = pData[i];
      return m;
    }

    float32_t& operator[](int idx) { return this->pData[idx]; }
    Mat operator+(const Mat &other) { Mat result; arm_mat_add_f32(this, &other, &result); return result; }
    Mat operator-(const Mat &other) { Mat result; arm_mat_sub_f32(this, &other, &result); return result; }
    Mat operator*(const Mat &other) { Mat result; arm_mat_mult_f32(this, &other, &result); return result; }
    Mat trans() { Mat result; arm_mat_trans_f32(this, &result); return result; }
    Mat inverse() { Mat result; arm_mat_inverse_f32(this, &result); return result; }
  private:
    uint16_t* pCount; //pData引用计数器
};

class KalmanFilter
{
  public:
    KalmanFilter(uint16_t stateDim, uint16_t measureDim, uint16_t controlDim = 0, float32_t processErr = 1e-2, float32_t measureErr = 1e-1);
	  void init(float *state, float error);
    float32_t* predict(float32_t* control);
    float32_t* correct(float32_t* measurement);

    Mat xhat;      //后验状态估计（当前估计值）
    Mat xhatminus; //先验状态估计（未来预测值）
    Mat A;         //状态转换模型（转移矩阵）
    Mat B;         //控制转换模型（控制矩阵）
    Mat Q;         //过程噪声协方差（由环境因素引入的噪声）
    Mat H;         //状态变量到测量值的转换矩阵（测量矩阵）
    Mat R;         //测量噪声协方差（传感器误差）
    Mat P;         //后验误差协方差（当前估计误差）
    Mat Pminus;    //先验误差协方差（未来预测误差）
    Mat K;         //卡尔曼增益
    Mat u;         //控制量（控制器数据）
    Mat z;         //测量值（传感器数据）

  private:
    Mat HT; //H.trans
    Mat AT; //A.trans
};

#endif
