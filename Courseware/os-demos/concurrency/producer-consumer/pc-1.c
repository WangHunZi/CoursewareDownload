#include <thread.h>
#include <thread-sync.h>

mutex_t lk = MUTEX_INIT();

int n, depth = 0;

void T_produce() {
    while (1) {
retry:
        mutex_lock(&lk);
        int ready = (depth < n);
        mutex_unlock(&lk);
        if (!ready) goto retry;

        // assert(depth < n);

        mutex_lock(&lk);
        printf("(");
        depth++;
        mutex_unlock(&lk);
    }
}

void T_consume() {
    while (1) {
retry:
        mutex_lock(&lk);
        int ready = (depth > 0);
        mutex_unlock(&lk);
        if (!ready) goto retry;

        // assert(depth > 0);

        mutex_lock(&lk);
        printf(")");
        depth--;
        mutex_unlock(&lk);
    }
}
