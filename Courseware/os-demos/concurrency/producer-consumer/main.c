#include <thread.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void T_produce();
void T_consume();
extern int n;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s depth num-thread-pairs\n", argv[0]);
        return 1;
    }
    n = atoi(argv[1]);
    int t = atoi(argv[2]);

    for (int i = 0; i < t; i++) {
        create(T_produce);
        create(T_consume);
    }
    join();
}
