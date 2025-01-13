#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <elf.h>
#include <fcntl.h>
#include <sys/mman.h>

void my_execve(const char *file, char *argv[], char *envp[]);
void *init_proc_stack(char *argv[], char *envp[]);

int main(int argc, char *argv[], char *envp[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file [args...]\n", argv[0]);
        exit(1);
    }

    my_execve(argv[1], argv + 1, envp);
}

void my_execve(const char *file, char *argv[], char *envp[]) {
    // WARNING: This execve leaks memory and ignores
    // error checing for brevity.

    int fd = open(file, O_RDONLY);
    assert(fd > 0);

    // Map ELF header to memory
    Elf64_Ehdr *h = mmap(NULL, 4096, PROT_READ, MAP_PRIVATE, fd, 0);

    #define ROUND(x, align) (((uintptr_t)x) & ~(align - 1))

    Elf64_Phdr *pht = (Elf64_Phdr *)((char *)h + h->e_phoff);
    for (int i = 0; i < h->e_phnum; i++) {
        Elf64_Phdr *p = &pht[i];

        if (p->p_type == PT_LOAD) {
            // Memory map region
            uintptr_t align = p->p_align;
            uintptr_t map_beg = ROUND(p->p_vaddr, align);

            // Memory map flags
            int prot = 0;
            if (p->p_flags & PF_R) prot |= PROT_READ;
            if (p->p_flags & PF_W) prot |= PROT_WRITE;
            if (p->p_flags & PF_X) prot |= PROT_EXEC;

            // Memory map size
            uintptr_t map_sz = ROUND(p->p_filesz + align - 1, align);
            uintptr_t alloc_sz = p->p_memsz - p->p_filesz;

            // Map file contents
            mmap(
                (void *)map_beg,               // addr, rounded to ALIGN
                map_sz,                        // length
                prot,                          // protection
                MAP_PRIVATE | MAP_FIXED,       // flags, private & strict
                fd, ROUND(p->p_offset, align)  // file and offset
            );

            // Map anonymous memory (bss)
            if (alloc_sz > 0) {
                mmap(
                    (void *)(map_beg + map_sz),// addr
                    alloc_sz,                  // length,
                    prot,                      // protection
                    MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, // flags
                    -1, 0                      // no file and offset
                );
            }

            // (Should check for return values.)
        }
    }
    close(fd);

    void *rsp = init_proc_stack(argv, envp);
    asm volatile(
        "mov $0, %%rdx;" // required by System-V ABI
        "mov %0, %%rsp;" // loader allocated
        "jmp *%1" : : "a"(rsp), "b"(h->e_entry)
    );
}

void *init_proc_stack(char *argv[], char *envp[]) {
    _Alignas(16) static char stack[1 << 20];
    static char rnd[16];

    void *sp = (void *)(stack + sizeof(stack) - 8192);
    #define push(sp, T, ...) ({ \
        *((T*)sp) = (T)__VA_ARGS__; \
        sp = (void *)((uintptr_t)(sp) + sizeof(T)); \
    })

    void *rsp = sp;

    // Calculate argc
    int argc = 0;
    while (argv[argc])
        argc++;

    // Create initial process stack
    push(sp, intptr_t, argc);

    // argv[], NULL-terminate
    for (int i = 0; i < argc; i++) {
        push(sp, intptr_t, argv[i]);
    }
    push(sp, intptr_t, 0);

    // envp[], NULL-terminate
    for (; *envp; envp++) {
        if (!strchr(*envp, '_')) {
            // remove some verbose ones
            push(sp, intptr_t, *envp);
        }
    }
    push(sp, intptr_t, 0);

    // auxv[], AT_NULL-terminate
    push(sp, Elf64_auxv_t,
        {.a_type = AT_RANDOM, .a_un.a_val = (uintptr_t)rnd}
    );
    push(sp, Elf64_auxv_t,
        {.a_type = AT_NULL}
    );

    // This stack layout is defined by System-V ABI.
    return rsp;
}
