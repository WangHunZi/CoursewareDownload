#include <stdio.h>

__attribute__((constructor)) void hello() {
  printf("Hello, World\n");
}

// See also: atexit(3)
__attribute__((destructor)) void goodbye() {
  printf("Goodbye, Cruel OS World!\n");
}

int main() {
}
