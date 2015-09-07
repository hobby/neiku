

include ../../makefile.neiku

DIY_INC = 

DIY_LIB = 

VPATH =

OUTPUT = libjsoncpp.a

LIBJSONCPP_A_OBJS = jsoncpp.o

ALL: $(OUTPUT)

libjsoncpp.so: $(LIBJSONCPP_A_OBJS)
	g++ -fPIC -shared -o $@ $^ $(ALL_LIB)
	mkdir -pv $(3RD_LIB_PATH) && cp -vf $@ $(3RD_LIB_PATH)

libjsoncpp.a: $(LIBJSONCPP_A_OBJS)
	ar -r $@ $^
	mkdir -pv $(3RD_LIB_PATH) && cp -vf $@ $(3RD_LIB_PATH)

clean:
	rm -f $(OUTPUT) *.o *.a *.so

