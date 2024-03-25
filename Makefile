LOCAL_SRC_DIR = $(shell pwd)
LINK    = @echo linking $@ && g++
GCC     = @echo compiling $@ && g++ 
GC      = @echo compiling $@ && gcc
AR      = @echo generating static library $@ && ar -crv
FLAGS   = -g -W -w -Wall -fPIC -std=c++11 -Wno-unused-parameter


GCCFLAGS = 
DEFINES = 
HEADER  = -I./ -I/usr/include/opencv4/
LIBS    = -L./ 

LINKFLAGS =  -DDEBUG

CURRENT_TIME   = $(shell date "+%Y%m%d%H%M%S")

LIBS    += -pthread -lm -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lz
LIBS    += -lavformat -lavutil -lavcodec -lswresample -lswscale

SOURCE_FILES :=\
		main.cpp\
		app/opencv/cvutils.cpp\
		app/audio/audioutils.cpp\
		app/librosa/audio_utils.cpp\


PROJECTNAME = MelSpectrumPic

TARGET = main

all: $(SOURCE_FILES)
	$(LINK) $(FLAGS) $(LINKFLAGS) -o ${PROJECTNAME}.out $^ $(LIBS) ${HEADER}
clean:
	rm -rf *.o *.out bin obj *.db *.txt *.bin ./output/*.png 
del:
	rm -rf *
