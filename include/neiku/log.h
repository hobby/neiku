
/*
 * file:   neiku/log.h
 * desc:   log tools for C++
 * author: hobby
 * date:   2015/07/18 23:03:00
 * version: 
 *         v1: 最简单莫过于只支持输出日志到终端
 *         v2: 其实，每条日志就是一行信息，便于分析
 *         v3: 基于va_list/variadic macro，日志也可以格式化打印
 *             注意，##在这里的作用是允许参数列表为空
 *            （了解#/##/__VA_ARGS__)
 *             http://www.cplusplus.com/reference/cstdarg/
 *             http://www.cplusplus.com/reference/cstdio/vprintf/
 *             https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
 * usage:
 *       #include <neiku/log.h>
 *
 * compile: -lneiku
 *
 * vim:ts=4;sw=4;expandtab
 */

#ifndef __NK_LOG_H__
#define __NK_LOG_H__ 1

#include <cstdarg>
#include <cstdio>

#include <neiku/singleton.h>
#define LOGER SINGLETON(neiku::CLog)
#define LOG(format, args...)   LOGER->DoLog(format"\n", ##args)

namespace neiku
{

class CLog
{
    public:
        CLog()
        {};

        int DoLog(const char* szFormat, ...)
        {
            va_list vArgs;
            va_start(vArgs, szFormat);
            vprintf(szFormat, vArgs);
            va_end(vArgs);
            return 0;
        };
};

};

#endif
