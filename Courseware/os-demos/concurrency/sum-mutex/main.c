#include <thread.h>

long sum = 0, N = 10000000;
extern void T_sum();

int main(int argc, char *argv[]) {
    int T = argv[1] ? atoi(argv[1]) : 2;
    N /= T;

    for (int i = 0; i < T; i++) {
        create(T_sum);
    }
    join();

    printf("sum = %ld\n", sum);
}
