#include <thread.h>
#include <stdatomic.h>
#include <math.h>

#define W 6400
#define H 6400
#define IMG_FILE "mandelbrot.ppm"

int nthread;
int pic[W][H];
_Atomic int done = 0;

void write_ppm(FILE *fp, int step);

static inline int worker_for(int x, int y) {
    return x / (W / nthread);
}

static inline double mandelbrot(double x, double y) {
    int n = 0;
    double a = 0, b = 0, c, d;
    while ((c = a * a) + (d = b * b) < 4 && n++ < 880) {
        b = 2 * a * b + y * 1024 / H * 8e-9 - 0.645411;
        a = c - d + x * 1024 / W * 8e-9 + 0.356888;
    }
    return n;
}

void T_worker(int tid) {
    // Every pixel can be computed in parallel.

    for (int x = 0; x < W; x++)
        for (int y = 0; y < H; y++)
            if (worker_for(x, y) == tid - 1) {
                pic[x][y] = mandelbrot(x, y);
            }

    atomic_fetch_add(&done, 1);
}

void T_monitor() {
    float ms = 0;

    while (atomic_load(&done) != nthread) {
        // Low-resolution preview (step = 256);
        // piped to real-time viewing
        FILE *fp = popen("viu -", "w");
        assert(fp);
        write_ppm(fp, W / 256);
        pclose(fp);

        usleep(1000000 / 2);
        ms += 1000 / 2;
    }
    printf("Approximate render time: %.1lfs\n", ms / 1000);

    // High-resolution final image (3200 x 3200)
    FILE *fp = fopen(IMG_FILE, "w");
    assert(fp);
    write_ppm(fp, 2);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    assert(argc == 2);
    nthread = atoi(argv[1]);

    // A monitor thread for real-time rendering
    create(T_monitor);

    // OpenMP parallel threads; equivalent to:
    //
    // for (int i = 0; i < nthread; i++) {
    //     create(T_worker);
    // }
    // join();
    //
    // We have (implicitly) defined a computation DAG.
    #pragma omp parallel num_threads(nthread)
    #pragma omp for schedule(static)
    for (int i = 0; i < nthread; i++) {
        T_worker(i + 1);
    }

    join();  // Wait for T_monitor, which syncs with T_workers.
    return 0;
}

void write_ppm(FILE *fp, int step) { 
    // Portable Pixel Map (PPM)

    int w = W / step, h = H / step;

    fprintf(fp, "P6\n%d %d 255\n", w, h);
    for (int j = 0; j < H; j += step) {
        for (int i = 0; i < W; i += step) {
            int n = pic[i][j];
            int r = 255 * pow((n - 80) / 800.0, 3);
            int g = 255 * pow((n - 80) / 800.0, 0.7);
            int b = 255 * pow((n - 80) / 800.0, 0.5);
            fprintf(fp, "%c%c%c", r, g, b);
        }
    }
}
