
#include "neiku/log.h"
#include "cmdprog.h"

int helloCmd()
{
    LOG_MSG("hello command called");
    return 0;
}

__attribute__((constructor)) static void init()
{
	struct CCmd cmd = {
		name: "hello",
		routine: helloCmd
	};

	PROG->add(cmd);
}