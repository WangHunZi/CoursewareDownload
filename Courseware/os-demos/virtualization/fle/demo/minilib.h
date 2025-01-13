// As if this is the C standard library headers.

#include <stddef.h>
#include <sys/syscall.h>

long syscall(int num, ...);
size_t strlen(const char *s);
char *strcpy(char *d, const char *s);
char *strchr(const char *s, int c);
void print(const char *s, ...);
