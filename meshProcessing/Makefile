
CXXFLAGS += $(shell pkg-config --cflags opencv) -I. -g
LDFLAGS += $(shell pkg-config --libs opencv)

PROGS=bin/flatfield bin/texture bin/PoissonRecon

.PHONY: all clean
all: $(PROGS)

bin/flatfield: src/flatfield.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

bin/texture: src/texture.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

bin/PoissonRecon:
	$(MAKE) -C PoissonRecon release
	ln -s ../PoissonRecon/Bin/Linux/PoissonRecon $@

clean:
	$(MAKE) -C PoissonRecon clean
	rm -f src/*.o $(PROGS)
