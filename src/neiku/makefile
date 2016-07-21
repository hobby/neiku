
include $(shell neiku_config --conf makefile.neiku)

INCS = $(shell neiku_config --incs charset cmdline)

LIBPATH = $(shell neiku_config --conf neiku.libpath)

OUTPUT = libneiku.a

ALL: $(OUTPUT)

libneiku.a: charset.o cmdline.o
	ar -r $@ $^
	mkdir -pv $(LIBPATH) && cp -vf $@ $(LIBPATH)

clean:
	rm -f $(OUTPUT) *.o *.a *.so

