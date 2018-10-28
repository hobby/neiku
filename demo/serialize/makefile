include $(shell neiku_config --conf makefile.neiku)

INCS = $(shell neiku_config --incs cgi log jsoncpp)

LIBS = $(shell neiku_config --libs cgi log jsoncpp)

OUTPUT += test_obj_serialize
OUTPUT += test_json_serialize

ALL: $(OUTPUT)

test_obj_serialize: test_obj_serialize.o
test_json_serialize: test_json_serialize.o

clean:
	rm -f $(OUTPUT) *.o *.a *.so

