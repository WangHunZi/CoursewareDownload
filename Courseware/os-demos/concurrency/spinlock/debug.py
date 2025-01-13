import os

def on_quit():
    gdb.execute('kill')

gdb.events.exited.connect(on_quit)
gdb.execute('target remote localhost:1234')
gdb.execute('file build/kernel-x86_64-qemu.elf')
gdb.Breakpoint('on_interrupt')
gdb.execute('continue')
gdb.execute('set scheduler-locking on')
