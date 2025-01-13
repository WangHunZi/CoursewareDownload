#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    const char *filepath = "example.txt";
    int fd = open(filepath, O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        close(fd);
        exit(EXIT_FAILURE);
    }

    size_t length = sb.st_size + 4096;
    void *addr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Overwrite this file.
    char *data = (char *)addr;
    for (size_t i = sb.st_size; i < length; ++i) {
        printf("Write byte %zu\n", i);
        fflush(stdout);
        data[i] = 'A';
    }

    if (munmap(addr, length) == -1) {
        perror("munmap");
    }
    close(fd);

    return 0;
}
