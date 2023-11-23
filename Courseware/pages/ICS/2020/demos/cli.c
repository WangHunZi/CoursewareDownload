#define _GNU_SOURCE

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <stdint.h>

void on_sigsegv(int sig, siginfo_t *info, void *ucontext) {
  ucontext_t *ctx = (ucontext_t *)ucontext;
  uint8_t *pc = (void *)ctx->uc_mcontext.gregs[REG_RIP];
  printf("PC = %p  ", pc);
  for (int i = -8; i < 8; i++) {
    printf(i == 0 ? "[%02x] " : "%02x ", pc[i]);
  }
  printf("\n");
  exit(1);
}

int main() {
  struct sigaction act;
  act.sa_sigaction = on_sigsegv;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGSEGV, &act, NULL);
  asm volatile("cli");
}
