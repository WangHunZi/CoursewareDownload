#include <am.h>
#include <klib.h>
#include <klib-macros.h>

typedef union thread {
    struct {
        const char    *name;
        void          (*entry)(void *);
        Context       context;
        union thread  *next;
        char          end[0];
    };
    uint8_t stack[8192];
} Thread;

void T1(void *);
void T2(void *);
void T3(void *);

Thread threads[] = {
    // Context for the bootstrap code:
    { .name = "_", .entry = NULL, .next = &threads[1] },

    // Thread contests:
    { .name = "1", .entry = T1, .next = &threads[2] },
    { .name = "2", .entry = T2, .next = &threads[3] },
    { .name = "3", .entry = T3, .next = &threads[1] },
};
Thread *current = &threads[0];

Context *on_interrupt(Event ev, Context *ctx) {
    // Save context.
    current->context = *ctx;

    // Thread schedule.
    current = current->next;

    // Restore current thread's context.
    return &current->context;
}

int main() {
    cte_init(on_interrupt);

    for (int i = 1; i < LENGTH(threads); i++) {
        Thread *t = &threads[i];
        t->context = *kcontext(
            // a Thread object:
            // +--------------------------------------------+
            // | name, ... end[0] | Kernel stack ...        |
            // +------------------+-------------------------+
            // ^                  ^                         ^     
            // t                  &t->end                   t + 1
            (Area) { .start = &t->end, .end = t + 1, },
            t->entry, NULL
        );
    }

    yield();
    assert(0);  // Never returns.
}


void delay() {
    for (int volatile i = 0;
         i < 10000000; i++);
}

void T1(void *arg) { while (1) { putch('A'); delay(); } }
void T2(void *arg) { while (1) { putch('B'); delay(); } }
void T3(void *arg) { while (1) { putch('C'); delay(); } }
