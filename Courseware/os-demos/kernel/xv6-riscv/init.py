import gdb
import re

R = {}

def stop_handler(event):
    if isinstance(event, gdb.StopEvent):
        regs = [
            line for line in 
                gdb.execute('info registers',
                            to_string=True).
                            strip().split('\n')
        ]
        for line in regs:
            parts = line.split()
            key = parts[0]

            if m := re.search(r'(\[.*?\])', line):
                val = m.group(1)
            else:
                val = parts[1]

            if key in R and R[key] != val:
                print(key, R[key], '->', val)
            R[key] = val

gdb.events.stop.connect(stop_handler)

gdb.execute('set confirm off')
gdb.execute('set architecture riscv:rv64')
gdb.execute('target remote 127.0.0.1:26000')
gdb.execute('symbol-file xv6-riscv/kernel/kernel')
gdb.execute('set riscv use-compressed-breakpoints yes')

# Set a breakpoint on trampoline
# All user traps go here.
gdb.execute('hb *0x3ffffff000')

# User program entry
gdb.execute('hb *0x0')
