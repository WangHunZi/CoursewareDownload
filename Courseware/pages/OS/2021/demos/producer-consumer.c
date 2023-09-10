#include <threads.h>

#define N 6

sem_t fill, empty;

void producer() {
  while (1) {
    P(&empty);
    printf("(");
    V(&fill);
  }
}

void consumer() {
  while (1) {
    P(&fill);
    printf(")");
    V(&empty);
  }
}

int main() {
  SEM_INIT(fill, 0);
  SEM_INIT(empty, N);
  for (int i = 0; i < 4; i++) create(producer);
  for (int i = 0; i < 4; i++) create(consumer);
  join(NULL);
}
