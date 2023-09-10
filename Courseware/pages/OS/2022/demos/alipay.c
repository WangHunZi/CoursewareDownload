#include "thread.h"

unsigned long balance = 100;

void Alipay_withdraw(int amt) {
  if (balance >= amt) {
    usleep(1); // unexpected delays
    balance -= amt;
  }
}

void Talipay(int id) {
  Alipay_withdraw(100);
}

int main() {
  create(Talipay);
  create(Talipay);
  join();
  printf("balance = %lu\n", balance);
}
