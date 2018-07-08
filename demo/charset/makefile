

include $(shell neiku_config --conf neiku.home)/makefile.neiku

DIY_INC = $(shell neiku_config --incs charset)

DIY_LIB = -lneiku

VPATH =

OUTPUT = charset_test

ALL: $(OUTPUT)

charset_test: charset_test.o

clean:
	rm -f $(OUTPUT) *.o *.a *.so

