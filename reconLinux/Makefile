
CXXFLAGS += -I/usr/include/opencv -I. -g
LDFLAGS += -L/usr/lib -lcv -lhighgui

.PHONY: all clean
all: reconLinux

reconLinux: reconstructionController.o ../common/cvCalibrateProCam.o ../common/cvScanProCam.o ../common/cvUtilProCam.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o ../common/*.o
