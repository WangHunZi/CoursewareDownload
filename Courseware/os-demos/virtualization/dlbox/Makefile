all: dlbox
	./dlbox gcc libhello.S
	./dlbox gcc libc.S
	./dlbox gcc main.S
	./dlbox objdump libhello.dl > libhello.txt
	./dlbox objdump libc.dl > libc.txt
	./dlbox objdump main.dl > main.txt
	rm -f *.o
	
dlbox: dlbox.c
	gcc $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.dl *.txt dlbox
