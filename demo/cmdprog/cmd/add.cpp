
#include "neiku/log.h"
#include "cmdprog.h"

int addCmd()
{
    LOG_MSG("add command called");
    return 0;
}

__attribute__((constructor)) static void init()
{
	struct CCmd cmd = {
		name: "add",
		routine: addCmd
	};

	PROG->add(cmd);
}
