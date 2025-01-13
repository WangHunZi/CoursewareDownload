all: join-1 join-2

CFLAGS := -O2 -I$(TLIB_PATH)

join-1: join-1.c main.c
	gcc $(CFLAGS) -o $@ $^

join-2: join-2.c main.c
	gcc $(CFLAGS) -o $@ $^

clean:
	rm -f join-1 join-2
