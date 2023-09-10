#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void try_open(const char *fname) {
  int fd = open(fname, O_RDWR);
  printf("open(\"%s\") returns %d\n", fname, fd);
  if (fd < 0) {
    perror("  FAIL");
  } else {
    printf("  SUCCESS!\n");
    close(fd);
  }
}

int main() {
  try_open("/something/not/exist");
  try_open("/dev/sda"); // hard drive
}
