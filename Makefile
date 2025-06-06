CXX := g++
CXXFLAGS := -Wall -Wextra -pedantic -O3

EXE := $(patsubst %.cxx, %, $(wildcard *.cxx))
TESTS := $(patsubst %.cxx, %, $(wildcard Test/test*.cxx))
SRCS := $(wildcard *.cc)
OBJS := $(patsubst %.cc,%.o,$(SRCS))

all: $(EXE)

%: %.cxx $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -fPIC -c $<

test: $(TESTS)
	./Test/testUF23Field
	./Test/testCovariance
	./Test/testRandomDraw

pybind: ./UF23Field.o
	$(MAKE) -C Python

clean:
	rm -rf $(EXE) *.o
	$(MAKE) -C Python clean

.PRECIOUS: %.o
.PHONY: all clean
