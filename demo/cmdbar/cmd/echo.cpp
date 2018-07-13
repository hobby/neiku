#include <cstdio>
#include "cmdbar.h"

static std::string message;

CMDBAR_CMD_TYPE cmd()
{
	printf("message: %s, version:[%s]\n", message.c_str(), cmdbar::getRef<std::string>().c_str());

	int32_t i = cmdbar::getVal<int32_t>("message");
	int64_t ii = cmdbar::getVal<int64_t>("message");
	uint32_t ui = cmdbar::getVal<uint32_t>("message");
	uint64_t uii = cmdbar::getVal<uint64_t>("message");
	std::string str = cmdbar::getVal<std::string>("message");
	printf("i:[%d], ii:[%lld], ui:[%u], uii:[%llu], str:[%s]\n", i, ii, ui, uii, str.c_str());
    return 0;
}

CMDBAR_INIT_TYPE init()
{
    cmdbar::registCmd("echo", cmd, "echo command");
    cmdbar::registOpt("echo", message);

    return 0;
}