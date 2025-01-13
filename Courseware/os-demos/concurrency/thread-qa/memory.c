#include <thread.h>

// We claim that n is shared among threads.
int n;

void T_hello(int id) {
    // To validate this claim, each thread reads and increments
    // n, and we expect to see different numbers being printed.
    int i = n++;
    printf("%d\n", i);
}

int main() {
    for (int i = 0; i < 10; i++) {
        create(T_hello);
    }
}
