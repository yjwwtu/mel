/* ------------------------------------------------------------------
* Copyright (C) 2020 ewan xu<ewan_xu@outlook.com>
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

#ifndef LIBROSA_H_
#define LIBROSA_H_

#include "eigen3/Eigen/Core"
#include "eigen3/unsupported/Eigen/FFT"

#include <vector>
#include <complex>
#include <iostream>

///
/// \brief c++ implemention of librosa
///
namespace librosa {

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // !M_PI

    typedef Eigen::Matrix<float, 1, Eigen::Dynamic, Eigen::RowMajor> Vectorf;
    typedef Eigen::Matrix<std::complex<float>, 1, Eigen::Dynamic, Eigen::RowMajor> Vectorcf;
    typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Matrixf;
    typedef Eigen::Matrix<std::complex<float>, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Matrixcf;

    namespace internal{

        static Vectorf pad(Vectorf& x, int left, int right, const std::string& mode, float value)
        {
            Vectorf x_paded = Vectorf::Constant(left + x.size() + right, value);
            x_paded.segment(left, x.size()) = x;

            if (mode.compare("reflect") == 0)
            {
                for (int i = 0; i < left; ++i)
                {
                    x_paded[i] = x[static_cast<Eigen::Index>(left) - i];
                }
                for (int i = left; i < left + right; ++i)
                {
                    x_paded[i + x.size()] = x[x.size() - 2 - i + left];
                }
            }

            if (mode.compare("symmetric") == 0)
            {
                for (int i = 0; i < left; ++i)
                {
                    x_paded[i] = x[left - i - 1];
                }
                for (int i = left; i < left + right; ++i)
                {
                    x_paded[i + x.size()] = x[x.size() - 1 - i + left];
                }
            }

            if (mode.compare("edge") == 0)
            {
                for (int i = 0; i < left; ++i)
                {
                    x_paded[i] = x[0];
                }
                for (int i = left; i < left + right; ++i)
                {
                    x_paded[i + x.size()] = x[x.size() - 1];
                }
            }
            return x_paded;
        }

        /// <summary>
        /// 短时傅立叶变换
        /// </summary>
        /// <param name="x"></param>
        /// <param name="n_fft"></param>
        /// <param name="n_hop"></param>
        /// <param name="win">窗函数</param>
        /// <param name="center"></param>
        /// <param name="mode"></param>
        /// <returns></returns>
        static Matrixcf stft(Vectorf& x, int n_fft, int n_hop, const std::string& win, bool center, const std::string& mode)
        {
            // hanning(只实现了汉宁窗)
            Vectorf window = 0.5 * (1.f - (Vectorf::LinSpaced(n_fft, 0.f, static_cast<float>(n_fft - 1)) * 2.f * M_PI / n_fft).array().cos());

            int pad_len = center ? (n_fft / 2) : 0;
            Vectorf x_paded = pad(x, pad_len, pad_len, mode, 0.f);

            int n_f = n_fft / 2 + 1;
            int n_frames = 1 + (x_paded.size() - n_fft) / n_hop;
            Matrixcf X(n_frames, n_fft);
            Eigen::FFT<float> fft;

            for (int i = 0; i < n_frames; ++i)
            {
                Vectorf x_frame = window.array() * x_paded.segment(i * n_hop, n_fft).array();
                X.row(i) = fft.fwd(x_frame);
            }
            return X.leftCols(n_f);
        }

        /// <summary>
        /// 梅尔谱值
        /// </summary>
        /// <param name="X"></param>
        /// <param name="power"></param>
        /// <returns></returns>
        static Matrixf spectrogram(Matrixcf& X, float power = 1.f)
        {
            return X.cwiseAbs().array().pow(power);
        }

        /// <summary>
        /// 梅尔过滤
        /// </summary>
        /// <param name="sr"></param>
        /// <param name="n_fft"></param>
        /// <param name="n_mels"></param>
        /// <param name="fmin"></param>
        /// <param name="fmax"></param>
        /// <returns></returns>
        static Matrixf melfilter(int sr, int n_fft, int n_mels, int fmin, int fmax)
        {
            int n_f = n_fft / 2 + 1;
            Vectorf fft_freqs = (Vectorf::LinSpaced(n_f, 0.f, static_cast<float>(n_f - 1)) * sr) / n_fft;

            float f_min = 0.f;
            float f_sp = 200.f / 3.f;
            float min_log_hz = 1000.f;
            float min_log_mel = (min_log_hz - f_min) / f_sp;
            float logstep = logf(6.4f) / 27.f;

            auto hz_to_mel = [=](int hz, bool htk = false) -> float
            {
                if (htk)
                {
                    return 2595.0f * log10f(1.0f + hz / 700.0f);
                }
                float mel = (hz - f_min) / f_sp;
                if (hz >= min_log_hz)
                {
                    mel = min_log_mel + logf(hz / min_log_hz) / logstep;
                }
                return mel;
            };
            auto mel_to_hz = [=](Vectorf& mels, bool htk = false) -> Vectorf
            {
                if (htk)
                {
                    return 700.0f * (Vectorf::Constant(n_mels + 2, 10.f).array().pow(mels.array() / 2595.0f) - 1.0f);
                }
                return (mels.array() > min_log_mel).select(((mels.array() - min_log_mel) * logstep).exp() * min_log_hz, (mels * f_sp).array() + f_min);
            };

            float min_mel = hz_to_mel(fmin);
            float max_mel = hz_to_mel(fmax);
            Vectorf mels = Vectorf::LinSpaced(n_mels + 2, min_mel, max_mel);
            Vectorf mel_f = mel_to_hz(mels);
            Vectorf fdiff = mel_f.segment(1, mel_f.size() - 1) - mel_f.segment(0, mel_f.size() - 1);
            Matrixf ramps = mel_f.replicate(n_f, 1).transpose().array() - fft_freqs.replicate(n_mels + 2, 1).array();

            Matrixf lower = -ramps.topRows(n_mels).array() / fdiff.segment(0, n_mels).transpose().replicate(1, n_f).array();
            Matrixf upper = ramps.bottomRows(n_mels).array() / fdiff.segment(1, n_mels).transpose().replicate(1, n_f).array();
            Matrixf weights = (lower.array() < upper.array()).select(lower, upper).cwiseMax(0);

            auto enorm = (2.0 / (mel_f.segment(2, n_mels) - mel_f.segment(0, n_mels)).array()).transpose().replicate(1, n_f);
            weights = weights.array() * enorm;

            return weights;
        }

        /// <summary>
        /// 梅尔谱值计算
        /// </summary>
        /// <param name="x"></param>
        /// <param name="sr"></param>
        /// <param name="n_fft"></param>
        /// <param name="n_hop"></param>
        /// <param name="win"></param>
        /// <param name="center"></param>
        /// <param name="mode"></param>
        /// <param name="power"></param>
        /// <param name="n_mels"></param>
        /// <param name="fmin"></param>
        /// <param name="fmax"></param>
        /// <returns></returns>
        static Matrixf melspectrogram(Vectorf& x, int sr, int n_fft, int n_hop,
            const std::string& win, bool center,
            const std::string& mode, float power,
            int n_mels, int fmin, int fmax)
        {
            Matrixcf X = stft(x, n_fft, n_hop, win, center, mode);
            Matrixf mel_basis = melfilter(sr, n_fft, n_mels, fmin, fmax);
            Matrixf sp = spectrogram(X, power);
            Matrixf mel = mel_basis * sp.transpose();
            return mel;
        }

        /// <summary>
        /// 能量转分贝
        /// </summary>
        /// <param name="x"></param>
        /// <returns></returns>
        static Matrixf power2db(Matrixf& x)
        {
            auto log_sp = 10.0f * x.array().max(1e-10).log10();
            return log_sp.cwiseMax(log_sp.maxCoeff() - 80.0f);
        }

        static Matrixf dct(Matrixf& x, bool norm, int type)
        {
            int N = x.cols();
            Matrixf xi = Matrixf::Zero(N, N);
            xi.rowwise() += Vectorf::LinSpaced(N, 0.f, static_cast<float>(N - 1));
            // type 2
            Matrixf coeff = 2 * (M_PI * xi.transpose().array() / N * (xi.array() + 0.5)).cos();
            Matrixf dct = x * coeff.transpose();
            // ortho
            if (norm)
            {
                Vectorf ortho = Vectorf::Constant(N, sqrtf(0.5f / N));
                ortho[0] = sqrtf(0.25f / N);
                dct = dct * ortho.asDiagonal();
            }
            return dct;
        }

    } // namespace internal

    class Feature
    {
    public:

        /// <summary>
        /// short-time fourier transform similar with librosa.feature.stft
        /// </summary>
        /// <param name="x">input audio signal</param>
        /// <param name="n_fft">length of the FFT size</param>
        /// <param name="n_hop">number of samples between successive frames</param>
        /// <param name="win">window function. currently only supports 'hann'</param>
        /// <param name="center">same as librosa</param>
        /// <param name="mode">pad mode. support "reflect","symmetric","edge"</param>
        /// <returns>complex-valued matrix of short-time fourier transform coefficients.</returns>
        static std::vector<std::vector<std::complex<float>>> stft(std::vector<float>& x,
            int n_fft, int n_hop,
            const std::string& win, bool center,
            const std::string& mode)
        {
            Vectorf map_x = Eigen::Map<Vectorf>(x.data(), x.size());
            Matrixcf X = internal::stft(map_x, n_fft, n_hop, win, center, mode);
            std::vector<std::vector<std::complex<float>>> X_vector(X.rows(), std::vector<std::complex<float>>(X.cols(), 0));
            for (int i = 0; i < X.rows(); ++i)
            {
                auto& row = X_vector[i];
                Eigen::Map<Vectorcf>(row.data(), row.size()) = X.row(i);
            }
            return X_vector;
        }


        /// <summary>
        /// compute mel spectrogram similar with librosa.feature.melspectrogram
        /// </summary>
        /// <param name="x">input audio signal</param>
        /// <param name="sr">sample rate of 'x'</param>
        /// <param name="n_fft">length of the FFT size</param>
        /// <param name="n_hop">number of samples between successive frames</param>
        /// <param name="win">window function. currently only supports 'hann'</param>
        /// <param name="center">same as librosa</param>
        /// <param name="mode">pad mode. support "reflect","symmetric","edge"</param>
        /// <param name="power">exponent for the magnitude melspectrogram</param>
        /// <param name="n_mels">number of mel bands</param>
        /// <param name="fmin">lowest frequency (in Hz)</param>
        /// <param name="fmax">highest frequency (in Hz)</param>
        /// <returns></returns>
        static std::vector<std::vector<float>> melspectrogram(std::vector<float>& x, int sr,
            int n_fft, int n_hop, const std::string& win, bool center, const std::string& mode,
            float power, int n_mels, int fmin, int fmax)
        {
            Vectorf map_x = Eigen::Map<Vectorf>(x.data(), x.size());
            Matrixf mel = internal::melspectrogram(map_x, sr, n_fft, n_hop, win, center, mode, power, n_mels, fmin, fmax).transpose();
            std::vector<std::vector<float>> mel_vector(mel.rows(), std::vector<float>(mel.cols(), 0.f));
            for (int i = 0; i < mel.rows(); ++i)
            {
                auto& row = mel_vector[i];
                Eigen::Map<Vectorf>(row.data(), row.size()) = mel.row(i);
            }
            return mel_vector;
        }


        /// <summary>
        /// ompute mfcc similar with librosa.feature.mfcc
        /// </summary>
        /// <param name="x">input audio signal</param>
        /// <param name="sr">sample rate of 'x'</param>
        /// <param name="n_fft">length of the FFT size</param>
        /// <param name="n_hop">number of samples between successive frames</param>
        /// <param name="win">window function. currently only supports 'hann'</param>
        /// <param name="center">same as librosa</param>
        /// <param name="mode">pad mode. support "reflect","symmetric","edge"</param>
        /// <param name="power">exponent for the magnitude melspectrogram</param>
        /// <param name="n_mels">number of mel bands</param>
        /// <param name="fmin">lowest frequency (in Hz)</param>
        /// <param name="fmax">highest frequency (in Hz)</param>
        /// <param name="n_mfcc">number of mfccs</param>
        /// <param name="norm">ortho-normal dct basis</param>
        /// <param name="type">dct type. currently only supports 'type-II'</param>
        /// <returns>mfcc matrix</returns>
        static std::vector<std::vector<float>> mfcc(std::vector<float>& x, int sr,
            int n_fft, int n_hop, const std::string& win, bool center, const std::string& mode,
            float power, int n_mels, int fmin, int fmax,
            int n_mfcc, bool norm, int type)
        {
            Vectorf map_x = Eigen::Map<Vectorf>(x.data(), x.size());
            Matrixf mel = internal::melspectrogram(map_x, sr, n_fft, n_hop, win, center, mode, power, n_mels, fmin, fmax).transpose();
            Matrixf mel_db = internal::power2db(mel);
            Matrixf dct = internal::dct(mel_db, norm, type).leftCols(n_mfcc);
            std::vector<std::vector<float>> mfcc_vector(dct.rows(), std::vector<float>(dct.cols(), 0.f));
            for (int i = 0; i < dct.rows(); ++i)
            {
                auto& row = mfcc_vector[i];
                Eigen::Map<Vectorf>(row.data(), row.size()) = dct.row(i);
            }
            return mfcc_vector;
        }


        /// <summary>
        ///  假设 magnitude 是一个浮点数向量
        /// </summary>
        /// <param name="magnitude">输入参数</param>
        /// <param name="ref"></param>
        /// <param name="amin"></param>
        /// <param name="top_db">最高</param>
        /// <returns></returns>
        static std::vector<float> power2db(const std::vector<float>& magnitude, float ref = 1.0, float amin = 1e-10, float top_db = 80.0)
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
    };

} // namespace librosa

#endif
