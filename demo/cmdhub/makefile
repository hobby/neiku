include $(shell neiku_config --conf makefile.neiku)

INCS = -I./ $(shell neiku_config --incs log)
LIBS = $(shell neiku_config --libs log)
SRCS = 
OUTPUT = cmdhub_test

CMD_CPPS = $(wildcard ./cmd/*.cpp)
CMD_OBJS = $(CMD_CPPS:%.cpp=%.o)

ALL: $(OUTPUT)
cmdhub_test: cmdhub_test.o $(CMD_OBJS) cmdhub.h

run:
	@make clean
	@make
	@./cmdhub_test || echo

clean:
	rm -f $(OUTPUT) *.o *.a *.so $(CMD_OBJS)

