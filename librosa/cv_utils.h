//
// Created by PKing on 2023/8/2.
//

#ifndef LIBROSACPP_CV_UTILS_H
#define LIBROSACPP_CV_UTILS_H

#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"

using namespace std;


extern "C"
{
    __const unsigned short int** __wrap___ctype_b(void)
    {
        return __ctype_b_loc();
    }

    __const unsigned short int** __ctype_b(void)
    {
        return __ctype_b_loc();
    }

}

void print_feature(string name, vector<vector<float>> v)
{
    printf("------------------------%s------------------------\n", name.c_str());
    for (int i = 0; i < v.size(); ++i)
    {
        printf("data[%03d,:]=[", i);
        for (int j = 0; j < v[i].size(); ++j)
        {
            printf("%11.7f,", v[i][j]);
        }
        printf("]\n");
    }

}

void print_vector(string name, vector<float> v)
{
    printf("------------------------%s------------------------\n", name.c_str());
    for (int i = 0; i < v.size(); ++i)
    {
        printf("%11.7f,", v[i]);
    }
}


/***
 * 转为以为向量
 * @param v
 * @return
 */
vector<float> get_vector(vector<vector<float>> v)
{
    vector<float> dst;
    int w = (int)v.size();
    int h = (int)v[0].size();
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            dst.push_back(v[j][i]);
        }
    }
    return dst;
}

/***
 *  Mat转vector
 * @tparam _Tp
 * @param mat
 * @return
 */
template<typename _Tp>
vector<_Tp> mat2vector(const cv::Mat& mat)
{
    return (vector<_Tp>) (mat.reshape(1, 1));//通道数不变，按行转为一行
}

/***
 * vector转Mat
 * @tparam _Tp
 * @param v
 * @param channels
 * @param rows
 * @return
 */
template<typename _Tp>
cv::Mat vector2mat(vector<_Tp> v, int channels, int rows)
{
    cv::Mat mat = cv::Mat(v);//将vector变成单列的mat
    cv::Mat dest = mat.reshape(channels, rows).clone();//PS：必须clone()一份，否则返回出错
    return dest;
}

void image_show(string name, cv::Mat& image, int waitKey)
{
    cv::namedWindow(name, cv::WINDOW_NORMAL);
    cv::Mat imgMat = image.clone();
    if (imgMat.channels() == 1)
    {
        cvtColor(imgMat, imgMat, cv::COLOR_GRAY2RGB);
    }
    //char str[200];
    //sprintf(str, ",Size:%dx%d", image.rows, image.cols);
    //RESIZE(img_show, 400);
    cv::imshow(name, imgMat);
    cv::waitKey(waitKey);
}


void image_write(string filepath, cv::Mat& image)
{
    cv::Mat imgMat = image.clone();

    printf("imgMat.channels():%d", imgMat.channels());
    if (imgMat.channels() == 1)
    {
        cvtColor(imgMat, imgMat, cv::COLOR_GRAY2RGB);
    }

    for (int row = 0; row < imgMat.rows; ++row)
    {
        for (int col = 0; col < imgMat.cols; ++col)
        {
            cv::Vec3b& pixel = imgMat.at<cv::Vec3b>(row, col); // 获取当前像素点的BGR值

            pixel = cv::Vec3b(0, 0, 255);

            //printf("row      = %d,", row);
            //printf("col      = %d,", col);
            //printf("pixel[0] = %d,", pixel[0]);
            //printf("pixel[1] = %d,", pixel[1]);
            //printf("pixel[2] = %d\n", pixel[2]);

            //if (col == 25)
            //{
            //    pixel = cv::Vec3b(0, 0, 255);

            //}

            //// 检查是否为白色像素
            //if (pixel == cv::Vec3b(215, 15, 56))
            //{
            //    // 将白色像素更改为红色
            //    pixel = cv::Vec3b(0, 0, 255);
            //}
        }
    }

    //char str[200];
    //sprintf(str, ",Size:%dx%d", image.rows, image.cols);
    //RESIZE(img_show, 400);
    cv::imwrite(filepath, imgMat * 255);
}

#endif //LIBROSACPP_CV_UTILS_H
