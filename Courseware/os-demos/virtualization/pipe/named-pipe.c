// Author: GPT-4-turbo

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

// We also have UNIX domain sockets for local inter-process
// communication--they also have a name in the file system
// like "/var/run/docker.sock". This is similar to a named
// pipe.
#define PIPE_NAME "/tmp/my_pipe"

void pipe_read() {
    int fd = open(PIPE_NAME, O_RDONLY);
    char buffer[1024];

    if (fd == -1) {
        perror("open");
        exit(1);
    }

    // Read from the pipe
    int num_read = read(fd, buffer, sizeof(buffer));
    if (num_read > 0) {
        printf("Received: %s\n", buffer);
    } else {
        printf("No data received.\n");
    }
    close(fd);
}

void pipe_write(const char *content) {
    // Open the pipe for writing
    int fd = open(PIPE_NAME, O_WRONLY);

    if (fd == -1) {
        perror("open");
        exit(1);
    }

    // Write the message to the pipe
    write(fd, content, strlen(content) + 1);
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s read|write [message]\n", argv[0]);
        return 1;
    }

    // Create the named pipe if it does not exist
    if (mkfifo(PIPE_NAME, 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo");
            return 1;
        }
    } else {
        printf("Created " PIPE_NAME "\n");
    }

    if (strcmp(argv[1], "read") == 0) {
        pipe_read();
    } else if (strcmp(argv[1], "write") == 0) {
        pipe_write(argv[2]);
    } else {
        fprintf(stderr, "Invalid command. Use 'read' or 'write'.\n");
        return 1;
    }

    return 0;
}
