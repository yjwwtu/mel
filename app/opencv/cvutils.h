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
    static int PrintFeature(const char* name, std::vector<std::vector<float>> v); //��ӡ��ʾ÷��Ƶ��ֵ
    static int PrintVector(const char* name, std::vector<float> v); //��ӡ��ʾ÷��Ƶ��ֵ
    static std::vector<float> Conver2DToVector(const std::vector<std::vector<float>>& v); //��ά����תһά����
    static std::vector<float> Mat2Vector(const cv::Mat& mat); //����תVector
    static cv::Mat Vector2Mat(std::vector<float> v, int channels, int rows); //Vectorתcv����

};


#endif // !__OPENCV_UTILS_H__
