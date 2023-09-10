#include <threads.h>

typedef struct spinlock {
  intptr_t locked;
} spinlock_t;

void spin_init(spinlock_t *lk) {
  lk->locked = 0;
}

void spin_lock(spinlock_t *lk) {
  while (atomic_xchg(&lk->locked, 1)) ;
}

void spin_unlock(spinlock_t *lk) {
  atomic_xchg(&lk->locked, 0);
}

spinlock_t lk;

long volatile sum = 0;

void do_sum() {
  for (int i = 0; i < 10000000; i++) {
    spin_lock(&lk);
    sum++;
    spin_unlock(&lk);
  }
}

void print() {
  printf("sum = %ld\n", sum);
}

int main() {
  spin_init(&lk);
  for (int i = 0; i < 4; i++) {
    create(do_sum);
  }
  join(print);
}
