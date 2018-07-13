#include <cstdio>
#include "cmdbar.h"

CMDBAR_CMD_TYPE cmd()
{
    int lhs = cmdbar::getVal<int>("lhs");
    int rhs = cmdbar::getVal<int>("rhs");
    printf("lhs:[%d], rhs:[%d], sum:[%d]\n", lhs, rhs, lhs+rhs);
    return 0;
}

CMDBAR_INIT_TYPE init()
{
    cmdbar::addCmd({
                exec: cmd,
                desc: "两个数相加，lhs是必填参数，rhs是可选参数"
            })
           .addOpt({
                name: "lhs",
                desc: "左因子"
            })
           .addOpt({
                name: "rhs",
                desc: "右因子",
                optional: true
            });
    return 0;
}