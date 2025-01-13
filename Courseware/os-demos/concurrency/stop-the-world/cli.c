int main() {
    // Clear FL_IF in the CPU.
    // Interrupt disabled.
    asm volatile("cli");

    // Set FL_IF in the CPU.
    // Interrupt enabled.
    asm volatile("sti");

    while (1);
}
