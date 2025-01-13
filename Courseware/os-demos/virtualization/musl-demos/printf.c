#include <stdio.h>

int main() {
    char buf[64];

    // We expect that all below share implementation!

    // Write to stdout.
    printf("x = %d\n", 1);

    fprintf(stdout, "x = %d\n", 1);
    fprintf(stderr, "x = %d\n", 1);

    // Writes at most size-1 characters to ensure
    // NULL-termination of strings.
    snprintf(buf, sizeof(buf), "x = %d\n", 1);
}
