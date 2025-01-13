MINIX := minix-1.7
# MINIX := minix-2.0

run: minix2
	cd minix2 \
	  && ln -s -f images/$(MINIX)-hd-*.img hd.img \
	  && ./runqemu

minix2:
	git clone --depth 1 --single-branch -b master \
	  https://github.com/davidgiven/minix2
	cd minix2 \
	  && ./mkall \
	  && gunzip images/*.gz

.PHONEY: minix2
