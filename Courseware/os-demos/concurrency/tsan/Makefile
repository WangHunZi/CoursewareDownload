CFLAGS := -O2 -g -I$(TLIB_PATH) -fsanitize=thread

race: race.c Makefile
	gcc $(CFLAGS) -o $@ $<

clean:
	rm -f race
