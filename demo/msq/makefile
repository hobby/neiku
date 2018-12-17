include ../../makefile.neiku

INCS += $(shell neiku_config --incs neiku)
LIBS += $(shell neiku_config --libs neiku)

OUTPUT = msq_test

ALL: $(OUTPUT)

msq_test: msq_test.o

clean:
	rm -f $(OUTPUT) *.o *.a *.so

