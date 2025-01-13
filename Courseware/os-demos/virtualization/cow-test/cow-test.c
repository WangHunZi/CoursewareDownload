#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#define SIZE (1 << 30)

uint32_t random_32() {
    uint32_t x = 0;

    int fd = open("/dev/urandom", O_RDONLY);
    assert(fd > 0);

    // Returns a random 32-bit integer.
    read(fd, &x, sizeof(x));

    close(fd);

    return x;
}

int main() {
    // Allocate 1GB private memory and fill it with data.
    char *data = malloc(SIZE);
    memset(data, '_', SIZE);

    // Create 1,000 new processes!

    for (int i = 0; i < 1000; i++) {
        if (fork() == 0) {
            // A new process is forked().
            break;
        }
    }

    // All processes go here.

    int i = random_32() % SIZE;

    data[i] = '.';
    printf("pid = %d, write data[%u]\n", getpid(), i);

    while (1) {
        sleep(1);
    }
}
