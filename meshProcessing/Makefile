
CXXFLAGS += $(shell pkg-config --cflags opencv) -I. -g
LDFLAGS += $(shell pkg-config --libs opencv)

PROGS=bin/flatfield bin/texture bin/PoissonRecon bin/applyMask bin/removeHead

.PHONY: all clean
all: bin $(PROGS)

bin:
	mkdir -p bin

bin/flatfield: src/flatfield.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

bin/texture: src/texture.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

bin/applyMask: src/applyMask.o ../imageProcessing/src/pageFinder.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

bin/removeHead: src/removeHead.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

bin/PoissonRecon: PoissonRecon/Bin/Linux
	$(MAKE) -C PoissonRecon release
	ln -s ../PoissonRecon/Bin/Linux/PoissonRecon $@

PoissonRecon/Bin/Linux:
	mkdir -p PoissonRecon/Bin/Linux

clean:
	$(MAKE) -C PoissonRecon clean
	rm -f src/*.o $(PROGS)
