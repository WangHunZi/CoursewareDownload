all: memory stack

CFLAGS := -O1 -g -I$(TLIB_PATH)

memory: memory.c
	gcc $(CFLAGS) -o $@ $<

stack: stack.c
	gcc $(CFLAGS) -o $@ $<

run: all
	@echo ----- Shared Memory Test -----
	@./memory | sort
	@echo ----- Stack Probe Test -----
	@./stack | sort -nk 3  | tail -n 1

clean:
	rm -f memory stack
