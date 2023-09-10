#include <threads.h>

__thread char *base, *now; // thread-local variables
__thread int id;

// objdump to see how thread local variables are implemented
void set_base(char *ptr) { base = ptr; }
void set_now(char *ptr)  { now = ptr; }
void *get_base()         { return &base; }
void *get_now()          { return &now; }

void stackoverflow(int n) {
  char x;
  if (n == 0) set_base(&x);
  set_now(&x);
  if (n % 1024 == 0) {
    printf("[T%d] Stack size @ n = %d: %p +%ld KiB\n",
      id, n, base, (base - now) / 1024);
  }
  stackoverflow(n + 1);
}

void probe(int tid) {
  id = tid;
  printf("[%d] thread local address %p\n", id, &base);
  stackoverflow(0);
}

int main() {
  setbuf(stdout, NULL);
  for (int i = 0; i < 4; i++) {
    create(probe);
  }
  join(NULL);
}
