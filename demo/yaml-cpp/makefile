include $(shell neiku_config --conf makefile.neiku)

INCS = -I ../../include $(shell neiku_config --incs cgi log jsoncpp yaml-cpp)

LIBS = $(shell neiku_config --libs cgi log jsoncpp yaml-cpp)

OUTPUT += test_yamp-cpp

ALL: $(OUTPUT)

test_yamp-cpp: test_yamp-cpp.o

clean:
	rm -f $(OUTPUT) *.o *.a *.so

