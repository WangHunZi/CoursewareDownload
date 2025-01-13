#include <thread-sync.h>

int count;
sem_t done;

void worker_init(int T) {
    count = T;
    SEM_INIT(&done, 0);
}

void worker_done(int id) {
    V(&done);
}

void worker_join() {
    for (int i = 0; i < count; i++) {
        P(&done);
    }
}
