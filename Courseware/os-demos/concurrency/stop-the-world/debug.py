import os

def on_quit():
    gdb.execute('kill')

gdb.events.exited.connect(on_quit)
gdb.execute('target remote localhost:1234')
gdb.execute('file build/cli-x86_64-qemu.elf')
gdb.Breakpoint('main')
gdb.execute('continue')
