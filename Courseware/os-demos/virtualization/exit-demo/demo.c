#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

void func() {
    printf("Goodbye, Cruel OS World!\n");
}

int main(int argc, char *argv[]) {
    // This is a convenient mechanism for 
    atexit(func);

    if (argc < 2) {
        // We can reference the return code $? in bash.
        // Our online judge also uses this return code.
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "exit") == 0) {
        // This is a libc exit.
        exit(0);
    }

    if (strcmp(argv[1], "_exit") == 0) {
        // This is an immediate operating system exit.
        // This _exit() is provided by libc.
        _exit(0);
    }

    if (strcmp(argv[1], "__exit") == 0) {
        // This is an even more "operating system" exit.
        syscall(SYS_exit, 0);
    }
}
