#include "audioutils.h"


/******************************************************************************
*  Name        :   Files::exists
*  Author      :   cqnews
*  Version     :   V1.0.0
*  Data        :   2021.05.07
*  Describe    :   检测文件是否存在
******************************************************************************/
bool AudioUtils::Exists(const char* name)
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


/// <summary>
/// 读取音频文件
/// </summary>
/// <param name="filename"></param>
/// <param name="out"></param>
/// <param name="sr"></param>
/// <returns></returns>
int AudioUtils::ReadAudioData(const char* pWavFilename, std::vector<float>& out, int& sr)
{
    int ret = -1;

    if (pWavFilename == NULL)
    {
        return -1;
    }

    if (Exists(pWavFilename) == false)
    {
        return -1;
    }

    //初始化解封装
    av_register_all();
    // 初始化网络
    avformat_network_init();
    avcodec_register_all();

    // 初始化FFmpeg
    AVFormatContext* pAvFmtCtx = nullptr; //音视频封装
    AVCodecContext* pCodecCtx = nullptr; //解码器

    AVPacket* packet = nullptr;
    AVFrame* outAvFrame = nullptr;

    int audioStream = -1;
    int count = 0;

    do
    {

        pAvFmtCtx = avformat_alloc_context();
        if (pAvFmtCtx == NULL)
        {
            printf("Error opening formatCtx\n");
            ret = -1;
            break;
        }

        if (avformat_open_input(&pAvFmtCtx, pWavFilename, nullptr, nullptr) != 0)
        {
            printf("Error opening input file\n");
            ret = -1;
            break;
        }

        if (avformat_find_stream_info(pAvFmtCtx, nullptr) < 0)
        {
            printf("Could not find stream information");
            ret = -1;
            break;
        }

        audioStream = FindAudioStream(pAvFmtCtx);

        if (audioStream <= -1)
        {
            printf("format_context->streams[audioStream] is null");
            ret = -1;
            break;
        } //如果未找到音频,直接结束

        AVStream* as = pAvFmtCtx->streams[audioStream];

        if (as == NULL)
        {
            printf("format_context->streams[audioStream] is null ", audioStream);
            return -1;
        }

        AVCodec* codec = avcodec_find_decoder(as->codecpar->codec_id);
        if (codec == NULL)
        {
            printf("Could not find AVCodec");
            ret = -1;
            break;
        }

        pCodecCtx = avcodec_alloc_context3(codec);
        if (pCodecCtx == NULL)
        {
            printf("Could not find AVCodecContext");
            ret = -1;
            break;
        }

        avcodec_parameters_to_context(pCodecCtx, pAvFmtCtx->streams[0]->codecpar);
        ret = avcodec_open2(pCodecCtx, codec, nullptr);
        if (0 != ret)
        {
            printf("Could not open avcodec_open2");
            ret = -1;
            break;
        }
        int numBytes = av_get_bytes_per_sample(pCodecCtx->sample_fmt);
        sr = pCodecCtx->sample_rate;

#ifdef DEBUG
        std::cout << "pCodecCtx->name: " << codec->name << std::endl;
        std::cout << "pCodecCtx->sample_rate: " << pCodecCtx->sample_rate << std::endl;
        std::cout << "pCodecCtx->sample_fmt: " << pCodecCtx->sample_fmt << std::endl;
        std::cout << "pCodecCtx->channels: " << pCodecCtx->channels << std::endl;
        std::cout << "pCodecCtx->channel_layout: " << pCodecCtx->channel_layout << std::endl;

#endif // DEBUG


        if (packet == nullptr)
        {
            packet = (AVPacket*)malloc(sizeof(AVPacket));
        }

        if (packet == nullptr)
        {
            ret = -1;
            break;
        }
        av_init_packet(packet);

        while (av_read_frame(pAvFmtCtx, packet) >= 0)
        {
            if (packet->stream_index != audioStream) //读取到非音频帧不处理
            {
                continue;
            }

            if (!outAvFrame) //检查解析输出帧是否存在
            {
                outAvFrame = av_frame_alloc();
            }

            if (avcodec_send_packet(pCodecCtx, packet) == 0)
            {
                while (avcodec_receive_frame(pCodecCtx, outAvFrame) == 0)
                {
                    if (outAvFrame == NULL) { continue; }

                    int nbSample = outAvFrame->nb_samples;
                    int channels = outAvFrame->channels;

                    if (channels <= 0) { channels = 1; }

                    int isPlanar = av_sample_fmt_is_planar(pCodecCtx->sample_fmt);

                    for (int i = 0; i < nbSample; i++)
                    {
                        float sampleValues = 0;

                        for (int ch = 0; ch < channels; ch++)
                        {
                            int16_t sampleValue = 0;

                            switch (numBytes)
                            {
                                case 1: //8位
                                {
                                    if (isPlanar)
                                    {
                                        sampleValue += ((int8_t*)outAvFrame->data[ch])[i];
                                    }
                                    else
                                    {
                                        sampleValue = ((int8_t*)outAvFrame->data[0])[i * channels + ch];
                                    }
                                    break;
                                }
                                case 4: //32位
                                {
                                    if (isPlanar)
                                    {
                                        sampleValue += ((int32_t*)outAvFrame->data[ch])[i];
                                    }
                                    else
                                    {
                                        sampleValue = ((int32_t*)outAvFrame->data[0])[i * channels + ch];
                                    }
                                    break;
                                }
                                default: //默认16位
                                {
                                    if (isPlanar)
                                    {
                                        sampleValue += ((int16_t*)outAvFrame->data[ch])[i];
                                    }
                                    else
                                    {
                                        sampleValue = ((int16_t*)outAvFrame->data[0])[i * channels + ch];
                                    }
                                    break;
                                }
                                break;
                            }

                            sampleValues += static_cast<float>(sampleValue) / 32768.f;




                        }
                        sampleValues /= channels * 1.0; //如果多通道,取平均值
#ifdef DEBUG
                        if (count <= 1000)
                        {
                            printf("%d: %lf\n", count, sampleValues);
                            count++;
                        }
#endif // DEBUG

                        //std::cout << sampleValues << std::endl;
                        out.push_back(sampleValues);
                        }
                    }
                }
            }

        } while (0);


        if (outAvFrame) { av_frame_free(&outAvFrame); }
        if (packet) { av_free_packet(packet); av_free(packet); }

        if (pAvFmtCtx) { avformat_close_input(&pAvFmtCtx); }
        if (pCodecCtx) { avcodec_free_context(&pCodecCtx); }

        return 0;
    }


/// <summary>
/// 查找音频线程
/// </summary>
/// <returns></returns>
int AudioUtils::FindAudioStream(const AVFormatContext* avFmtCxt)
{
    if (avFmtCxt == NULL) { printf("this->avFmtCxt is null"); return -1; }

    if (avFmtCxt->nb_streams <= 0) { printf("this->avFmtCxt->nb_streams <= 0"); return -1; }

    for (unsigned int i = 0; i < avFmtCxt->nb_streams; i++)
    {
        AVStream* as = avFmtCxt->streams[i];
        if (as == NULL)
        {
            printf("format_context->streams[ch] is null");
            continue;
        }

        if (as->codec == NULL)
        {
            printf("as->codec is null");
            continue;
        }

        if (as->codec->codec_type != AVMEDIA_TYPE_AUDIO)
        {
            printf("other media\n");
            continue;
        }

        printf("sample_rate = %d, channels = %d, sample_format = %d\n",
            as->codec->sample_rate,
            as->codec->channels,
            as->codec->get_format
        );

        return i;

    }

}
