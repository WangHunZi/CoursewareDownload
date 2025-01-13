NAME := kernel
SRCS := $(shell find . -name "*.c")
export ARCH := x86_64-qemu
include $(AM_HOME)/Makefile

debug:
	qemu-system-x86_64 -s -S build/kernel-x86_64-qemu
