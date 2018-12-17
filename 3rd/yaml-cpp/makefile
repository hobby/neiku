prefix=/opt/yaml-cpp

all:
	@echo "usage: make <install>"
	@echo "# install yaml-cpp to ${prefix} with root"

install: yaml-cpp
	echo "----- install yaml-cpp done! -----"

yaml-cpp:
	rm -vfr yaml-cpp-release-0.3.0/
	tar -zxvf yaml-cpp-release-0.3.0.tar.gz
	cd yaml-cpp-release-0.3.0/ && mkdir build && cd build && cmake -D CMAKE_INSTALL_PREFIX=${prefix} ..
	cd yaml-cpp-release-0.3.0/build && make
	cd yaml-cpp-release-0.3.0/build && make install
