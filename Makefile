CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -pedantic -O3

EXE := $(patsubst %.cxx, %, $(wildcard *.cxx))
SRCS := $(wildcard *.cc)
OBJS := $(patsubst %.cc,%.o,$(SRCS))

all: $(EXE)

%: %.cxx $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $<

test: testUF23Field testCovariance
	./testUF23Field
	./testCovariance

clean:
	rm -rf $(EXE) *.o

.PRECIOUS: %.o
.PHONY: all clean
