#include <threads.h>

long sum;

void do_sum() {
  for (int i = 0; i < 10000000; i++) {
    asm volatile("addq $1, %0": "=m"(sum));
  }
}

void print() {
  printf("sum = %ld\n", sum);
}

int main() {
  for (int i = 0; i < 4; i++) 
    create(do_sum);
  join(print);
}
