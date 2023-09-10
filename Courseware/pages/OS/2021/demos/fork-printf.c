#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  int n = 0;
  if (argv[1]) {
    n = atoi(argv[1]);
  }

  for (int i = 0; i < n; i++) {
    fork();
    printf("Hello\n");
  }

  for (int i = 0; i < n; i++)
    wait(NULL);
}
