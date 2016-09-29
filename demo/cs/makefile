
include $(shell neiku_config --conf makefile.neiku)

INCS = $(shell neiku_config --incs cs log)

LIBS = $(shell neiku_config --libs cs log)

SRCS = $(shell neiku_config --conf neiku.home)/src/neiku

OUTPUT = cs_test

ALL: $(OUTPUT)

cs_test: cs_test.o cs.o

clean:
	rm -f $(OUTPUT) *.o *.a *.so

