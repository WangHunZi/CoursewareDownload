all: mem-model

CFLAGS := -O2 -I$(TLIB_PATH)

mem-model: mem-model.c
	gcc $(CFLAGS) -o $@ $<

clean:
	rm -f mem-model
