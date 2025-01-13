all: peterson

CFLAGS := -I$(TLIB_PATH) -O2

peterson: peterson.c Makefile
	gcc $(CFLAGS) -o $@ $<

clean:
	rm -f peterson
