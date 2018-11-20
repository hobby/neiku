
prefix=/usr/local/tinyxml2

all:
	@echo "usage: make <install>"
	@echo "# install tinyxml2 to ${prefix} with root"

install: tinyxml2
	if [ -d "${prefix}" ] ; then echo "${prefix} already exist, now exist"; exit 1; fi
	sudo mkdir -pv ${prefix}/{include,lib}
	sudo cp -v tinyxml2-5.0.0/tinyxml2.h ${prefix}/include/
	sudo cp -v tinyxml2-5.0.0/libtinyxml2.a ${prefix}/lib/
	sudo chmod -x ${prefix}/include/tinyxml2.h
	echo "----- install tinyxml2 done! -----"

tinyxml2:
	rm -vfr tinyxml2-5.0.0/
	unzip tinyxml2-5.0.0.zip
	cd tinyxml2-5.0.0/ && make
