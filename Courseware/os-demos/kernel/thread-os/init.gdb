set pagination off
target remote localhost:1234
file build/kernel-x86_64-qemu.elf
break on_interrupt
tui disable
continue
