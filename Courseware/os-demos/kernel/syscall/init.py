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
                    if not line.startswith('xmm')
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

gdb.execute('target remote localhost:1234')
gdb.execute('hb *0x401000')
gdb.execute('continue')
