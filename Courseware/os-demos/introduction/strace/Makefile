minimal: minimal.S
	gcc -S minimal.S > minimal.s
	as -o minimal.o minimal.s
	ld -o minimal minimal.o

gcc-demo:
	strace -f gcc -o hello hello.c

xedit-demo:
	strace xedit

clean:
	rm -f minimal minimal.o minimal.s hello
