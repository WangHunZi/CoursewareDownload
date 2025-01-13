extern long sum, N;

void T_sum() {
    for (int i = 0; i < N; i++) {
        asm volatile(
            "lock addq $1, %0" : "+m"(sum)
        );
    }
}
