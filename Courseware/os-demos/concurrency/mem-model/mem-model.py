def T1():
    heap.x = 1
    sys_sched()
    y_ = heap.y
    sys_sched()
    sys_write(f'{y_}')

def T2():
    heap.y = 1
    sys_sched()
    x_ = heap.x
    sys_sched()
    sys_write(f'{x_}')

def main():
    heap.x, heap.y = 0, 0
    sys_spawn(T1)
    sys_spawn(T2)
