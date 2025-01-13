minimal.img: Makefile
	( echo "eb fe 11 22 33 44" | xxd -r -p; \
	  cat /dev/zero | head -c 504; \
	  echo "55 aa" | xxd -r -p \
	) > minimal.img

run: minimal.img
	qemu-system-x86_64 -monitor stdio $<

debug: minimal.img
	qemu-system-x86_64 -s -S $< &
	gdb -x init.gdb

clean:
	rm -f *.img

.PHONY: run debug
