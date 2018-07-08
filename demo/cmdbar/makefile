include $(shell neiku_config --conf makefile.neiku)

INCS = -I./ $(shell neiku_config --incs log) -D__NK_CMDBAR_DEBUG_w_
LIBS = $(shell neiku_config --libs log)
SRCS = 
OUTPUT = cmdbar_test

CMD_CPPS = $(wildcard ./cmd/*.cpp)
CMD_OBJS = $(CMD_CPPS:%.cpp=%.o)

ALL: $(OUTPUT)
cmdbar_test: cmdbar_test.o $(CMD_OBJS) cmdbar.h

run:
	@make clean
	@make
	@./cmdbar_test || echo

clean:
	rm -f $(OUTPUT) *.o *.a *.so $(CMD_OBJS)

