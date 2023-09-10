// CFLAGS:  -shared -ldl -fPIC
// run:     LD_PRELOAD=$PWD/mtrace.so command [args...]

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))
static long buf[64];

static void print_stats() {
  int max = LENGTH(buf) - 1;
  while (max >= 0 && buf[max] == 0) {
    max--;
  }

  printf("Statistics of malloc():\n");
  for (int i = 1; i <= max; i++) {
    printf("  2^%-2d (%5ld): %ld\n", i, 1L << i, buf[i]);
  }
}

static inline int LOG2(size_t size) {
  int result = 0;
  while (size >>= 1) result++;
  return result;
}

void *malloc(size_t size) {
  static void *(*real_malloc)(size_t) = NULL;
  if (!real_malloc) {
    atexit(print_stats);
    real_malloc = dlsym(RTLD_NEXT, "malloc"); 
  }
  buf[LOG2(size)]++;
  return real_malloc(size);
}
