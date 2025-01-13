// Author: GPT-4-turbo

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void do_parent(int fd) {
    const char *msg = "Hello, world!";

    printf("[%d] Write: '%s'\n", getpid(), msg);
    write(fd, msg, strlen(msg) + 1);

    close(fd);

    // Wait for the child to finish
    wait(NULL);

    printf("[%d] Done.\n", getpid());
}

void do_child(int fd) {
    static char buf[1024];

    ssize_t num_read = read(fd, buf, sizeof(buf));
    if (num_read == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    printf("[%d] Got: '%s'\n", getpid(), buf);

    // Close the read end of the pipe
    close(fd);
}

int main() {
    int pipefd[2];

    // Create a pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork the current process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child
        close(pipefd[1]); // Close unused write end
        do_child(pipefd[0]);
    } else {
        // Parent
        close(pipefd[0]); // Close unused read end
        do_parent(pipefd[1]);
    }

    return 0;
}
