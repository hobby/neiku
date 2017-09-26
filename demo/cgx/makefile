
include $(shell neiku_config --conf makefile.neiku)

INCS = $(shell neiku_config --incs cgi log)

LIBS = $(shell neiku_config --libs cgi log)

OUTPUT = cgi_test upload fcgi_hello cgx_getbody

ALL: $(OUTPUT)

cgi_test: cgi_test.o
upload: upload.o
fcgi_hello: fcgi_hello.o
cgx_getbody: cgx_getbody.o

clean:
	rm -f $(OUTPUT) *.o *.a *.so

