#include <thread.h>
#include <thread-sync.h>
#include <unistd.h>

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

const int N = 7;

struct Edge {
    int from, to;
    mutex_t mutex;
} edges[] = {
    {1, 2, MUTEX_INIT()},
    {2, 3, MUTEX_INIT()},
    {2, 4, MUTEX_INIT()},
    {2, 5, MUTEX_INIT()},
    {4, 6, MUTEX_INIT()},
    {5, 6, MUTEX_INIT()},
    {4, 7, MUTEX_INIT()},
};

void T_worker(int id) {
    for (int i = 0; i < LENGTH(edges); i++) {
        struct Edge *e = &edges[i];
        if (e->to == id) {
            mutex_lock(&e->mutex);
        }
    }

    printf("Start %d\n", id);
    sleep(1);
    printf("End %d\n", id);
    sleep(1);

    for (int i = 0; i < LENGTH(edges); i++) {
        struct Edge *e = &edges[i];
        if (e->from == id) {
            // Well... This is undefined behavior
            // for POSIX threads. This is just a
            // hack for demonstration.
            mutex_unlock(&e->mutex);
        }
    }
}

int main() {
    for (int i = 0; i < LENGTH(edges); i++) {
        struct Edge *e = &edges[i];
        mutex_lock(&e->mutex);
    }

    for (int i = 0; i < N; i++) {
        create(T_worker);
    }
}
