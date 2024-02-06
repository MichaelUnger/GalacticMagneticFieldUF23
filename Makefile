CC := g++
CFLAGS := -std=c++11 -Wall -Wextra -pedantic -O3
TARGET := uf23Field

SRCS := $(wildcard *.cc)
OBJS := $(patsubst %.cc,%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

%.o: %.cc
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(TARGET) *.o

.PHONY: all clean
