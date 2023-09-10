#include "ulib.h"

void hello_test();
void dfs_test();

int main() {
  printf("Hello from user\n");
  hello_test();
  dfs_test();
  while (1);
}

void hello_test() {
  int pid = fork(), x = 0;

  const char *fmt;
  if (pid) {
    fmt = "Parent #%d\n";
  } else {
    sleep(1);
    fmt = "Child #%d\n";
  }

  while (1) {
    printf(fmt, ++x);
    sleep(2);
  }
}

#define DEST  '+'
#define EMPTY '.'

static struct move {
  int x, y, ch;
} moves[] = {
  { 0, 1, '>' },
  { 1, 0, 'v' },
  { 0, -1, '<' },
  { -1, 0, '^' },
};

static char map[][16] = {
  "#####",
  "#..+#",
  "##..#",
  "#####",
  "",
};

void display();

void dfs(int x, int y) {
  if (map[x][y] == DEST) {
    display();
  } else {
    sleep(1);
    int nfork = 0;

    for (struct move *m = moves; m < moves + 4; m++) {
      int x1 = x + m->x, y1 = y + m->y;
      if (map[x1][y1] == DEST || map[x1][y1] == EMPTY) {
        int pid = fork();
        if (pid == 0) { // map[][] copied
          map[x][y] = m->ch;
          dfs(x1, y1);
        } else {
          nfork++;
        }
      }
    }
  }
  while (1) sleep(1);
}

void dfs_test() {
  dfs(1, 1);
  while (1) sleep(1);
}

void display() {
  for (int i = 0; ; i++) {
    for (const char *s = map[i]; *s; s++) {
      switch (*s) {
        case EMPTY: printf("   "); break;
        case DEST : printf(" ○ "); break;
        case '>'  : printf(" → "); break;
        case '<'  : printf(" ← "); break;
        case '^'  : printf(" ↑ "); break;
        case 'v'  : printf(" ↓ "); break;
        default   : printf("▇▇▇"); break;
      }
    }
    printf("\n");
    if (strlen(map[i]) == 0) break;
  }
}
