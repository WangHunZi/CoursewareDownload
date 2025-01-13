#include <thread-sync.h>

typedef struct {
    mutex_t mutex;
    const char *name;
} lock_t;

#define STRINGIFY(s) #s
#define TOSTRING(s) STRINGIFY(s)
#define LOCK_INIT() \
    ((lock_t) { \
        .mutex = MUTEX_INIT(), \
        .name = __FILE__ ":" TOSTRING(__LINE__), \
    })

#ifndef __cplusplus
void lock(lock_t *lk);
void unlock(lock_t *lk);
#else
extern "C" void lock(lock_t *lk);
extern "C" void unlock(lock_t *lk);
#endif
