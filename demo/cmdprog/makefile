include $(shell neiku_config --conf makefile.neiku)

INCS = -I./ $(shell neiku_config --incs log)
LIBS = $(shell neiku_config --libs log)
SRCS = 
OUTPUT = cmdprog_test

CMD_CPPS = $(wildcard ./cmd/*.cpp)
CMD_OBJS = $(CMD_CPPS:%.cpp=%.o)

ALL: $(OUTPUT)
cmdprog_test: cmdprog_test.o $(CMD_OBJS)

clean:
	rm -f $(OUTPUT) *.o *.a *.so $(CMD_OBJS)

