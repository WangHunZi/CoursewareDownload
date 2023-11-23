#include <stdio.h>
extern long foo(long x);

int main() {
  for (int x = -10; x <= 10; x++) {
    printf("f(%d) = %d\n", x, foo(x));
  }
}
