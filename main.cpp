#include <iostream>
#include <vector>
#include <algorithm>
#include <iostream> 
#include <cmath> 
#include <limits>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#include "librosa/audio_utils.h"
#include "librosa/librosa.h"
#include "librosa/cv_utils.h"


/******************************************************************************
*  Name        :   Files::exists
*  Author      :   cqnews
*  Version     :   V1.0.0
*  Data        :   2021.05.07
*  Describe    :   检测文件是否存在
******************************************************************************/
bool exists(const char* name)
{
    try
    {
        struct stat buffer;
        memset(&buffer, 0, sizeof(buffer));
        return (stat(name, &buffer) == 0);

    }
    catch (...)
    {

    }

    return false;
}

// 假设 magnitude 是一个浮点数向量
std::vector<float> power_to_db(const std::vector<float>& magnitude, float ref = 1.0, float amin = 1e-10, float top_db = 80.0)
{
    std::vector<float> db_spec(magnitude.size());

    // 获取最大功率用于top_db参数处理
    float ref_value = *std::max_element(magnitude.begin(), magnitude.end());

    for (size_t i = 0; i < magnitude.size(); ++i)
    {
        float vaule = std::max(magnitude[i], amin);
        float vaule2 = std::max(amin, ref_value);

        // 计算dB值
        db_spec[i] = 10.0 * std::log10(vaule);
        db_spec[i] -= 10.0 * std::log10(vaule2);
    }

    float log_spec_max = *std::max_element(db_spec.begin(), db_spec.end());

    // 应用top_db限制
    if (top_db >= 0.0)
    {
        for (size_t i = 0; i < magnitude.size(); ++i)
        {
            db_spec[i] = std::max(db_spec[i], log_spec_max - top_db);
        }
    }

    return db_spec;
}
int print(const char*);

int main(int argc, char* argv[])
{
    std::string inputPath = "./data/20_16000Hz_5S.wav";

    if (argc >= 2)
    {
        inputPath = argv[1];
    }

    for (size_t i = 0; i < 10; i++)
    {
        print(inputPath.c_str());
    }
}

int print(const char* inputPath)
{
    if (exists(inputPath) == false)
    {
        printf("%s not found.", inputPath);
        return -1;
    }

    clock_t start, end;
    double cpu_time_used;
    start = clock();


    int sr = -1;
    int n_fft = 2048;
    int hop_length = 500;
    int n_mel = 224;
    int fmin = 20;
    int fmax = 22050;
    int n_mfcc = 224;
    int dct_type = 2;
    float power = 2.f;
    bool center = false;
    bool norm = true;
    const char* window = "hann";
    const char* pad_mode = "reflect";
       
    std::vector<float> data;
    int res = read_audio(inputPath, data, &sr, false);
    if (res < 0)
    {
        printf("read wav file error: %s\n", inputPath);
        return -1;
    }

    //printf("n_fft      = %d\n", n_fft);
    //printf("n_mel      = %d\n", n_mel);
    //printf("hop_length = %d\n", hop_length);
    //printf("fmin, fmax = (%d,%d)\n", fmin, fmax);
    //printf("sr         = %d, data size=%ld\n", sr, data.size());

    vector<vector<float>> melSpectrogram = librosa::Feature::melspectrogram(data, sr, n_fft, hop_length, window,
        center, pad_mode, power, n_mel, fmin, fmax);

    int mels_w = (int)melSpectrogram.size();
    int mels_h = (int)melSpectrogram[0].size();

    std::vector<float> log_mel_spec = power_to_db(get_vector(melSpectrogram));

    double min_val, max_val;
    cv::minMaxLoc(log_mel_spec, &min_val, &max_val);

    //printf("min_val:%lf", min_val);
    //printf("max_val:%lf", max_val);

    if (max_val == min_val)
    {
        return -1;
    }

    for (size_t i = 0; i < log_mel_spec.size(); i++)
    {
        log_mel_spec[i] = (255.0 * (log_mel_spec[i] - min_val) / (max_val - min_val));
    }

    cv::Mat melMat = vector2mat<float>(log_mel_spec, 1, mels_h);
    melMat.convertTo(melMat, CV_8UC1);

    cv::Mat melColor;
    cv::applyColorMap(melMat, melColor, cv::COLORMAP_MAGMA);

    cv::flip(melColor, melColor, 0);
    cv::resize(melColor, melColor, cv::Size(224, 224));
    cv::imwrite("./output/1.jpg", melColor);

    end = clock();
    cpu_time_used = ((double)(end - start)) / (CLOCKS_PER_SEC / 1000);
    std::cout << "函数运行时间:" << cpu_time_used << " 毫秒" << std::endl;

    return 0;
}