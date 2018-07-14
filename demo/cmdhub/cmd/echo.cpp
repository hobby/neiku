#include <cstdio>
#include "cmdhub.h"

static std::string message;

CMDHUB_CMD_TYPE cmd()
{
	printf("message: %s, version:[%s]\n", message.c_str(), cmdhub::getRef<std::string>().c_str());

	int32_t i = cmdhub::getVal<int32_t>("message");
	int64_t ii = cmdhub::getVal<int64_t>("message");
	uint32_t ui = cmdhub::getVal<uint32_t>("message");
	uint64_t uii = cmdhub::getVal<uint64_t>("message");
	std::string str = cmdhub::getVal<std::string>("message");
	printf("i:[%d], ii:[%lld], ui:[%u], uii:[%llu], str:[%s]\n", i, ii, ui, uii, str.c_str());
    return 0;
}

CMDHUB_INIT_TYPE init()
{
	cmdhub::addCmd({exec: cmd, desc: "echo command"}).
			addOpt({name: "message", desc: "message", optional: true});

    return 0;
}