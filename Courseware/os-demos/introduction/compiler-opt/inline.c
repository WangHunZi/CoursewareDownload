int equals(int x, int y) {
    if (x == y) {
        return 1;
    } else {
        return 0;
    }
}

int foo(int n) {
    // After inlining equals(), the code becomes
    //   (if (n==n) 1 else 0) + (if (n==n-1) 1 else 0)
    // Both branches can be determined immediately; and
    // dead code can be eliminated.
    return equals(n, n) + equals(n, n - 1);
}
