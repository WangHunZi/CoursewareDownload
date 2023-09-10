#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#define NTASK 2

typedef union task {
  struct {
    union task *next;
    void      (*entry)(void *);
    Context    *context;
  };
  uint8_t stack[8192];
} Task;

Task *current, tasks[NTASK];

void thread_entry(void *arg) {
  while (1) {
    yield();
  }
}

Context *on_interrupt(Event ev, Context *ctx) {
  if (!current) {
    current = &tasks[0];  // First trap
  } else {
    current->context = ctx;
    current = current->next;
  }
  return current->context;
}

int main() {
  cte_init(on_interrupt);

  for (int i = 0; i < LENGTH(tasks); i++) {
    Task *task    = &tasks[i];
    Area stack    = (Area) { &task->context + 1, task + 1 };
    task->context = kcontext(stack, thread_entry, NULL);
    task->next    = &tasks[(i + 1) % LENGTH(tasks)];
  }
  yield();
}
