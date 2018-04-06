
#include "neiku/log.h"
#include "cmdprog.h"

int subCmd()
{
    LOG_MSG("sub command called");
    return 0;
}

__attribute__((constructor)) static void init()
{
	struct CCmd cmd = {
		name: "sub",
		routine: subCmd
	};

	PROG->add(cmd);
}
