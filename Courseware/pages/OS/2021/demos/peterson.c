#include <threads.h>
#include <stdatomic.h>

#define T1 1
#define T2 2

#define FENCE __sync_synchronize()

atomic_int  nested = ATOMIC_VAR_INIT(0);
atomic_long count  = ATOMIC_VAR_INIT(0);

void critical_section() {
  long cnt = atomic_fetch_add(&count, 1);
  int i = atomic_fetch_add(&nested, 1) + 1;
  if (i != 1) {
    printf("%d threads in the critical section @ count=%ld\n", i, cnt);
    assert(0);
  }
  atomic_fetch_add(&nested, -1);
}

// Peterson's algorithm; with memory fences
// !! x, y, and turn must be volatile !!
int volatile x = 0, y = 0, turn;

void thread_1() {
  while (1) {
    x = 1;                   FENCE;
    turn = T2;               FENCE;
    while (1) {
      if (!y) break;         FENCE;
      if (turn != T2) break; FENCE;
    }
    critical_section();
    x = 0;                   FENCE;
  }
}

void thread_2() {
  while (1) {
    y = 1;                   FENCE;
    turn = T1;               FENCE;
    while (1) {
      if (!x) break;         FENCE;
      if (turn != T1) break; FENCE;
    }
    critical_section();
    y = 0;                   FENCE;
  }
}

int main() {
  create(thread_1);
  create(thread_2);
}
