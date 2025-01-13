CFLAGS := -O2 -I$(TLIB_PATH)

main: main.o lockdep.o
	g++ $(CFLAGS) -o $@ $^

main.o: main.c
	gcc $(CFLAGS) -c -o $@ $^

lockdep.o: lockdep.cc
	g++ -std=c++20 $(CFLAGS) -c -o $@ $^

clean:
	rm -f *.o main
