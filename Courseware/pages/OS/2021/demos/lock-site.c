#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct lock {
  int locked;
  const char *site;
} lock_t;

#define STRINGIFY(s) #s
#define TOSTRING(s)  STRINGIFY(s)
#define LOCK_INIT() \
  ( (lock_t) { .locked = 0, .site = "lock-" __FILE__ ":" TOSTRING(__LINE__), } )

lock_t lk1 = LOCK_INIT();
lock_t lk2 = LOCK_INIT();

void lock(lock_t *lk) {
  printf("LOCK   %s\n", lk->site);
}

void unlock(lock_t *lk) {
  printf("UNLOCK %s\n", lk->site);
}

struct some_object {
  lock_t lock;
  int data;
};

void object_init(struct some_object *obj) {
  obj->lock = LOCK_INIT();
}

int main() {
  lock(&lk1);
  lock(&lk2);
  unlock(&lk1);
  unlock(&lk2);

  struct some_object *obj = malloc(sizeof(struct some_object));
  assert(obj);
  object_init(obj);
  lock(&obj->lock);

  lock(&lk2);
  lock(&lk1);
}
