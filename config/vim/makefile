
OUTPUT = vimrc.neiku

all:
	@echo usage: make install

install:
	ln -sf $$(pwd)/$(OUTPUT) $$HOME/.vimrc
	@echo -e "install $$(pwd)/$(OUTPUT) to $$HOME/.vimrc [ \e[32;1msucc\e[0m ]"

clean:
	rm -fv *.~
