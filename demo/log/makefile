include $(shell neiku_config --conf neiku.home)/makefile.neiku

INCS = $(shell neiku_config --incs log)

LIBS = $(shell neiku_config --libs log)

VPATH = 

OUTPUT = test_log

ALL: $(OUTPUT)

test_log:test_log.o

clean:
	rm -f $(OUTPUT) *.o *.a *.so

