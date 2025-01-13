all: pc-1 pc-2 pc-cv pc-cv-broadcast

CFLAGS := -O2 -I$(TLIB_PATH)

pc-1: pc-1.c main.c
	gcc $(CFLAGS) -o $@ $^

pc-2: pc-2.c main.c
	gcc $(CFLAGS) -o $@ $^

pc-cv: pc-cv.c main.c
	gcc $(CFLAGS) -o $@ $^

pc-cv-broadcast: pc-cv-broadcast.c main.c
	gcc $(CFLAGS) -o $@ $^

clean:
	rm -f pc-1 pc-2 pc-cv
