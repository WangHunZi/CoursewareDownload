#include <thread.h>

void worker_init(int T);
void worker_done(int id);
void worker_join();

void T_worker(int id) {
    printf("Thread %d start.\n", id);
    sleep(1);
    printf("Thread %d end.\n", id);

    worker_done(id);
}

int main() {
    worker_init(4);

    for (int i = 0; i < 4; i++) {
        create(T_worker);
    }

    worker_join();
    printf("Workers joined.\n");
}
