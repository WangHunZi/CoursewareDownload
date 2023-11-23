#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

int main() {
  for (int i = 0; i < 3; i++) {
    fork(); // create some processes
  }

  volatile int *ptr = mmap(
    (void *)0x20000000,          // mapping address (hint)
    1 << 20,                     // mapping size
    PROT_WRITE | PROT_READ,      // read/write mapping
    MAP_PRIVATE | MAP_ANONYMOUS, // mapping flags
    -1,                          // file descriptor
    0                            // offset
  );

  *ptr = getpid();
  printf("#%d sets %p = %d\n", getpid(), ptr, getpid());

  sleep(1);
  printf("#%d *%p = %d\n", getpid(), ptr, *ptr);
}
