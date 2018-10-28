
prefix=/usr/local/clearsilver

all:
	@echo "usage: make <install>"
	@echo "# install clearsilver to ${prefix} with root"

install: clearsilver
	if [ -d "${prefix}" ] ; then echo "${prefix} already exist, now exist"; exit 1; fi
	cd clearsilver-0.10.5/ && sudo make install
	rm -vfr clearsilver-0.10.5/
	echo "----- install clearsilver done! -----"

clearsilver:
	rm -vfr clearsilver-0.10.5/
	tar -zxvf clearsilver-0.10.5-post-content-type-bugfix.tar.gz
	cd clearsilver-0.10.5/ && ./configure --prefix=${prefix} --disable-wdb --disable-apache --disable-python --disable-perl --disable-java --disable-csharp --disable-ruby
	cd clearsilver-0.10.5/ && sed -i 's/test_crc.cs//g' cs/Makefile
	cd clearsilver-0.10.5/ && make
