include ../../makefile.neiku

INCS = $(shell neiku_config --incs jsoncpp)

LIBS = $(shell neiku_config --libs jsoncpp)

VPATH = 

OUTPUT = json_test

ALL: $(OUTPUT)

json_test:json_test.o

clean:
	rm -f $(OUTPUT) *.o *.a *.so

