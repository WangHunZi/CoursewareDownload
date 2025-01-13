LIB := libbloat.so

all: libbloat.so bloat

libbloat.so: bloat.c
	gcc -shared -o $@ $^

bloat: main.o
	gcc -o $@ $^ -L. -lbloat

clean:
	rm -f *.o libbloat.so bloat

run:
.PHONY: all clean
