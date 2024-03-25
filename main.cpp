#include <iostream>
#include <vector>
#include <algorithm>
#include <iostream> 
#include <cmath> 
#include <limits>
#include <time.h>

#include "app/librosa/librosa.h"
#include "app/opencv/cvutils.h"
#include "app/audio/audioutils.h"



int PrintMel(const char*);

int main(int argc, char* argv[])
{
    std::string inputPath = "./data/20_16000Hz_5S.wav";

    if (argc >= 2)
    {
        inputPath = argv[1];
    }

    PrintMel(inputPath.c_str());

}

int PrintMel(const char* inputPath)
{
    int ret = -1;

    if (AudioUtils::Exists(inputPath) == false)
    {
        printf("%s not found.", inputPath);
        return -1;
    }

    clock_t start_t, mel_t, db_t, end_t;
    double cpu_time_used;
    start_t = clock();
    
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
    ret = AudioUtils::ReadAudioData(inputPath, data, sr);

    if (ret < 0)
    {
        return ret;
    }

#ifdef DEBUG
    printf("n_fft      = %d\n", n_fft);
    printf("n_mel      = %d\n", n_mel);
    printf("hop_length = %d\n", hop_length);
    printf("fmin, fmax = (%d,%d)\n", fmin, fmax);
    printf("sr         = %d, data size=%ld\n", sr, data.size());
#endif // DEBUG

    std::vector<std::vector<float>> melSpectrogram = librosa::Feature::melspectrogram(data, sr, n_fft, hop_length, window,
        center, pad_mode, power, n_mel, fmin, fmax);

    int mels_w = (int)melSpectrogram.size();
    int mels_h = (int)melSpectrogram[0].size();

    mel_t = clock();
    cpu_time_used = ((double)(mel_t - start_t)) / (CLOCKS_PER_SEC / 1000);
    std::cout << "Mel函数运行时间:" << cpu_time_used << " 毫秒" << std::endl;

    std::vector<float> log_mel_spec = librosa::Feature::power2db(CvUtils::Conver2DToVector(melSpectrogram));

    db_t = clock();
    cpu_time_used = ((double)(db_t - start_t)) / (CLOCKS_PER_SEC / 1000);
    std::cout << "dB函数运行时间:" << cpu_time_used << " 毫秒" << std::endl;

    double min_val, max_val;
    cv::minMaxLoc(log_mel_spec, &min_val, &max_val);

    if (max_val == min_val)
    {
        return -1;
    }

    for (size_t i = 0; i < log_mel_spec.size(); i++)
    {
        log_mel_spec[i] = (255.0 * (log_mel_spec[i] - min_val) / (max_val - min_val));
    }

    cv::Mat melMat = CvUtils::Vector2Mat(log_mel_spec, 1, mels_h);
    melMat.convertTo(melMat, CV_8UC1);

    cv::Mat melColor;
    cv::applyColorMap(melMat, melColor, cv::COLORMAP_MAGMA);

    cv::flip(melColor, melColor, 0);
    cv::resize(melColor, melColor, cv::Size(224, 224));
    cv::imwrite("./output/1.jpg", melColor);

    end_t = clock();
    cpu_time_used = ((double)(end_t - start_t)) / (CLOCKS_PER_SEC / 1000);
    std::cout << "函数运行时间:" << cpu_time_used << " 毫秒" << std::endl;
    std::cout << "=============================================" << std::endl;

    return 0;
}