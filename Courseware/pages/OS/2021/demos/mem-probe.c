#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/wait.h>
#include <readline/readline.h>

void ignore(int num) {
  printf("Fail.\n");
  exit(0);
}

int main() {
  char buf[4096], cmd[128];
  setbuf(stdout, NULL);
  signal(SIGSEGV, ignore);

  sprintf(buf, "pmap %d", getpid());
  assert(0 == system(buf));

  while (1) {
    uintptr_t addr;

    char *line = readline("r/w> ");
    if (!line) break;

    sscanf(line, "%s%lx", cmd, &addr);
    free(line);

    if (cmd[0] == 'q') break;

    pid_t pid = fork();
    if (pid == 0) {
      volatile char *p = (void *)addr;
      if (cmd[0] == 'r') {
        uint8_t data[16];
        memcpy(data, (void *)p, sizeof(data));
        for (int i = 0; i < sizeof(data); i++) {
          printf("%02x ", data[i]);
        }
        printf("\n");
      } else if (cmd[0] == 'w') {
        char ch = *p;
        *p = ch;
        printf("Success.\n");
      }
      exit(0);
    } else {
      wait(NULL);
    }

  }
}
