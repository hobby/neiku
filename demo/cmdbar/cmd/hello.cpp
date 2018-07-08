#include "neiku/log.h"
#include "cmdbar.h"

CMDBAR_CMD_TYPE hello()
{
	// 自己取参数
	// 系统自动映射
    LOG_MSG("hello command was called");
    return 0;
}

CMDBAR_CMD_TYPE cmd1()
{
	LOG_MSG("cmd1 command was called");
	return 0;
}

CMDBAR_INIT_TYPE init()
{
    cmdbar::registCmd("hello", hello, "cmdbar hello command");

    cmdbar::registCmd("cmd1", cmd1, "cmd1 in hello.cpp");

    return 0;
}