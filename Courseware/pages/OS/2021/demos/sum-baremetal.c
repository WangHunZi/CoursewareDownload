#include <am.h>
#include <klib.h>

#define N 1000000

typedef struct {
  int locked;
} lock_t;

#define LOCK_INIT()     ((lock_t) { .locked = 0 })
void lock(lock_t *lk)   { while (atomic_xchg(&lk->locked, 1)); }
void unlock(lock_t *lk) { atomic_xchg(&lk->locked, 0); }

static lock_t lk = LOCK_INIT();
static int sum = 0;

static void mp_entry() {
  for (int i = 0; i < N; i++) {
    lock(&lk);
    sum++;
    if (sum % 10000 == 0) {
      printf("sum = %d\n", sum);
    }
    unlock(&lk);
  }
  while (1);
}

int main() {
  mpe_init(mp_entry);
}
