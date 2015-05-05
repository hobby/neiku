

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
