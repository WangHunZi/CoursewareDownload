def T_sum():
    for _ in range(3):
        t = heap.sum
        sys_sched()
        t = t + 1
        heap.sum = t
        sys_sched()
    sys_write(f'sum = {heap.sum}\n')

def main():
    heap.sum = 0
    sys_spawn(T_sum)
    sys_spawn(T_sum)
    sys_spawn(T_sum)
