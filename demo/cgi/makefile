
include $(shell neiku_config --conf makefile.neiku)

INCS = $(shell neiku_config --incs cgi log)

LIBS = $(shell neiku_config --libs cgi log)

SRCS = 

OUTPUT = cgi_test upload

ALL: $(OUTPUT)

cgi_test: cgi_test.o
upload: upload.o

clean:
	rm -f $(OUTPUT) *.o *.a *.so

