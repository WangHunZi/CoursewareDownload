#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void hanoi_r(int n, char from, char to, char via) {
  /* PC */
  /*  0 */ if (n == 1) printf("Move %c -> %c\n", from, to);
  /*  . */ else {
  /*  1 */   hanoi_r(n - 1, from, via, to);
  /*  2 */   hanoi_r(1,     from, to,  via);
  /*  3 */   hanoi_r(n - 1, via,  to,  from);
  /*  4 */ }
}

struct frame {
  int pc, n;
  char from, to, via;
};

#define call(...) ({ stk[top++] = (struct frame) { 0, __VA_ARGS__ }; })
#define ret()     ({ top--; })

void hanoi(int n, char from, char to, char via) {
  // inital state: empty stack
  struct frame stk[64];
  int top = 0;

  // push an initial frame
  call(n, from, to, via);

  while (top > 0) {
    // just like a full-system emulator!
    struct frame *f = &stk[top - 1];
    switch (f->pc++) {
      case  0:
        if (f->n == 1) {
          printf("Move %c -> %c\n", f->from, f->to);
          f->pc = 4; // or: ret();
        }
        break;
      case  1: call(f->n - 1, f->from, f->via, f->to);   break;
      case  2: call(       1, f->from, f->to,  f->via);  break;
      case  3: call(f->n - 1, f->via,  f->to,  f->from); break;
      case  4: ret();                                    break;
      default: assert(0);
    }
  }
}

int main(int argc, char *argv[]) {
  int n = argc < 2 ? 1 : atoi(argv[1]);
  hanoi(n, 'A', 'C', 'B');
}
