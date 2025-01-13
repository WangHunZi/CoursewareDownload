#include <thread-sync.h>

sem_t done[16];
int count = 0;

void worker_init(int T) {
    count = T;
    for (int i = 1; i <= T; i++) {
        SEM_INIT(&done[i], 0);
    }
}

void worker_done(int id) {
    V(&done[id]);
}

void worker_join() {
    for (int i = 1; i <= count; i++) {
        P(&done[i]);
    }
}
