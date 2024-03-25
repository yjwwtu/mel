#ifndef __OPENCV_UTILS_H__
#define __OPENCV_UTILS_H__

#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"

#ifdef MIPS
extern "C"
{
    __const unsigned short int** __attribute__((weak)) __wrap___ctype_b(void)
    {
        return __ctype_b_loc();
    }
    __const unsigned short int** __attribute__((weak)) __ctype_b(void)
    {
        return __ctype_b_loc();
    }
}

#endif // MIPS

class CvUtils
{
public:
    static int PrintFeature(const char* name, std::vector<std::vector<float>> v); //打印显示梅尔频谱值
    static int PrintVector(const char* name, std::vector<float> v); //打印显示梅尔频谱值
    static std::vector<float> Conver2DToVector(const std::vector<std::vector<float>>& v); //二维数组转一维数组
    static std::vector<float> Mat2Vector(const cv::Mat& mat); //矩阵转Vector
    static cv::Mat Vector2Mat(std::vector<float> v, int channels, int rows); //Vector转cv矩阵

};


#endif // !__OPENCV_UTILS_H__
