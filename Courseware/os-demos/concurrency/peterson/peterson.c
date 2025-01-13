#include <thread.h>
#include <stdatomic.h>

#define A 1
#define B 2

// The caveat is: no matter how many times we run this test
// without seeing it fail, we cannot be certain that we have
// inserted sufficient barriers. Understanding the correctness
// of this code is far beyond the scope of this course.
// 
#define BARRIER __sync_synchronize()
//
// Peterson's algorithm is wrong without proper barriers:
//
// #define BARRIER

atomic_int inside;
long count;

void critical_section() {
    // We expect this thread executing code exclusively,
    // if the critical section is correctly implemented.
 
    assert(
        // assert(inside == 0);
        // inside++
        atomic_fetch_add(&inside, +1) == 0
    );

    // On some machines, printing a character will hide
    // the bug!
    // putchar('.');

    assert(
        // assert(inside == 1);
        // inside--
        atomic_fetch_add(&inside, -1) == 1
    );
}

int volatile a = 0, b = 0, turn;

void T_A() {
    while (1) {
        a = 1;                    BARRIER;
        turn = B;                 BARRIER; // <- this is critcal for x86
        while (1) {
            if (!b) break;        BARRIER;
            if (turn != B) break; BARRIER;
        }

        // T_B can't execute critical_section now.
        critical_section();

        a = 0;                    BARRIER;
    }
}

void T_B() {
    while (1) {
        b = 1;                    BARRIER;
        turn = A;                 BARRIER;
        while (1) {
            if (!a) break;        BARRIER;
            if (turn != A) break; BARRIER;
        }

        // T_A can't execute critical_section now.
        critical_section();

        b = 0;                    BARRIER;
    }
}

int main() {
    create(T_A);
    create(T_B);
}
