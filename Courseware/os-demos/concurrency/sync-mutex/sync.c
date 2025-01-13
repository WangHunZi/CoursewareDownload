#include <thread.h>
#include <thread-sync.h>

#include "piano.h"

mutex_t locks[] = {
    MUTEX_INIT(),
    MUTEX_INIT(),
    MUTEX_INIT(),
    MUTEX_INIT(),
};

void wait_next_beat(int tid) {
    // Use the mutex for synchronization.
    mutex_lock(&locks[tid - 1]);
}

void release_beat() {
    // Release all locks; players are ready to go.
    for (int i = 0; i < T; i++) {
        mutex_unlock(&locks[i]);
    }
}

void T_player(int id) {
    int *notes = NOTES[id - 1], beat = 0;
    char cmd[128];

    while (1) {
        wait_next_beat(id);

        int cur = (beat++) % N;
        int note = notes[cur];
        printf("%d ", cur);
        fflush(stdout);

        sprintf(cmd,
            "ffplay -nodisp -autoexit"
            " -loglevel quiet"
            " piano_key_%d.wav"
            " > /dev/null &", note);

        // Should not use "system" like this
        // in production code.
        system(cmd);
    }
}

void T_conductor() {
    char buf[32];
    while (1) {
        // This is an "incorrect" synchronization
        // for waiting player threads to finish
        // their printfs.
        usleep(1000);

        printf("> ");
        fflush(stdout);

        fgets(buf, sizeof(buf), stdin);
        if (feof(stdin)) {
            break;
        }

        release_beat();
    }
}

int main() {
    for (int i = 0; i < T; i++) {
        // Make all mutexes "locked".
        mutex_lock(&locks[i]);
    }

    for (int i = 0; i < T; i++) {
        create(T_player);
    }

    // T_conductor will release the locks.
    create(T_conductor);
}
