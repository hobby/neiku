#include <cstdio>
#include "cmdbar.h"

static int lhs;
static int rhs;

CMDBAR_CMD_TYPE cmd()
{
	printf("lhs:[%d], rhs:[%d], sum:[%d]\n", lhs, rhs, lhs+rhs);
    return 0;
}

CMDBAR_INIT_TYPE init()
{
    cmdbar::registCmd("add", cmd, "两个数相加，lhs是必填参数，rhs是可选参数");
    cmdbar::registOpt("add", lhs, "左因子", "0", true);
    cmdbar::registOpt("add", rhs, "右因子", "0");

    return 0;
}
