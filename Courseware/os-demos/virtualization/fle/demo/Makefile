CC := ./cc
LD := ./ld

CFLAGS := -Wall -g -Os

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

# Build and link hello (fle)
hello: $(OBJS) tools
	$(LD) $(SRCS:.c=.fle) -o $@

%.o: %.c tools
	$(CC) $(CFLAGS) $< -o $@

# fle.py is all the binutils!
tools:
	@ln -f -s ../fle.py cc
	@ln -f -s ../fle.py ld
	@ln -f -s ../fle.py exec
	@ln -f -s ../fle.py objdump
	@ln -f -s ../fle.py readfle
	@ln -f -s ../fle.py nm

clean:
	rm -f $(OBJS) $(SRCS:.c=.fle) cc ld exec exec objdump readfle nm hello

.PHONY: all clean tools
