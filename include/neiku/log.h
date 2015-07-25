
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
 *         v4: 进程id/文件名&号行/函数名是日志的现场信息，很有用
 *             注意，__FILE__还包含路径信息
 *             https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html
 *             FIXME: 不能强制使用者对于basename版本的选择，所以还是收敛下吧
 *                    posix版basename遇到/结层的路径时(如/usr/)，会挂掉，so还是用gnu的吧
 *                    (当然，gnu版basename遇到NULL时也会挂掉)
 *         v5: 收敛basename（简单实现），去掉gnu/posix版basename的依赖
 *             注意，__FILE__中的文件名部分总是非空的
 * usage:
 *       #include <neiku/log.h>
 *
 * compile: -lneiku
 *
 * vim:ts=4;sw=4;expandtab
 */

#ifndef __NK_LOG_H__
#define __NK_LOG_H__ 1

#include <unistd.h>
#include <cstring>
#include <cstdarg>
#include <cstdio>

#ifndef __NK_BASENAME
#define __NK_BASENAME(filepath) \
        (strrchr(filepath, '/') ? (strrchr(filepath, '/') + 1) : filepath)
#endif

#include <neiku/singleton.h> 
#define LOGER SINGLETON(neiku::CLog)
#define LOG(format, args...) \
        LOGER->DoLog("[pid:%d][%s:%d][%s]" \
                     format \
                     "\n" \
                     , getpid(), __NK_BASENAME(__FILE__), __LINE__, __FUNCTION__ \
                     , ##args)

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
