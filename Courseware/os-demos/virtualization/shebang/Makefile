CFLAGS := -Wall -g

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=)

# Default target
all: $(OBJS)

%: %.c
	gcc $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS)

.PHONY: all clean
