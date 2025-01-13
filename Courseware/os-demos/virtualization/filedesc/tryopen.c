#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void try_open(const char *fname) {
    int fd = open(fname, O_RDWR);
    // fd is a "pointer" to a kernel object.

    printf("open(\"%s\") = %d\n", fname, fd);

    if (fd < 0) {
        perror(fname);
        goto release;
    } else {
        // ...
    }

release:
    if (fd >= 0) {
        close(fd);
    }
}

int main() {
    try_open("/something/not/exist");
    try_open("/dev/sda"); // hard drive
}
