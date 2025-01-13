#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

double gettime() {
    struct timeval t;
    gettimeofday(&t, NULL); // trap-less system call
    return t.tv_sec + t.tv_usec / 1000000.0;
}

int main() {
    // trap-less system call
    printf("Time stamp: %ld\n", time(NULL));

    double st = gettime();
    sleep(1);
    double ed = gettime();
    printf("Time: %.6lfs\n", ed - st);
}
