# We statically compile the binary
# for simpler system call traces.
CFLAGS := -Wall -g -static

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=)

all: $(OBJS)

%: %.c
	gcc $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS)

.PHONY: all clean
