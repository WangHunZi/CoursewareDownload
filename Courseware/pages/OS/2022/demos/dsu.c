#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>

void foo()     { printf("In old function %s\n", __func__); }
void foo_new() { printf("In new function %s\n", __func__); }

// 48 b8 ff ff ff ff ff ff ff ff    movabs $0xffffffffffffffff,%rax
// ff e0                            jmpq   *%rax
void DSU(void *old, void *new) {
  #define ROUNDDOWN(ptr) ((void *)(((uintptr_t)ptr) & ~0xfff))
  size_t    pg_size = sysconf(_SC_PAGESIZE);
  char *pg_boundary = ROUNDDOWN(old);
  int         flags = PROT_WRITE | PROT_READ | PROT_EXEC;

  printf("Dynamically updating... "); fflush(stdout);

  mprotect(pg_boundary, 2 * pg_size, flags);
  memcpy(old +  0, "\x48\xb8", 2);
  memcpy(old +  2,       &new, 8);
  memcpy(old + 10, "\xff\xe0", 2);
  mprotect(pg_boundary, 2 * pg_size, flags & ~PROT_WRITE);

  printf("Done"); fflush(stdout);
}

int main() {
  foo();
  DSU(foo, foo_new);
  foo();
}
