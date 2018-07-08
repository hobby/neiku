// vim:ts=4:sw=4:expandtab
#ifndef __NK_CMDBAR_H__
#define __NK_CMDBAR_H__

#include <cstddef>
#include <map>
#include <string>

// cmdbar调试
#ifdef __NK_CMDBAR_DEBUG__
    #include "neiku/log.h"
    #define DEBUG LOG_MSG
    #define ERROR LOG_ERR
#else
    #define DEBUG
    #define ERROR
#endif

// cmdbar辅助类型
#define CMDBAR_CMD_TYPE static int                               // 命令字入口类型（一个命令字源文件可包含多个命令字，建议保持1个）
#define CMDBAR_INIT_TYPE static __attribute__((constructor)) int // 命令字初始化类型（一个命令字源文件可以有多个初始化过程，建议保持1个）

class cmdbar
{
public:
    // 命令入口原型
    typedef int (*Routine)();

    // 启动命令行
    // usage:  <program> < [cmd] [opt...] | [opt...] >
    static int launch(int argc, char* argv[])
    {
        DEBUG("launch cmdbar, argc:[%d]", argc);

        if (argc < 2)
        {
            ERROR("argc < 2, argc:[%d]", argc);
            return 1;
        }

        std::string name = argv[1];
        std::map<std::string, Routine>::iterator cmd = me()->_CmdRoutineMap.find(name);
        if (cmd == me()->_CmdRoutineMap.end())
        {
            ERROR("cmd not found, name:[%s]", name.c_str());
            return 2;
        }

        Routine routine = cmd->second;
        return routine();
    }

    // 注册命令
    static int regist(const char* name, Routine routine)
    {
        if (name == NULL || routine == NULL)
        {
            ERROR("name or routine is nullptr, name:[%p], routine:[%p]"
                  , name, routine);
            return -1;
        }

        std::map<std::string, Routine>::iterator cmd = me()->_CmdRoutineMap.find(name);
        if (cmd != me()->_CmdRoutineMap.end())
        {
            ERROR("name duplicated, name:[%s]", name);
            return -2;
        }

        DEBUG("regist cmd, name:[%s], routine:[%p]", name, routine);
        me()->_CmdRoutineMap.insert(std::pair<std::string, Routine>(name, routine));
        return 0;
    }

private:
    static cmdbar* me()
    {
        static cmdbar bar;
        return &bar;
    }

private:
    std::map<std::string, Routine> _CmdRoutineMap;
};

#endif