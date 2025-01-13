CFLAGS := -O2 -I$(TLIB_PATH)

all: main.c sum-spin.c sum-mutex.c sum-atomic.c
	gcc $(CFLAGS) -o atomic sum-atomic.c main.c
	gcc $(CFLAGS) -o mutex sum-mutex.c main.c
	gcc $(CFLAGS) -o spin sum-spin.c main.c

clean:
	rm -f atomic mutex spin
