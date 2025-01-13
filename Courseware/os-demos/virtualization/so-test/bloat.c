void bloat() {
    // 100M of nops
    asm volatile(
        ".fill 104857600, 1, 0x90"
    );
}
