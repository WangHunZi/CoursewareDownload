CFLAGS := -g3 -static

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=)

# Default target
all: $(OBJS)

%: %.c
	musl-gcc $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS)

.PHONY: all clean
