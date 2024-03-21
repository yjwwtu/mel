import librosa
import cv2
import numpy as np
import matplotlib.pyplot as plt


class AudioProcessor:
        def __init__(self):
                self.FMIN = 20 # 最小频率
                self.FMAX = 16000 # 最大频率
                self.SPEC_SHAPE = (224, 224) # mel频谱图的形状
                self.HOP_LENGTH = 0 #int(self.SAMPLE_RATE * 0.01) # 帧移
                self.TOP_DB = 10 # 去除背景噪音的阈值
                self.SEGMENT_LENGTH = 5 # 分割长度（秒）


        # 定义一个函数，输入一个5秒的音频片段，输出一个mel频谱图的对象
        def generate_melspectrogram(self):

                y, sr = librosa.load(
                        './20_16000Hz_5S.wav',
                        duration = self.SEGMENT_LENGTH,
                        sr=None
                        )
                print(y.shape)

                
                # 计算mel频谱
                self.HOP_LENGTH = int(self.SEGMENT_LENGTH * sr / (self.SPEC_SHAPE[1] - 1))

                mel_spec = librosa.feature.melspectrogram(
                        y=y,
                        sr=sr,
                        n_fft=2048,
                        hop_length=self.HOP_LENGTH,
                        n_mels=self.SPEC_SHAPE[0],
                        fmin=self.FMIN,
                        fmax=self.FMAX
                        )
                #print(mel_spec.shape)

                #audio to db
                log_mel_spec = librosa.power_to_db(mel_spec, ref=np.max)
                print(log_mel_spec.min())
                print(log_mel_spec.max())


                exit(0)


                # 将梅尔频谱图转为0-255范围内的灰度图像
                gray_image = ().astype(np.uint8)

                # 将灰度图像转换为彩色映射后的图像
                colormap_image = cv2.applyColorMap(gray_image, cv2.COLORMAP_MAGMA)

                # 创建一个窗口并显示图像
                cv2.namedWindow('Mel Spectrogram with Color Map', cv2.WINDOW_NORMAL)
                cv2.imshow('Mel Spectrogram with Color Map', colormap_image)

                # 等待用户按键后关闭所有窗口
                cv2.waitKey(0)
                cv2.destroyAllWindows()

                #print(mel_spec_db)

                #Noremalize
                # mel_spec -= mel_spec.min()
                # mel_spec /= mel_spec.max()
                # mel_spec *= 255
                # #print(mel_spec)
                #
                # fig,ax = plt.subplots()
                # ax.axis("on")
                #
                # img = librosa.display.specshow(
                #         mel_spec_db,
                #         x_axis='time',
                #         y_axis='mel',
                #         sr=sr,
                #         fmin=self.FMIN,
                #         fmax=self.FMAX,
                #         ax=None
                #         )
                #
                # plt.show()

def main():
        print("Hello, World!")

        t1 = AudioProcessor()
        t1.generate_melspectrogram()


if __name__ == '__main__':
        main()
