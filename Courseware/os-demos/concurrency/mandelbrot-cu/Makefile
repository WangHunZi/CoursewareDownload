a.out: mandelbrot.cu
	nvcc -g -O2 $< -o $@
	cuobjdump --dump-ptx a.out > mandelbrot.ptx

run:
	nvprof ./a.out

