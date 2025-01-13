#include <am.h>
#include <klib.h>
#include <stdint.h>
#include <spinlock.h>

struct cpu cpus[16];

#define INTR    assert(ienabled())
#define NO_INTR assert(!ienabled())

spinlock_t bkl = spin_init("Big Kernel Lock");
spinlock_t A = spin_init("Lock A");
spinlock_t B = spin_init("Lock B");

// This is NOT recommended in C:
// "return" inside ATOMIC leads to deadlocks.
#define ATOMIC(lk) \
    for (int _ = (spin_lock(lk), 0); _ < 1; \
        spin_unlock(lk), _++)

uint64_t volatile x;

void increment(int n) {
    for (int i = 0; i < n; i++) {
        x++;
    }
}

Context *on_interrupt(Event ev, Context *ctx) {
    NO_INTR;

    ATOMIC(&bkl) {
        // Lock body; check atomicity.
        increment(100);
        assert(x % 10 == 0);

        // Print the current cpu.
        printf("%d", cpu_current());

        NO_INTR;
    }

    // Interrupt shoud still be off here.
    NO_INTR;

    ATOMIC(&A) {
        ATOMIC(&B) {
            increment(10);
            NO_INTR;
        }
    }

    NO_INTR;
    return ctx;
}

void T_worker() {
    int cpuid = cpu_current();

    spin_lock(&bkl);
    printf("CPU %d is on.\n", cpuid);
    spin_unlock(&bkl);

    iset(true);

    while (true) {
        // Enable interrupt on main loop.
        INTR;

        ATOMIC(&bkl) {
            ATOMIC(&A) {
                ATOMIC(&B) {
                    // Critical section

                    NO_INTR;
                    assert(x % 10 == 0);
                    increment(10);
                }
                NO_INTR;
            }
            // Still no interrupt at this point.
            NO_INTR;
        }

        INTR;
    }
}

int main() {
    cte_init(on_interrupt);
    mpe_init(T_worker);
}
