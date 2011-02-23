
CXXFLAGS += $(shell pkg-config --cflags opencv) -I. -g
LDFLAGS += $(shell pkg-config --libs opencv)

PROGS=cropToPage findPageSize reTexture

.PHONY: all clean
all: $(PROGS)

cropToPage: src/cropToPage.o src/pageFinder.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

findPageSize: src/findPageSize.o src/pageFinder.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

reTexture: src/reTexture.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f src/*.o $(PROGS)
