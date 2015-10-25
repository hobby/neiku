
git_completion_bash = `pwd`/git-completion.bash
git_prompt_sh = `pwd`/git-prompt.sh

all:
	@echo "Usage: make [install | clean]"

clean:
	rm -vf *~

install:
	@if which git >/dev/null 2>&1; \
	then \
		./git.config.sh && echo -e "git config [ \e[32;1msucc\e[0m ]";\
	else \
		echo -e "git config [ \e[31;1mfail\e[0m ]"; \
	fi
	@echo -e "\e[32;1minstall ${git_completion_bash}\e[0m"
	@echo -e "\e[32;1minstall ${git_prompt_sh}\e[0m"
	echo -e "#" >> ~/.bashrc
	echo -e "# git tools" >> ~/.bashrc
	echo -e "[ -f ${git_completion_bash} ] && source ${git_completion_bash}" >> ~/.bashrc
	echo -e "[ -f ${git_prompt_sh} ] && source ${git_prompt_sh}" >> ~/.bashrc
	echo -e "#" >> ~/.bashrc
	@echo -e "\e[32;1minstall ${git_completion_bash} ---> succ\e[0m"
	@echo -e "\e[32;1minstall ${git_prompt_sh} ---> succ\e[0m"
