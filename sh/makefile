
OUTPUT = rpmbuild.bb

all: $(OUTPUT)
	@echo "you can run 'make install' to install $(OUTPUT)"

install:
	mkdir -pv ~/bin/
	@for file in $(OUTPUT) ; do ln -sfv `pwd`/$$file ~/bin/ ; done
	@echo install '$(OUTPUT)' successfully.

uninstall:
	@for file in $(OUTPUT) ; do rm -fv ~/bin/$$file; done
	@echo uninstall '$(OUTPUT)' successfully.

clean:
	@echo nothing clean.

