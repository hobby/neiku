include ../../makefile.neiku

INCS = -I../../include/ \
	   $(shell neiku_config --incs log jsoncpp)

LIBS = -lneiku \
	   $(shell neiku_config --libs log jsoncpp)

VPATH = 

OUTPUT = json_test

ALL: $(OUTPUT)

json_test:json_test.o

clean:
	rm -f $(OUTPUT) *.o *.a *.so

