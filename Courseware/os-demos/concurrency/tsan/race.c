#include <thread.h>
#include <thread-sync.h>

#define N 1000000

long sum = 0;

mutex_t lk = MUTEX_INIT();

void T_sum() {
    for (int i = 0; i < N; i++) {
        mutex_lock(&lk);
        sum++;
        mutex_unlock(&lk);

        if (i % (N / 10) == 0) {
            sum++;
        }
    }
}

int main() {
    create(T_sum);
    create(T_sum);

    join();

    printf("sum = %ld\n", sum);
    printf("2*n = %ld\n", 2L * N);
}
