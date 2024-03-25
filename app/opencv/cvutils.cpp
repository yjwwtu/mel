#include "cvutils.h"


/// <summary>
/// 打印显示梅尔频谱值
/// </summary>
/// <param name="name">显示标志</param>
/// <param name="v">参数</param>
/// <returns></returns>
int CvUtils::PrintFeature(const char* name, std::vector<std::vector<float>> v)
{
    printf("------------------------%s------------------------\n", name);
    for (int i = 0; i < v.size(); ++i)
    {
        printf("data[%03d,:]=[", i);
        for (int j = 0; j < v[i].size(); ++j)
        {
            printf("%11.7f,", v[i][j]);
        }
        printf("]\n");
    }

    return 0;
}


/// <summary>
/// 打印显示梅尔频谱值
/// </summary>
/// <param name="name">显示标志</param>
/// <param name="v"></param>
/// <returns></returns>
int CvUtils::PrintVector(const char* name, std::vector<float> v)
{
    printf("------------------------%s------------------------\n", name);
    for (int i = 0; i < v.size(); ++i)
    {
        printf("%11.7f,", v[i]);
    }

    return 0;
}

/// <summary>
/// 二维数组转一维数组,从上至下,从左至右依次
/// </summary>
/// <param name="v"></param>
/// <returns></returns>
std::vector<float> CvUtils::Conver2DToVector(const std::vector<std::vector<float>>& v)
{
    std::vector<float> dst;

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



/// <summary>
/// Mat转vector
/// </summary>
/// <typeparam name="_Tp"></typeparam>
/// <param name="mat"></param>
/// <returns></returns>
std::vector<float> CvUtils::Mat2Vector(const cv::Mat& mat)
{
    return (std::vector<float>) (mat.reshape(1, 1));//通道数不变，按行转为一行
}

/// <summary>
/// vector转Mat
/// </summary>
/// <typeparam name="_Tp"></typeparam>
/// <param name="v">参数值</param>
/// <param name="channels">通道数</param>
/// <param name="rows">行数</param>
/// <returns></returns>
cv::Mat CvUtils::Vector2Mat(std::vector<float> v, int channels, int rows)
{
    cv::Mat mat = cv::Mat(v);//将vector变成单列的mat
    cv::Mat dest = mat.reshape(channels, rows).clone();//PS：必须clone()一份，否则返回出错
    return dest;
}
