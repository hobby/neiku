

include ../../makefile.neiku

DIY_INC = -I../../include/ \

DIY_LIB = -lpthread

VPATH =

OUTPUT = singleton_test singleton_mt singleton_tsd

ALL: $(OUTPUT)

singleton_test:singleton_test.o singleton_module1.o singleton_module2.o
singleton_mt:singleton_mt.o
singleton_tsd:singleton_tsd.o

clean:
	rm -f $(OUTPUT) *.o *.a *.so

