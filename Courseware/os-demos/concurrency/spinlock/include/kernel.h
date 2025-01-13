#ifndef KERNEL_H__
#define KERNEL_H__

struct cpu {
    int noff;
    int intena;
};

extern struct cpu cpus[];
#define mycpu (&cpus[cpu_current()])

#define panic(...) \
    do { \
        printf("Panic: " __VA_ARGS__); \
        halt(1); \
    } while (0)

#endif
