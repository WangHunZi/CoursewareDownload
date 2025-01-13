#include <thread.h>
#include <thread-sync.h>

enum {
    C = 40,
    D = 42,
    E = 44,
    F = 45,
    G = 47,
    A = 49,
    B = 51,
    C0 = C - 12,
    D0 = D - 12,
    E0 = E - 12,
    F0 = F - 12,
    G0 = G - 12,
    A0 = A - 12,
    B0 = B - 12,
};

mutex_t lk = MUTEX_INIT();

int n = -1;

void wait_next_beat(int expect) {
    // This is a spin-wait loop.
retry:
    mutex_lock(&lk);
    // This read is protected by a mutex.
    int got = n;
    mutex_unlock(&lk);

    if (got != expect) goto retry;
}

void release_beat() {
    mutex_lock(&lk);
    n++;
    mutex_unlock(&lk);
}

int NOTES[3][8] = {
    {C, G0, A0, E0, F0, C0, F0, G0},
    {E, B0, C0, G0, A0, E0, A0, B0},
    {G, D,  E,  B0, C0, G0, C0, D0},
};

void T_player(int id) {
    int *notes, expect = 0;
    char cmd[128];

    switch (id) {
        case 1: notes = NOTES[0]; break;
        case 2: notes = NOTES[1]; break;
        case 3: notes = NOTES[2]; break;
    }

    while (1) {
        // This is a spin-wait.
        wait_next_beat(expect);

        int note = notes[expect % 8];

        // This is also UNIX philosophy: make everything
        // work together!
        sprintf(cmd,
            "ffplay -nodisp -autoexit"
            " -loglevel quiet"
            " piano_key_%d.wav"
            " > /dev/null &", note);

        // Should not use "system" like this
        // in production code.
        system(cmd);

        expect++;
    }
}

void T_conductor() {
    // This conductor is not full synchronized with the players.
    // You may make the system out of sync.

    char buf[32];
    while (1) {
        printf("> ");
        fgets(buf, sizeof(buf), stdin);
        release_beat();
    }
}

int main() {
    for (int i = 0; i < 3; i++) {
        create(T_player);
    }
    create(T_conductor);
}
