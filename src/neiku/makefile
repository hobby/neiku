include $(shell neiku_config --conf makefile.neiku)

INCS += $(shell neiku_config --incs charset cmdline clearsilver-cgi fcgi mysql jsoncpp tinyxml2)

LIBPATH = $(shell neiku_config --conf neiku.libpath)

OUTPUT = libneiku.a libcgi.a libdbx.a

ALL: $(OUTPUT)
 
libneiku.a: charset.o cmdline.o cs.o utility.o md5.o
	ar -r $@ $^
	mkdir -pv $(LIBPATH) && cp -vf $@ $(LIBPATH)

libcgi.a: cgx.o cgx.main.o cgx.file.o
	ar -r $@ $^
	mkdir -pv $(LIBPATH) && cp -vf $@ $(LIBPATH)

libdbx.a: dbx.o
	ar -r $@ $^
	mkdir -pv $(LIBPATH) && cp -vf $@ $(LIBPATH)


clean:
	rm -f $(OUTPUT) *.o *.a *.so

