#include <thread.h>
#include <thread-sync.h>

int n, depth = 0;
mutex_t lk = MUTEX_INIT();
cond_t cv = COND_INIT();
 
#define CAN_PRODUCE (depth < n)
#define CAN_CONSUME (depth > 0)

void T_produce() {
    while (1) {
        mutex_lock(&lk);

        if (!CAN_PRODUCE) {
            cond_wait(&cv, &lk);
            // This is subtle. Seemingly "more efficient"
            // implementation is dangerous for newbies.
        }

        printf("(");
        depth++;

        cond_signal(&cv);
        mutex_unlock(&lk);
    }
}

void T_consume() {
    while (1) {
        mutex_lock(&lk);

        if (!CAN_CONSUME) {
            cond_wait(&cv, &lk);
        }

        printf(")");
        depth--;

        cond_signal(&cv);
        mutex_unlock(&lk);
    }
}
