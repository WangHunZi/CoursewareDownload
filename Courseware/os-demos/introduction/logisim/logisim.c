#include <logisim.h>
#include <stdio.h>

// Wire and registers in the circuit
wire X, Y, X1, Y1, A, B, C, D, E, F, G;
reg b1 = {.in = &X1, .out = &X};
reg b0 = {.in = &Y1, .out = &Y};

int main() {
    CLOCK_CYCLE {
        // 1. Propagate wire values through combinatorial logic
        X1 = AND(NOT(X), Y);
        Y1 = NOT(OR(X, Y));
        A = D = E = NOT(Y);
        B = 1;
        C = NOT(X);
        F = Y1;
        G = X;

        // 2. Edge triggering: Lock values in the flip-flops
        b0.value = *b0.in;
        b1.value = *b1.in;
        *b0.out = b0.value;
        *b1.out = b1.value;

        // 3. End of a cycle; display output wire values
        #define PRINT(X) printf(#X " = %d; ", X)
        PRINT(A);
        PRINT(B);
        PRINT(C);
        PRINT(D);
        PRINT(E);
        PRINT(F);
        PRINT(G);
        printf("\n");
        fflush(stdout);
        sleep(1);
    }
}
