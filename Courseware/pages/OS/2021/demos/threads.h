#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

struct thread {
  int id;
  pthread_t thread;
  void (*entry)(int);
  struct thread *next;
};

struct thread *threads;
void (*join_fn)();

// ========== Basics ==========

__attribute__((destructor)) static void join_all() {
  for (struct thread *next; threads; threads = next) {
    pthread_join(threads->thread, NULL);
    next = threads->next;
    free(threads);
  }
  join_fn ? join_fn() : (void)0;
}

static inline void *entry_all(void *arg) {
  struct thread *thread = (struct thread *)arg;
  thread->entry(thread->id);
  return NULL;
}

static inline void create(void *fn) {
  struct thread *cur = (struct thread *)malloc(sizeof(struct thread));
  assert(cur);
  cur->id    = threads ? threads->id + 1 : 1;
  cur->next  = threads;
  cur->entry = (void (*)(int))fn;
  threads    = cur;
  pthread_create(&cur->thread, NULL, entry_all, cur);
}

static inline void join(void (*fn)()) {
  join_fn = fn;
}

// ========== Synchronization ==========

#include <stdint.h>

intptr_t atomic_xchg(volatile intptr_t *addr,
                               intptr_t newval) {
  // swap(*addr, newval);
  intptr_t result;
  asm volatile ("lock xchg %0, %1":
    "+m"(*addr), "=a"(result) : "1"(newval) : "cc");
  return result;
}

intptr_t locked = 0;

static inline void lock() {
  while (1) {
    intptr_t value = atomic_xchg(&locked, 1);
    if (value == 0) {
      break;
    }
  }
}

static inline void unlock() {
  atomic_xchg(&locked, 0);
}

#include <semaphore.h>

#define P sem_wait
#define V sem_post
#define SEM_INIT(sem, val) sem_init(&(sem), 0, val)
