#include "thread.h"

#define N 4

char * volatile low[N];
char * volatile high[N];

void update_range(int T, char *ptr) {
    // We have a witness of thread T accessing stack pointer
    // ptr. So we keep this record.
    if (ptr < low[T]) {
        low[T] = ptr;
    }
    if (ptr > high[T]) {
        high[T] = ptr;
    }
}

void probe(int T, int n) {
    // Local variables are stack-allocated.
    char scratch[64];
    update_range(T, scratch);

    printf("Stack(T%d) >= %ld KB\n",
        T, (high[T] - low[T]) / 1024);

    probe(T, n + 1); // Infinite recursion.
}

void T_probe(int T) {
    T -= 1;
    low[T] = (char *)-1;  // 0xffffffffffffffff
    high[T] = (char *)0;  // 0x0000000000000000
    probe(T, 0);
}

int main() {
    // Disable printf buffering. Try to make more lines being
    // printed, because the program will eventually crash (and
    // all buffered contents are lost).
    setbuf(stdout, NULL);

    for (int i = 0; i < N; i++) {
        create(T_probe);
    }
}
