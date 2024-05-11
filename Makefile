CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -pedantic -O3

EXE := $(patsubst %.cxx, %, $(wildcard *.cxx))
SRCS := $(wildcard *.cc)
OBJS := $(patsubst %.cc,%.o,$(SRCS))

all: $(EXE)

%: %.cxx
	$(CXX) -o $@ $^ $(CXXFLAGS) $(OBJS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf $(TARGET) *.o

.PHONY: all clean
