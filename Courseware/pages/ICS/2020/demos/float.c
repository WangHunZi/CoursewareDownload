#include <stdio.h>
#include <float.h>
#include <stdint.h>
#include <math.h>

int main() {
  float x = FLT_MAX;
  printf("x = %e (10^%.1f)\n", x, log10(x));

  printf("========================================\n");

  float y = 1e38;
  printf("y         = %.0f\n", y);
  printf("y + 1e30f = %.0f\n", y + 1e30f);
  printf("y + 1e31f = %.0f\n", y + 1e31f);

  printf("========================================\n");

  unsigned long n1 = 0, n2 = 0, n3 = 0;
  union { float f; int i; } z;

  for (uint32_t i = 0; ; i++) {
    z.i = i;
    if (-1.0 < z.f && z.f < 1.0) n1++;
    if (-0.5f < z.f && z.f < 0.5f) n2++;
    if (-0.001 < z.f && z.f < 0.001) n3++;
    if (i == UINT32_MAX) break;
  }

  double n = (double)UINT32_MAX + 1;
  printf("%.2lf%% of floats are in (-1, 1)\n", (double)n1 / n * 100);
  printf("%.2lf%% of floats are in (-0.5, 0.5)\n", (double)n2 / n * 100);
  printf("%.2lf%% of floats are in (-0.001, 0.001)\n", (double)n3 / n * 100);
}
