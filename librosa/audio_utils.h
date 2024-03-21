/* ------------------------------------------------------------------
* Copyright (C) 2009 Martin Storsjo
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
* express or implied.
* See the License for the specific language governing permissions
* and limitations under the License.
* -------------------------------------------------------------------
*/

#ifndef WAVREADER_H_
#define WAVREADER_H_

#include <iostream>
#include <vector>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif
/***
 *
 * @param filename
 * @return
 */
void *wav_read_open(const char *filename);

/***
 *
 * @param obj
 */
void wav_read_close(void *obj);

/**
 * 读取音频文件,目前仅支持wav格式文件
 * @param filename wav格式文件
 * @param out 输出音频数据
 * @param sr 输出音频采样率
 * @param mono 设置为true是单通道，否则是双通道
 * @return
 */
int read_audio(const char *filename, vector<float> &out, int *sr, bool mono = true);

/***
 *
 * @param obj
 * @param format
 * @param channels
 * @param sample_rate
 * @param bits_per_sample
 * @param data_length
 * @return
 */
int wav_get_header(void *obj, int *format, int *channels, int *sample_rate, int *bits_per_sample,
                   unsigned int *data_length);

/***
 *
 * @param obj
 * @param data
 * @param length
 * @return
 */
int wav_read_data(void *obj, unsigned char *data, unsigned int length);

#ifdef __cplusplus
}
#endif

#endif