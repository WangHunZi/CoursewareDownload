#include <stddef.h>

void bar() {
  *(int *)NULL = 1;
}

void foo() {
  bar();
}

int main() {
  foo();
}
