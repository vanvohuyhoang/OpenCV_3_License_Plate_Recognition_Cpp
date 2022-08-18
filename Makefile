#RK_FOLDER = /home/hoang/Desktop/4-Secam2/rv1126_linux_210924
#CXX = $(RK_FOLDER)/prebuilts/gcc/linux-x86/arm/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++
#SYSROOT_RK += --sysroot=$(RK_FOLDER)/buildroot/output/rockchip_rv1126_rv1109/host/arm-buildroot-linux-gnueabihf/sysroot
	
#prefix=../lib_secam/lib_opencv
#CXXFLAGS += -I ${prefix}/include/opencv -I ${prefix}/include -L ${prefix}/lib -g 
CXXFLAGS_opencv += -I ./ -Wall -pedantic -ggdb -O2 ` pkg-config --cflags --libs opencv4`
CFLAGS += -DDEBUG -O2 -DLINUX -D_GNU_SOURCE -Wall
LFLAGS += -lpthread -ldl 
LFOPENCV += -lopencv_core -lopencv_imgproc -lopencv_ml -lopencv_imgcodecs
obj_files = DetectChars.o DetectPlates.o PossiblePlate.o Preprocess.o Recognise.o Main.o
.PHONY: $(obj_files) 
all: $(obj_files)   host
clean:
	rm -f *.o core *~ *.so *.lo
DetectChars.o:
	g++ $(CXXFLAGS_opencv) -c  DetectChars.cpp -o DetectChars.o $(LFOPENCV)
Recognise.o:
	g++ $(CXXFLAGS_opencv) -c  Recognise.cpp -o Recognise.o $(LFOPENCV)
DetectPlates.o:
	g++ $(CXXFLAGS_opencv) -c  DetectPlates.cpp -o DetectPlates.o $(LFOPENCV)
PossiblePlate.o:
	g++ $(CXXFLAGS_opencv) -c  PossiblePlate.cpp -o PossiblePlate.o $(LFOPENCV)
Preprocess.o:
	g++ $(CXXFLAGS_opencv) -c  Preprocess.cpp -o Preprocess.o $(LFOPENCV)
Main.o:
	g++ $(CXXFLAGS_opencv) -c  Main.cpp -o Main.o $(LFOPENCV)
host: 
	g++ $(obj_files) $(CXXFLAGS_opencv) $(LFOPENCV) -o test_detect