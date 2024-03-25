#include "cvutils.h"


/// <summary>
/// ��ӡ��ʾ÷��Ƶ��ֵ
/// </summary>
/// <param name="name">��ʾ��־</param>
/// <param name="v">����</param>
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
/// ��ӡ��ʾ÷��Ƶ��ֵ
/// </summary>
/// <param name="name">��ʾ��־</param>
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
/// ��ά����תһά����,��������,������������
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
/// Matתvector
/// </summary>
/// <typeparam name="_Tp"></typeparam>
/// <param name="mat"></param>
/// <returns></returns>
std::vector<float> CvUtils::Mat2Vector(const cv::Mat& mat)
{
    return (std::vector<float>) (mat.reshape(1, 1));//ͨ�������䣬����תΪһ��
}

/// <summary>
/// vectorתMat
/// </summary>
/// <typeparam name="_Tp"></typeparam>
/// <param name="v">����ֵ</param>
/// <param name="channels">ͨ����</param>
/// <param name="rows">����</param>
/// <returns></returns>
cv::Mat CvUtils::Vector2Mat(std::vector<float> v, int channels, int rows)
{
    cv::Mat mat = cv::Mat(v);//��vector��ɵ��е�mat
    cv::Mat dest = mat.reshape(channels, rows).clone();//PS������clone()һ�ݣ����򷵻س���
    return dest;
}
