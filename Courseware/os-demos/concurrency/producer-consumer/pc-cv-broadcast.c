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

        while (!CAN_PRODUCE) {
            cond_wait(&cv, &lk);
            // We are here if the thread is being waked up, with
            // the mutex being acquired. Then we check once again,
            // and move out of the loop if CAN_PRODUCE holds.
        }

        // We still hold the mutex--and we check again.
        assert(CAN_PRODUCE);

        printf("(");
        depth++;

        cond_broadcast(&cv);
        mutex_unlock(&lk);
    }
}

void T_consume() {
    while (1) {
        mutex_lock(&lk);

        while (!CAN_CONSUME) {
            cond_wait(&cv, &lk);
        }

        printf(")");
        depth--;

        cond_broadcast(&cv);
        mutex_unlock(&lk);
    }
}
