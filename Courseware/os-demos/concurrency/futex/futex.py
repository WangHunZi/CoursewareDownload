LOCKED   = 1
UNLOCKED = 0

def T_worker(name):
    while True:
        # Atomic exchange
        t = heap.status
        heap.status = LOCKED
        sys_sched()

        if t == LOCKED:
            # futex_wait()
            heap.wait += name
            while name in heap.wait:
                sys_sched()
        else:
            break
        sys_sched()

    # Critical section
    heap.cs += name
    sys_write(name)
    sys_sched()

    heap.cs = heap.cs.replace(name, '')
    sys_sched()

    # futex_wake()
    if heap.wait:
        c = sys_choose(heap.wait)
        heap.wait.replace(c, '')

    sys_sched()

    heap.status = UNLOCKED
    sys_sched()

def main():
    heap.status = UNLOCKED
    heap.wait = ''
    heap.cs = ''

    sys_spawn(T_worker, 'A')
    sys_spawn(T_worker, 'B')
    sys_spawn(T_worker, 'C')
