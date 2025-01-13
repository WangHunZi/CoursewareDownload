#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define DEST '+'
#define EMPTY '.'

struct move {
    int move, x, y;
} moves[] = {
    {'>', 0, 1},
    {'v', 1, 0},
    {'<', 0, -1},
    {'^', -1, 0},
};

char map[][512] = {
    "######",
    "#...+#",
    "#..#.#",
    "#..#.#",
    "#....#",
    "######",
    "",
};

void display(int steps);
void dfs(int x, int y, int steps);

int main() {
    dfs(1, 1, 0);
}


void dfs(int x, int y, int steps) {
    // Each search level gets 1 second of delay.
    sleep(1);

    if (map[x][y] == DEST) {
        display(steps);
        exit(0);
    } else {
        int nfork = 0;

        for (struct move *m = moves; m < moves + 4; m++) {
            int x1 = x + m->x, y1 = y + m->y;
            int pid = fork();

            assert(pid >= 0);

            if (pid == 0) {
                // Forked worker process

                map[x][y] = m->move;
                if (map[x1][y1] == DEST || map[x1][y1] == EMPTY) {
                    dfs(x1, y1, steps + 1);
                }
                exit(0);
            } else {
                nfork++;

                // If we wait here, the search will be serialized.
                // waitpid(pid, NULL, 0);
            }
        }

        while (nfork--)
            wait(NULL);
    }
}

void display(int steps) {
    #define append(buf, ...) sprintf(buf + strlen(buf), __VA_ARGS__)

    char buf[4096] = {0};

    append(buf, "%d steps\n", steps);
    for (int i = 0; map[i][0]; i++) {
        for (const char *s = map[i]; *s; s++) {
            const char *draw;
            switch (*s) {
            case EMPTY: draw = "   "; break;
            case DEST:  draw = " ○ "; break;
            case '>':   draw = " → "; break;
            case '<':   draw = " ← "; break;
            case '^':   draw = " ↑ "; break;
            case 'v':   draw = " ↓ "; break;
            default:    draw = "▇▇▇"; break;
            }
            append(buf, draw);
        }
        append(buf, "\n");
    }
    append(buf, "\n");

    write(STDOUT_FILENO, buf, strlen(buf));
}
