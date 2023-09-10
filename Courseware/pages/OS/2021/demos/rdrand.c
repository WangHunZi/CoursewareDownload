#include <stdio.h>
#include <stdint.h>

int main() {
  uint64_t volatile val = 114514;
  asm volatile ("rdrand %0": "=r"(val));
  printf("rdrand returns %016lx\n", val);
}
