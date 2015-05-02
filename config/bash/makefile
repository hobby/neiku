
bashrc = `pwd`/bashrc.neiku

all:
	@echo "Usage: make [install | clean]"

clean:
	rm -fv *~

install:
	@echo -e "\e[32;1minstall ${bashrc}\e[0m"
	echo -e "#" >> ~/.bashrc
	echo -e "# neiku config" >> ~/.bashrc
	echo -e "[ -f ${bashrc} ] && source ${bashrc}" >> ~/.bashrc
	echo -e "#" >> ~/.bashrc
	@echo -e "\e[32;1minstall ${bashrc} ---> succ\e[0m"
