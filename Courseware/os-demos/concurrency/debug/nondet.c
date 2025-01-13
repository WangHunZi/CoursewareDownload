#include <stdio.h>

int random() {
    int ret;
    asm volatile("rdrand %0" : "=r"(ret));
    return ret & 0xf;
}

int main() {
    int volatile a[] = { -1, -1, -1, -1 };
    for (int i = 0; i < 4; i++) {
        a[i] = random();
    }

    printf("%d %d %d %d\n", a[0], a[1], a[2], a[3]);
}
