#include "neiku/log.h"
#include "cmdhub.h"

CMDHUB_CMD_TYPE hello()
{
	// 自己取参数
	// 系统自动映射
    LOG_MSG("hello command was called");
    return 0;
}

CMDHUB_CMD_TYPE cmd1()
{
	LOG_MSG("cmd1 command was called");
	return 0;
}

CMDHUB_INIT_TYPE init()
{
    // cmdhub::registCmd("hello", hello, "cmdhub hello command");

    // cmdhub::registCmd("cmd1", cmd1, "cmd1 in hello.cpp");

    return 0;
}