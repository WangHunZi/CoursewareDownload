#include <thread.h>
#include <stdatomic.h>

int x = 0, y = 0;
atomic_int flag;

#define F1  1
#define F2  2

#define FLAG \
    atomic_load(&flag)
#define FLAG_XOR(val) \
    atomic_fetch_xor(&flag, val)
#define WAIT_FOR(cond) \
    while (!(cond))    \
        ;

__attribute__((noinline))
void write_x_read_y() {
    int y_val;

    asm volatile(
        "movl $1, %0;" // x = 1
        "movl %2, %1;" // y_val = y
        : "=m"(x), "=r"(y_val)
        : "m"(y)
    );

    printf("%d ", y_val);
}

__attribute__((noinline))
void write_y_read_x() {
    int x_val;

    asm volatile(
        "movl $1, %0;" // y = 1
        "movl %2, %1;" // x_val = x
        : "=m"(y), "=r"(x_val)
        : "m"(x)
    );

    printf("%d ", x_val);
}

void T_1(int id) {
    while (1) {
        // Wait until F1 is raised.
        WAIT_FOR((FLAG & F1));

        write_x_read_y();

        // Put F1 down.
        FLAG_XOR(F1);
    }
}

void T_2() {
    while (1) {
        // Wait until F2 is raised.
        WAIT_FOR((FLAG & F2));

        write_y_read_x();

        // Put F2 down.
        FLAG_XOR(F2);
    }
}

void T_flag() {
    while (1) {
        x = 0;
        y = 0;
        __sync_synchronize(); // full barrier
        usleep(1);            // + delay

        // Now, x = 0, y = 0, and flag = 0.
        // T_1 and T_2 should be waiting for their flags.
        assert(FLAG == 0);

        // flag = F1 | F2; Both flags are raised.
        FLAG_XOR(F1 | F2);

        // T1 and T2 are ready to go...
        // They will eventually put F1 and F2 down.
        WAIT_FOR(FLAG == 0);

        printf("\n");
        fflush(stdout);
    }
}

int main() {
    // All infinite loops
    create(T_1);
    create(T_2);
    create(T_flag);
}
