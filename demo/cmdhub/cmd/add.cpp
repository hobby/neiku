#include <cstdio>
#include "cmdhub.h"

CMDHUB_CMD_TYPE cmd()
{
    int lhs = cmdhub::getVal<int>("lhs");
    int rhs = cmdhub::getVal<int>("rhs");
    printf("lhs:[%d], rhs:[%d], sum:[%d]\n", lhs, rhs, lhs+rhs);
    return 0;
}

CMDHUB_INIT_TYPE init()
{
    cmdhub::addCmd({
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