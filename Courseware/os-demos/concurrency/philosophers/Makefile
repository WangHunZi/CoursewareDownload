all: philosophers

CFLAGS := -O2 -I$(TLIB_PATH)

philosophers: philosophers.c
	gcc $(CFLAGS) -o $@ $^

clean:
	rm -f philosophers
