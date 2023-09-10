#include <stdio.h>
#include <unistd.h>

int main() {
  int fd = dup(STDOUT_FILENO);

  freopen("output.txt", "w", stdout);
  printf("Hello\n");
  fclose(stdout);

  freopen("/dev/null", "w", stdout);
  dup2(fd, STDOUT_FILENO);
  close(fd);
  printf("World\n");
}
