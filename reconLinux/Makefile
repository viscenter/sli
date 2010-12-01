
CXXFLAGS += $(shell pkg-config --cflags opencv) -I. -g
LDFLAGS += $(shell pkg-config --libs opencv)

.PHONY: all clean
all: reconLinux

reconLinux: reconstructionController.o ../common/cvCalibrateProCam.o ../common/cvScanProCam.o ../common/cvUtilProCam.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o ../common/*.o
