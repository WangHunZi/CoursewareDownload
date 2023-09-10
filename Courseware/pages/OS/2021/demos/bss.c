#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>

int main() {
  extern char end;
  char buf[8192];

  int fd = open("/proc/self/maps", O_RDONLY);
  assert(fd >= 0);

  int nread = read(fd, buf, sizeof(buf) - 1);
  assert(nread >= 0);
  buf[nread] = '\n';
  
  printf("%s", buf);
  printf("End of bss at %lx\n", (uintptr_t)&end);
}
