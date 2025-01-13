CC=gcc
CFLAGS=-Wall -fPIC -Os
LDFLAGS=-shared

# Executable and libraries
EXE=main
LIBA=liba.so
LIBB=libb.so

# Source files
LIBA_SRCS=liba-1.c liba-2.c
LIBB_SRCS=libb-1.c libb-2.c
MAIN_SRCS=main.c

# Object files
LIBA_OBJS=$(LIBA_SRCS:.c=.o)
LIBB_OBJS=$(LIBB_SRCS:.c=.o)
MAIN_OBJS=$(MAIN_SRCS:.c=.o)

# Default target
all: $(EXE)

# Link the main executable
$(EXE): $(MAIN_OBJS) $(LIBA) $(LIBB)
	$(CC) -o $@ $(MAIN_OBJS) -L. -la -lb

# Build liba.so from liba-1.o and liba-2.o
$(LIBA): $(LIBA_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Build libb.so from libb.o
$(LIBB): $(LIBB_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ -L. -la

# Generic rule for building object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run main
run:
	LD_LIBRARY_PATH=. ./$(EXE)

# Clean up
clean:
	rm -f $(EXE) $(LIBA) $(LIBB) *.o

# Phony targets
.PHONY: all run clean
