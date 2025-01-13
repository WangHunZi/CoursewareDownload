#include <stdio.h>

// This x is from liba.
extern int x;

// This y is from libb.
extern int __attribute__((visibility("hidden"))) y;

void *get_x_addr() {
    return &x;
}

void *get_y_addr() {
    return &y;
}

void set_x() {
    x = 1;
}

void set_y() {
    y = 1;
}

void *get_stderr() {
    return &stderr;
}

void B() {
    printf("B   : &stderr = %p\n", &stderr);
    printf("B   : &x      = %p\n", &x);
}
