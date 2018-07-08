#include <cstdio>
#include "cmdbar.h"

static std::string message;

CMDBAR_CMD_TYPE cmd()
{
	printf("message: %s\n", message.c_str());
    return 0;
}

CMDBAR_INIT_TYPE init()
{
    cmdbar::registCmd("echo", cmd, "echo command");
    cmdbar::registOpt("echo", message);

    return 0;
}