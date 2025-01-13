def T1():
    while True:
        heap.x = '🏴'
        sys_sched()
        heap.turn = '❷'
        sys_sched()
        while True:
            t = heap.turn
            sys_sched()
            y = heap.y != ''
            sys_sched()
            if not y or t == '❶':
                break
        sys_sched()
        heap.cs += '❶'
        sys_sched()
        heap.cs = heap.cs.replace('❶', '')
        sys_sched()
        heap.x = ''
        sys_sched()
 
def T2():
    while True:
        heap.y = '🏁'
        sys_sched()
        heap.turn = '❶'
        sys_sched()
        while True:
            t = heap.turn
            sys_sched()
            x = heap.x
            sys_sched()
            if not x or t == '❷':
                break
            sys_sched()
        sys_sched()
        heap.cs += '❷'
        sys_sched()
        heap.cs = heap.cs.replace('❷', '')
        sys_sched()
        heap.y = ''
        sys_sched()

def main():
    heap.x = ''
    heap.y = ''
    heap.turn = ''
    heap.cs = ''
    sys_spawn(T1)
    sys_spawn(T2)
