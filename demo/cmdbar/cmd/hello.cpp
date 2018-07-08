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
    LOG_MSG("regist hello cmd, address:[%p]", hello);
    cmdbar::regist("hello", hello /*参数列表，拉位置映射*/);
    cmdbar::regist("cmd1", cmd1);
    cmdbar::regist("cmd1", cmd1);
    cmdbar::regist(NULL, cmd1);
    cmdbar::regist("cmd2", NULL);
    return 0;
}