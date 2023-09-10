#include "thread.h"
#include "thread-sync.h"

long f(int x);

int main() {
  int x;
  long sum = 0;
  while (!feof(stdin) && scanf("%d", &x) == 1) {
    sum += f(x); // 功能正确，但会 TLE
  }
  printf("%ld\n", sum);
}

/* 
T1.1 T1.2 T1.3
T1.4 T1.5 T1.6
T1.7 T1.8 T1.9
T2.1 T2.2 T2.3
T2.4 T2.5 T2.6
T2.7 T2.8 T2.9
对答案的解释 (100 字以内)
*/
