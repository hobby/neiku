
output = dssh dgo \
         dlog \
         dssh1 dssh2 \

prefix = ${HOME}

link = no

all:
	@echo "usage: make [ install|uninstall [ prefix=/path/to/dox link=yes|no ]"
	@echo ""
	@echo "example: make                               # show this usage"
	@echo ""
	@echo "         make install                       # install dox to ~/bin/ using cp"
	@echo "         make install link=yes              # install dox to ~/bin/ using ln"
	@echo "         make install prefix=/usr/          # install dox to /usr/bin/ using cp"
	@echo "         make install prefix=/usr/ link=yes # install dox to /usr/bin/ using ln"
	@echo ""
	@echo "         make uninstall                     # uninstall dox from ~/bin/ using rm"
	@echo "         make uninstall prefix=/usr/        # uninstall dox from /usr/bin/ using rm"
	@echo ""

clean:

install:
	@echo -e "\e[32;1minstall ${output} to '${prefix}' with link=${link}\e[0m"
	mkdir -pv ${prefix}/bin/
	for file in ${output} ; do \
		if [ "${link}" = "yes" ] ; then \
			ln -svf `pwd`/$$file ${prefix}/bin/; \
		else \
			cp -vf `pwd`/$$file ${prefix}/bin/; \
		fi \
	done
	@echo -e "\e[32;1minstall ${output} ---> end\e[0m"

uninstall:
	@echo -e "\e[32;1muninstall ${output} from '${prefix}'\e[0m"
	for file in ${output} ; do \
		\rm -vf ${prefix}/bin/$$file; \
	done
	@echo -e "\e[32;1muninstall ${output} ---> end\e[0m"
