include $(shell neiku_config --conf makefile.neiku)

INCS = $(shell neiku_config --incs cgi log)

LIBS = $(shell neiku_config --libs cgi log)

OUTPUT = test_obj_serialize

ALL: $(OUTPUT)

test_obj_serialize: test_obj_serialize.o

clean:
	rm -f $(OUTPUT) *.o *.a *.so

