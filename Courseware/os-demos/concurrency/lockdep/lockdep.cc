#include "lockdep.h"
#include <vector>
#include <set>
#include <iostream>
#include <cassert>
#include <string>

using std::string, std::cout, std::endl, std::set;
using edge = std::pair<string, string>;

// Subtle: this *memory-leak* is intentional.
// There lacks a mechanism to force destructor join()
// to be called *before* global objects' destructions.
static auto* vertices = new set<string>();
static auto* edges = new set<edge>();

// It's safe to auto-destruct thread-local objects.
static thread_local set<string> held_locks;

static void check_cycles();
static mutex_t GL = MUTEX_INIT();

class HoldLock {
    mutex_t *lk;

public:
    HoldLock(mutex_t *lock): lk(lock) {
        mutex_lock(lk);
    }

    ~HoldLock() {
        mutex_unlock(lk);
    }
};

// This function is to be C-linked; name mangling is disabled.
extern "C"
void lock(lock_t *lk) {
    // Vertices and edges (set<>s) are shared across threads.
    // Keep them safe with an RAII-guarded lock.
    { [[maybe_unused]] HoldLock h(&GL);
        bool updated = false;

        vertices->insert(lk->name);
        for (auto name : held_locks) {
            edge e(name, lk->name);
            if (!edges->contains(e)) {
                edges->insert(e);
                updated = true;
            }
        }

        if (updated) {
            check_cycles();
        }
    }

    // The held_locks is declared as thread_local.
    // No need for locks.
    held_locks.insert(lk->name);

    mutex_lock(&lk->mutex);
}

extern "C"
void unlock(lock_t *lk) {
    mutex_unlock(&lk->mutex);

    held_locks.erase(lk->name);
}

static void check_cycles() {
    // At this point, we must have held GL.
    // Unfortunately, there is no graceful way to check if
    // this lock is held by the current thread.
    assert(pthread_mutex_trylock(&GL) == EBUSY);

    // Transitive closure by Floyd-Warshall's algorithm.
    for (auto v: *vertices)
        for (auto u: *vertices)
            for (auto w: *vertices)
                if (edges->contains({u, v}) && edges->contains({v, w})) {
                    edges->insert({u, w});
                }

    // Check for cycles
    cout << endl << "Lockdep check:" << endl;
    for (auto [u, v] : *edges) {
        cout << "    " << u << " -> " << v << endl;
        if (u == v) {
            cout << "    \033[31m!!! Cycle detected for "
                 << u << "\033[0m" << endl;
        }
    }
}
