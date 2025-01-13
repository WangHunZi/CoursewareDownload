def main():
    x = sys_fork()
    sys_sched()
    y = sys_fork()
    sys_sched()
    sys_write(f'{x} {y}; ')
