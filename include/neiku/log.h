
/*
 * file:   neiku/log.h
 * desc:   log tools for C++
 * author: hobby
 * date:   2015/07/18 23:03:00
 * version: 
 *         v1: 最简单莫过于只支持输出日志到终端
 *         v2: 其实，每条日志就是一行信息，便于分析
 * usage:
 *       #include <neiku/log.h>
 *
 * compile: -lneiku
 *
 * vim:ts=4;sw=4;expandtab
 */

#ifndef __NK_LOG_H__
#define __NK_LOG_H__ 1

#include <stdio.h>

#include <neiku/singleton.h>
#define LOGER SINGLETON(neiku::CLog)
#define LOG(log)   LOGER->DoLog(log)

namespace neiku
{

class CLog
{
    public:
        CLog()
        {};

        int DoLog(const char* szLog)
        {
            printf("%s\n", szLog);
            return 0;
        };
};

};

#endif
