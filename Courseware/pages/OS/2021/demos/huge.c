#include <stdio.h>
#include <unistd.h>

int main() {
  asm volatile(
    ".fill 1024 * 1024 * 128, 1, 0x90"
  );
  printf("pid = %d\n", getpid());
  while (1) sleep(1);
}
