all: sum

CFLAGS := -I$(TLIB_PATH)

sum: sum.c Makefile
	gcc $(CFLAGS) -o $@ $<

clean:
	rm -f sum
