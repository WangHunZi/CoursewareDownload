def T_print(name):
    for _ in range(3):
        heap.n += 1
        sys_sched()
        sys_write(f'{name}{heap.n}')
        sys_sched()

def main():
    heap.n = 0
    sys_spawn(T_print, 'A')
    sys_spawn(T_print, 'B')
