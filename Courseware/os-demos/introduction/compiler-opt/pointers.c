int foo(int *p) {
    *p = 1;
    *p = 1;

    // Returning the value pointed by p, which must be 1.
    return *p;
}

// Suppose we don't have any knowledge about external(); e.g.,
// we can't inline it.
void external();

int foo_func_call(int *p) {
    *p = 1;

    // external() may read *p. Therefore, we must write value
    // back before calling external().
    external();

    // This should not be removed by a compiler because
    //  external() might have modified it.
    *p = 1;

    // Return the value, which must be 1.
    return *p;
}
