CXXFLAGS := -Wall -g -std=c++20

SRCS := $(wildcard *.cc)
OBJS := $(SRCS:.cc=)

# Default target
all: $(OBJS)

%: %.cc
	g++ $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJS)

.PHONY: all clean
