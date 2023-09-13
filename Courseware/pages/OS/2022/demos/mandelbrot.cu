#include <stdio.h>
#include <stdint.h>

#define MAX_ITER 100
#define DIM 12800
static uint32_t colors[MAX_ITER + 1];
static uint32_t data[DIM * DIM];

__device__ uint32_t mandelbrot(double x, double y) {
  double zr = 0, zi = 0, zrsqr = 0, zisqr = 0;
  int i;

  for (i = 0; i < MAX_ITER; i++) {
    zi = zr * zi * 2 + y;
    zr = zrsqr - zisqr + x;
    zrsqr = zr * zr;
    zisqr = zi * zi;
    if (zrsqr + zisqr > 4.0) {
      break; // SIMT threads diverges here!
    }
  }
  
  return i;
}

__global__ void mandelbrot_kernel(uint32_t *data, double xmin, double ymin, double step, uint32_t *colors) {
  int pix_per_thread = DIM * DIM / (gridDim.x * blockDim.x);
  int tId = blockDim.x * blockIdx.x + threadIdx.x;
  int offset = pix_per_thread * tId;
  for (int i = offset; i < offset + pix_per_thread; i++) {
    int x = i % DIM;
    int y = i / DIM;
    double cr = xmin + x * step;
    double ci = ymin + y * step;
    data[y * DIM + x] = colors[mandelbrot(cr, ci)];
  }
  if (gridDim.x * blockDim.x * pix_per_thread < DIM * DIM
      && tId < (DIM * DIM) - (blockDim.x * gridDim.x)) {
    int i = blockDim.x * gridDim.x * pix_per_thread + tId;
    int x = i % DIM;
    int y = i / DIM;
    double cr = xmin + x * step;
    double ci = ymin + y * step;
    data[y * DIM + x] = colors[mandelbrot(cr, ci)];
  }
}

int main() {
  float freq = 6.3 / MAX_ITER;
  for (int i = 0; i < MAX_ITER; i++) {
    char r = sin(freq * i + 3) * 127 + 128;
    char g = sin(freq * i + 5) * 127 + 128;
    char b = sin(freq * i + 1) * 127 + 128;
    colors[i] = b + 256 * g + 256 * 256 * r;
  }
  colors[MAX_ITER] = 0;

  uint32_t *dev_colors, *dev_data;
  cudaMalloc((void**)&dev_colors, sizeof(colors));
  cudaMalloc(&dev_data, sizeof(data));
  cudaMemcpy(dev_colors, colors, sizeof(colors), cudaMemcpyHostToDevice);

  double xcen = -0.5, ycen = 0, scale = 3;
  mandelbrot_kernel<<<512, 512>>>(
    dev_data,
    xcen - (scale / 2),
    ycen - (scale / 2),
    scale / DIM,
    dev_colors
  );

  cudaMemcpy(data, dev_data, sizeof(data), cudaMemcpyDeviceToHost);
  cudaFree(dev_data);
  cudaFree(dev_colors);

  FILE *fp = fopen("mandelbrot.ppm", "w");
  fprintf(fp, "P6\n%d %d 255\n", DIM, DIM);
  for (int i = 0; i < DIM * DIM; i++) {
    fputc((data[i] >> 16) & 0xff, fp);
    fputc((data[i] >>  8) & 0xff, fp);
    fputc((data[i] >>  0) & 0xff, fp);
  }
  
  return 0;
}
