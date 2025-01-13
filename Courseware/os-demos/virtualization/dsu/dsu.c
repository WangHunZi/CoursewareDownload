#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <assert.h>

__attribute__((noinline)) void foo() {
    printf("In old function %s\n", __func__);
}

__attribute__((noinline)) void foo_new() {
    printf("In new function %s\n", __func__);
}

// 48 b8 (64-bit imm)   movabs $imm,%rax
// ff e0                jmpq   *%rax
const char PATCH[] = "\x48\xb8--------\xff\xe0";

void DSU(void *func, void *func_new) {
    int flag = PROT_WRITE | PROT_READ | PROT_EXEC, rc, np;

    // Grant write permission to the memory
    // We must handle boundary cases
    uintptr_t fn = (uintptr_t)func;
    uintptr_t base = fn & ~0xfff;
    if (fn + sizeof(PATCH) > base + 4096) {
        np = 2;  // Cross page boundary
    } else {
        np = 1;
    }
    printf("np = %d\n", np);

    rc = mprotect((void *)base, np * 4096, flag);
    assert(rc == 0);  // Not expecting a failure
  
    // Patch the first instruction (this is UB in C spec)
    memcpy(func, PATCH, sizeof(PATCH));
    memcpy((char *)func + 2, &func_new, sizeof(func_new));

    // Revoke the write permission
    rc = mprotect((void *)base, np * 4096, PROT_READ | PROT_EXEC);
    assert(rc == 0);  // Not expecting a failure
}

int main() {
    setbuf(stdout, NULL);
    foo();
    DSU(foo, foo_new);  // Dynamic software update
    foo();
}
