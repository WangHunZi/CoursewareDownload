#include <thread.h>

// We create 3 threads in this example.
#define T        3
#define N  1000000

#define LOCKED   1
#define UNLOCKED 0

int status = UNLOCKED;

void lock() {
    int expected;
    do {
        // Try compare status with expected.
        // If the comparison succeeded, perform
        // an exchange.
        expected = UNLOCKED;
        asm volatile (
            "lock cmpxchgl %2, %1"
            : "+a" (expected) // Value for comparison.
                              // x86 uses eax/rax.
            : "m" (status),   // Memory location.
              "r" (LOCKED)    // Value to be written if
                              // status == expected
            : "memory", "cc"
        );
    } while (expected != UNLOCKED);
}

void unlock() {
    // To be safer:
    //
    // asm volatile (
    //     "movl %1, %0"
    //     : "=m" (status)
    //     : "r" (UNLOCKED)
    //     : "memory"
    // );

    // But actually we can do this:
    asm volatile("" ::: "memory");
    status = UNLOCKED;
}


long volatile sum = 0;

void T_sum(int tid) {
    for (int i = 0; i < N; i++) {
        lock();

        // This critical section is even longer; but
        // it should be safe--the world is stopped.
        // We also marked sum as volatile to make
        // sure it is loaded and stored in each
        // loop iteration.
        for (int _ = 0; _ < 10; _++) {
            sum++;
        }

        unlock();
    }

    printf("Thread %d: sum = %ld\n", tid, sum);
}

int main() {
    for (int i = 0; i < T; i++) {
        create(T_sum);
    }

    join();

    printf("sum  = %ld\n", sum);
    printf("%d*n = %ld\n", T * 10, T * 10L * N);
}
