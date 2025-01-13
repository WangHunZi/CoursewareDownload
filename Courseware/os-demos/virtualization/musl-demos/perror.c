#include <stdio.h>

int main() {
    // A wrapper of open system call
    FILE *fp = fopen("nonexist.file", "r");
    if (!fp) {
        perror("open");
        return 1;
    }
}
