CFLAGS := -Wall -g

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=)

# Default target
all: $(OBJS)

%: %.c
	yes | head -c 4000 > example.txt
	gcc $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) example.txt

.PHONY: all clean
