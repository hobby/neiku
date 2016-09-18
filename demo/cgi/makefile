
include $(shell neiku_config --conf makefile.neiku)

INCS = $(shell neiku_config --incs cgi log)

LIBS = $(shell neiku_config --libs cgi log)

SRCS = $(shell neiku_config --conf neiku.home)/src/neiku/

OUTPUT = cgi_test upload fcgi_hello

ALL: $(OUTPUT)

cgi_test: cgi_test.o cgi.o
upload: upload.o cgi.o
fcgi_hello: fcgi_hello.o cgi.o

clean:
	rm -f $(OUTPUT) *.o *.a *.so

