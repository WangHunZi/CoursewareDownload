# Requires statically linked busybox in $PATH

INIT := /init

initramfs: minimal
# Copy kernel and minimal from the host system
	@mkdir -p build/initramfs/bin
	sudo bash -c "cp /boot/vmlinuz build/ && chmod 666 build/vmlinuz"
	cp minimal build/initramfs/init

# Pack build/initramfs as gzipped cpio archive
	cd build/initramfs && \
	  find . -print0 \
	  | cpio --null -ov --format=newc \
	  | gzip -9 > ../initramfs.cpio.gz

run:
# Run QEMU with the installed kernel and generated initramfs
	qemu-system-x86_64 \
	  -serial mon:stdio \
	  -kernel build/vmlinuz \
	  -initrd build/initramfs.cpio.gz \
	  -machine accel=kvm:tcg \
	  -append "console=ttyS0 quiet rdinit=$(INIT)"

clean:
	rm -rf build minimal.s minimal.o minimal

.PHONY: initramfs run clean minimal

minimal: minimal.S
	gcc -g -S minimal.S > minimal.s
	as minimal.s -o minimal.o
	ld -o minimal minimal.o
