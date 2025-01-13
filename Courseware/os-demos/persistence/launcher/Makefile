# Set the name of the kernel module
obj-m := driver.o

# Set the path to the kernel source directory
KERNEL_SRC := /lib/modules/$(shell uname -r)/build

# Set the current working directory
PWD := $(shell pwd)

# Default target to build the kernel module
all:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules
	gcc -static $(CFLAGS) -o launch user.c

# Target to clean the build artifacts
clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean

.PHONY: all clean
