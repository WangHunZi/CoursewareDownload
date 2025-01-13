CFLAGS := -O2 -fopenmp -I$(TLIB_PATH)

mandelbrot: mandelbrot.c
	gcc $(CFLAGS) -o $@ $^ -lm

clean:
	rm -f mandelbrot *.jpg *.ppm
