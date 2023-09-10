#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define KB * (1L << 10)
#define MB * (1L << 20)
#define GB * (1L << 30)

void alloc(long bytes, int each) {
  for (int i = 0; i < bytes / each; i++) {
    void *ptr = malloc(each);
    assert(ptr);
    memset(ptr, 0, each);
  }
  printf("Allocated %ld bytes of size %d\n", bytes, each);
}

int main() {
  alloc(10 MB, 32);
  alloc(1 GB, 1 GB);
}
