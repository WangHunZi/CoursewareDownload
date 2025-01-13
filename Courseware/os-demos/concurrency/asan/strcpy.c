#include <stdio.h>
#include <string.h>

void mystrcpy(char *dest, const char *src) {
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
}

int main() {
    char hello[] = "Hello, world!";
    char goodbye[] = "Goodbye, world!";
    char buf[13]; // Buggy!

    mystrcpy(buf, hello);

    printf("%s\n", buf);
    printf("%s\n", goodbye);
    return 0;
}
