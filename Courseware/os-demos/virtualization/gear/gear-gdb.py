import gdb
import datetime

ratio = float(gdb.parse_and_eval('$gear_ratio'))

# Get the current time
start = datetime.datetime.now()

def hacked_time():
    now = datetime.datetime.now()
    
    # The speed of the clock is adjusted
    t = start + (now - start) * ratio

    tv_sec = int(t.timestamp())
    tv_usec = t.microsecond
    return (tv_sec, tv_usec)

class SetTimevalBreakpoint(gdb.Breakpoint):
    def __init__(self):
        super(SetTimevalBreakpoint, self).__init__(
            'gettimeofday',
            gdb.BP_BREAKPOINT,
            internal=False
        )

    def stop(self):
        tv_sec, tv_usec = hacked_time()

        # Replace the function body
        gdb.execute(
            'set *(struct timeval *)($rdi) = {{ {}, {} }}'
                .format(tv_sec, tv_usec)
        )
        gdb.execute('set $rax = 0')
        gdb.execute('return')

        return False  # Continue execution

SetTimevalBreakpoint()
gdb.execute('run')
