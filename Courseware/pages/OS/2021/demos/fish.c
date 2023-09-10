#include "threads.h"

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

enum state {
  A = 1, B, C, D, E, F,
};

struct rule {
  enum state from;
  char ch;
  enum state to;
};

struct rule rules[] = {
  { A, '<', B },
  { B, '>', C },
  { C, '<', D },
  { A, '>', E },
  { E, '<', F },
  { F, '>', D },
  { D, '_', A },
};
enum state current = A;
int quota = 1;

pthread_mutex_t lk   = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;

int next(char ch) {
  for (int i = 0; i < LENGTH(rules); i++) {
    struct rule *rule = &rules[i];
    if (rule->from == current && rule->ch == ch) {
      return rule->to;
    }
  }
  return 0;
}

void fish_before(char ch) {
  pthread_mutex_lock(&lk);
  while (next(ch) == 0 || quota == 0) {
    pthread_cond_wait(&cond, &lk);
  }
  quota--;
  pthread_mutex_unlock(&lk);
}

void fish_after(char ch) {
  pthread_mutex_lock(&lk);
  quota++;
  current = next(ch);
  assert(current);
  pthread_cond_broadcast(&cond);
  pthread_mutex_unlock(&lk);
}

void fish_init() { }

static const char roles[] = "<<<<<>>>>___";

void fish_thread(int id) {
  char role = roles[id];
  while (1) {
    fish_before(role);
    putchar(role); // should not hold *any* mutex lock now
    fish_after(role);
  }
}

int main() {
  setbuf(stdout, NULL);
  fish_init();
  for (int i = 0; i < strlen(roles); i++)
    create(fish_thread);
  join(NULL);
}
