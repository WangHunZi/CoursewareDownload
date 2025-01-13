# Requires statically linked busybox in $PATH

INIT := /init
IMG := build/disk.img
MOUNT := $(shell mktemp -d)
K := $(shell uname -r)

all: initramfs fsroot

initramfs:
# Copy kernel and busybox from the host system
	@mkdir -p build/initramfs/bin
	sudo bash -c "cp /boot/vmlinuz build/ && chmod 666 build/vmlinuz"
	cp init build/initramfs/
	cp $(shell which busybox) build/initramfs/bin/

# Pack build/initramfs as gzipped cpio archive
	cd build/initramfs && \
	  find . -print0 \
	  | cpio --null -ov --format=newc \
	  | gzip -9 > ../initramfs.cpio.gz

fsroot:
	mkdir -p fsroot/modules
	cp $$(find /lib/modules/$K/ -name e1000.ko) fsroot/modules/

	dd if=/dev/zero of=$(IMG) bs=1M count=64
	mkfs.ext4 -F $(IMG)
	sudo mount $(IMG) $(MOUNT)
	cd fsroot && sudo cp -r * $(MOUNT)
	sudo umount $(MOUNT)

run:
# Run QEMU with the installed kernel and generated initramfs
	qemu-system-x86_64 \
	  -serial mon:stdio -vga std \
	  -drive file=$(IMG),format=raw,index=0,media=disk \
	  -netdev user,id=net0,hostfwd=tcp:127.0.0.1:8080-:8080 -device e1000,netdev=net0 \
	  -kernel build/vmlinuz \
	  -initrd build/initramfs.cpio.gz \
	  -machine accel=kvm:tcg \
	  -append "console=ttyS0 quiet rdinit=$(INIT)"

clean:
	rm -rf build fsroot/modules

.PHONY: initramfs run clean fsroot
