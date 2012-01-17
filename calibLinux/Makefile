
CXXFLAGS += $(shell pkg-config --cflags opencv) -I.
LDFLAGS += $(shell pkg-config --libs opencv)

all: calibLinux matDiff

debug: CXXFLAGS += -g -Wall
debug: calibLinux

calibLinux: calibLinux.o cvCalibrateProCam.o ../common/cvScanProCam.o ../common/cvUtilProCam.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

matDiff: matDiff.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o ../common/*.o calibLinux matDiff
