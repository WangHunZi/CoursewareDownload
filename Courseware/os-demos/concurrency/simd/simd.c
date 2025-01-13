#include <stdio.h>

float dot_product(float *x, float *y) {
    float result;

    asm (
        "movaps (%1), %%xmm0;"   // xmm0 = x[0:4]
        "movaps (%2), %%xmm1;"   // xmm1 = y[0:4]

        "mulps %%xmm1, %%xmm0;"  // xmm0 .*= xmm1
        "haddps %%xmm0, %%xmm0;" // pair-wise horizontal add
        "haddps %%xmm0, %%xmm0;" // xmm0[0] is the result.
        : "=x" (result)
        : "r" (x), "r" (y)
        : "%xmm1"
    );

    return result;
}

int main() {
    float x[4] = {1.0, 2.0, 3.0, 4.0};
    float y[4] = {5.0, 6.0, 7.0, 8.0};

    printf("Dot product: %f\n",
        dot_product(x, y)
    );

    return 0;
}
