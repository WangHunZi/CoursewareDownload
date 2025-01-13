#include <pthread.h>

extern long sum, N;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void T_sum() {
    for (int i = 0; i < N; i++) {
        pthread_mutex_lock(&lock);
        sum++;
        pthread_mutex_unlock(&lock);
    }
}
