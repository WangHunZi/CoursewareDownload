// As if this is the implementation of libc.

#include "minilib.h"
#include <stdarg.h>
#include <sys/syscall.h>

long syscall(int num, ...) {
    va_list ap;
    va_start(ap, num);
    register long a0 asm("rax") = num;
    register long a1 asm("rdi") = va_arg(ap, long);
    register long a2 asm("rsi") = va_arg(ap, long);
    register long a3 asm("rdx") = va_arg(ap, long);
    register long a4 asm("r10") = va_arg(ap, long);
    va_end(ap);
    asm volatile("syscall"
                 : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4)
                 : "memory", "rcx", "r8", "r9", "r11");
    return a0;
}

size_t strlen(const char *s) {
    size_t len = 0;
    for (; *s; s++)
        len++;
    return len;
}

char *strcpy(char *d, const char *s) {
    char *r = d;
    while (*s) {
        *d++ = *s++;
    }
    *d = '\0';
    return r;
}

char *strchr(const char *s, int c) {
    for (; *s; s++) {
        if (*s == c)
            return (char *)s;
    }
    return NULL;
}

void print(const char *s, ...) {
    va_list ap;
    va_start(ap, s);
    while (s) {
        syscall(SYS_write, 1, s, strlen(s));
        s = va_arg(ap, const char *);
    }
    va_end(ap);
}

int main();

// libc provides the "_start".
void _start() {
    syscall(
        SYS_exit,
        main()
    );
}
