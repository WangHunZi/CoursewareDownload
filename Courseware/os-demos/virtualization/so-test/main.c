#include <stdio.h>
#include <unistd.h>

// Dynamically linked.
void bloat();

int main() {
    bloat();
    printf("bloat() called; sleeping...\n");
    while (1) {
        sleep(1);
    }
}
