#include <unistd.h>
#include <stdio.h>

int main() {
    char *const argv[] = {
        "/bin/bash",
        "-c",
        "env",
        NULL,
    };

    char *const envp[] = {
        "HELLO=WORLD",
        NULL,
    };

    // Reset the state machine to "/bin/bash"
    execve(argv[0], argv, envp);

    // We are here only on error.
    printf("Hello, World!\n");
}
