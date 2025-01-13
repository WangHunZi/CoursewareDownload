nondet: nondet.c
	gcc -O1 -g -o $@ $<

debug: nondet
	gdb -x reverse.gdb nondet

clean:
	rm -f nondet
