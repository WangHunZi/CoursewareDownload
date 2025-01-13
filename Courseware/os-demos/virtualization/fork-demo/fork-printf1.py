def main():
    heap.buf = ''
    for _ in range(2):
        sys_fork()
        heap.buf += '⭐️'  # Actual behavior of printf()
    sys_write(heap.buf)
