#include <threads.h>

int x = 0;

void f() {
  printf("Hello from thread #%c\n", "123456789X"[x++]);
  while (1); // to make sure we're not calling f() for ten times
}

int main() {
  for (int i = 0; i < 10; i++) {
    create(f);
  }
  join(NULL);
}
