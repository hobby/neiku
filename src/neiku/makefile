
include $(shell neiku_config --conf makefile.neiku clearsilver-cgi fcgi)

INCS = $(shell neiku_config --incs charset cmdline clearsilver-cgi fcgi)

LIBPATH = $(shell neiku_config --conf neiku.libpath)

OUTPUT = libneiku.a libcgi.a

ALL: $(OUTPUT)

libneiku.a: charset.o cmdline.o cs.o
	ar -r $@ $^
	mkdir -pv $(LIBPATH) && cp -vf $@ $(LIBPATH)

libcgi.a: cgx.o cgx_main.o
	ar -r $@ $^
	mkdir -pv $(LIBPATH) && cp -vf $@ $(LIBPATH)

clean:
	rm -f $(OUTPUT) *.o *.a *.so

