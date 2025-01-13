int foo_volatile(int volatile *p) {
    // *p is volatile.
    *p = 1;

    // Each access to a volatile variable is treated as a
    // side-effect, so this write should also not be 
    // optimized out.
    *p = 1;

    return *p;
}

int foo_volatile2(int * volatile p) {
    // Only p is volatile.
    *p = 1;

    // While the pointer is volatile, this does not prevent
    // the compiler from optimizing the writes to the
    // pointed-to value.
    *p = 1;

    return *p;
}

int foo_barrier(int *p) {
    *p = 1;

    // Inserting an inline assembly code that acts as a compiler
    // barrier. This prevents the compiler from reordering
    // reads/writes across this point.
    //
    // The "memory" clobber tells the compiler that memory is
    // being modified, so it cannot assume values loaded before
    // are still valid.
    asm volatile("" : : : "memory");

    *p = 1;
    return *p;
}
