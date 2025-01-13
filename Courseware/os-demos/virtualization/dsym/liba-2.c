#include <stdio.h>

extern int x;

void A() {
    printf("liba: x = %d\n", x++);
}
