CFLAGS := -O2 -I$(TLIB_PATH)

sync: sync.c piano_key_1.wav
	gcc $(CFLAGS) -o $@ $<

piano_key_1.wav:
	python3 gen-notes.py

clean:
	rm -f *.wav sync
