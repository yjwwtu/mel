#ifndef ___AUDIO_UTILS_H____
#define ___AUDIO_UTILS_H____
#include <vector>
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>

#if defined(__cplusplus)
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>

#include <libavutil/opt.h>
#include <libavutil/avutil.h>
#include <libavutil/ffversion.h>
#include <libavutil/macros.h>
#include <libavutil/bprint.h>
#if defined(__cplusplus)
}
#endif


class AudioUtils
{
public:
    static bool Exists(const char* name);
    static int  ReadAudioData(const char* filename, std::vector<float>& out, int& sr); //¶ÁÈ¡ÒôÆµÎÄ¼þ

    static int  FindAudioStream(const AVFormatContext* avFmtCxt);

};


#endif // ___AUDIO_UTILS_H____


