TRACED = 'bwrite balloc ialloc iappend rinode winode rsect wsect'.split()
IGNORE = 'ip xp buf'.split()

class trace(gdb.Breakpoint):
    def stop(self):
        f, bt = gdb.selected_frame(), []
        while f and f.is_valid():
            if (name := f.name()) in TRACED:
                lvars = [f'{sym.name}={sym.value(f)}'
                    for sym in f.block()
                    if sym.is_argument and sym.name not in IGNORE]
                bt.append(f'\033[32m{name}\033[0m({", ".join(lvars)})')
            f = f.older()
        print('    ' * (len(bt) - 1) + bt[0])
        return False # won't stop at this breakpoint

gdb.execute('set prompt off')
gdb.execute('set pagination off')
for fn in TRACED:
    trace(fn)
gdb.execute('run fs.img README user/_ls')
gdb.execute('quit')
