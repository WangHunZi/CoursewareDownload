logisim: logisim.c
	gcc -o logisim -I. logisim.c

run: logisim
	./logisim | python3 seg-display.py  # The UNIX Philosophy

clean:
	rm -f logisim

.PHONY: run clean
