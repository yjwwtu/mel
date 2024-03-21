# MelSpectrumPic

#### 介绍
梅尔频谱图实现librosa的功能,提交准备好OpenCV的库,Linux系统下,Windows没测试,应该是可以用的

## 1.项目结构

```bash
.
├── data
├── librosa
│   ├── eigen3
│   ├── audio_utils.cpp
│   ├── audio_utils.h
│   ├── cv_utils.h
│   └── librosa.h
├── python
│   ├── test.py
│   └── test(1).py
├── Makefile
├── main.cpp
├── main.py
├── build.sh
├── README.md
└── requirements.txt
```

## 2.依赖项

- Eigen3: 项目已经支持[Eigen](librosa/eigen3)，无须安装
- OpenCV：项目用opencv出图,以便显示梅尔频谱图

## 3.Demo

```bash
    make
```

# 4.参考资料

- [librosa](https://github.com/librosa/librosa)
- [eigen](https://gitlab.com/libeigen/eigen)
- [vo-amrwbenc](https://github.com/mstorsjo/vo-amrwbenc/blob/master/wavreader.h)
- [LibrosaCpp](https://github.com/ewan-xu/LibrosaCpp)

