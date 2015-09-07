

include ../../makefile.neiku

DIY_INC = -I ../../include/ \
		  -I ../../3rd/ \

DIY_LIB = -L ../../3rd/lib64/ -ljsoncpp \

VPATH = 

OUTPUT = test_json_serialize

ALL: $(OUTPUT)

test_json_serialize:test_json_serialize.o 

clean:
	rm -f $(OUTPUT) *.o *.a *.so

