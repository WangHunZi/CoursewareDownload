#include <am.h>
#include <kernel.h>
#include <klib.h>

#define UNLOCKED  0
#define LOCKED    1

typedef struct {
    const char *name;
    int status;
    struct cpu *cpu;
} spinlock_t;

#define spin_init(name_) \
    ((spinlock_t) { \
        .name = name_, \
        .status = UNLOCKED, \
        .cpu = NULL, \
    })
void spin_lock(spinlock_t *lk);
void spin_unlock(spinlock_t *lk);
