NAME := kernel
SRCS := main.c spinlock.c
export smp  := 4
export ARCH := x86_64-qemu

-include $(AM_HOME)/Makefile

debug:
	qemu-system-x86_64 -s -S -machine accel=tcg -smp "2,sockets=1" \
		-drive format=raw,file=build/kernel-x86_64-qemu &
	gdb -x debug.py
