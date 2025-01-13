#include <stdio.h>

void A();
void B();

extern int x;

int main() {
    A();

    printf("main: &stderr = %p\n", &stderr);
    printf("main: &x      = %p\n", &x);

    B();
}
