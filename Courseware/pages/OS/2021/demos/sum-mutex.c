#include <threads.h>

typedef pthread_mutex_t mutex_t;
#define MUTEX_INIT() PTHREAD_MUTEX_INITIALIZER
void mutex_lock(mutex_t *lk)   { pthread_mutex_lock(lk); }
void mutex_unlock(mutex_t *lk) { pthread_mutex_unlock(lk); }

static mutex_t mutex = MUTEX_INIT();
static long volatile sum = 0;

void do_sum() {
  for (int i = 0; i < 1000; i++) {
    mutex_lock(&mutex);
    for (int volatile j = 0; j < 10000; j++) {
      sum++;
    }
    mutex_unlock(&mutex);
  }
}

void print() {
  printf("sum = %ld\n", sum);
}

int main() {
  for (int i = 0; i < 10; i++)
    create(do_sum);
  join(print);
}
