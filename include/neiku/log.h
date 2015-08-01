
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
 *         v6: 日志打印时间也是重要的现场信息
 *             gettimeofday可以获取精确到微秒的时间，localtime_r是线程安全的
 *             strftime可以得到精确到秒的日期时间串，结合snprintf可以得到精确到微秒，方便
 *             分析日志
 *         v7: 引入日志优先级，ERR级是最重要最需要关注的，MSG级属于正常流水（默认）
 *             DBG级常用于调试
 *         v8: 支持输出日志到文件，默认只输出到标准输出设备
 *         v9: 其实用户可以选择自己的单例组件，实现自己的类LOG/LOG_*宏
 * usage:
 *       #include <neiku/log.h>
 *
 * compile:
 *
 * vim:ts=4;sw=4;expandtab
 */

#ifndef __NK_LOG_H__
#define __NK_LOG_H__ 1

#include <unistd.h>
#include <sys/time.h>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <string>

#ifndef __NK_BASENAME
#define __NK_BASENAME(filepath) \
        (strrchr(filepath, '/') ? (strrchr(filepath, '/') + 1) : filepath)
#endif

// 默认情况下，提供neiku版日志单例
#ifndef __NO_NEIKU_SINGLETON_LOG__
#include <neiku/singleton.h> 
#define LOG SINGLETON(neiku::CLog)
#define LOG_ERR(format, args...) \
        LOG->DoLog(neiku::CLog::LOG_LEVEL_ERR, "[%s][pid:%d][%s:%d][%s] ERR: " \
                   format \
                   "\n" \
                   , LOG->GetTime(), getpid() \
                   , __NK_BASENAME(__FILE__), __LINE__, __FUNCTION__ \
                   , ##args);
#define LOG_MSG(format, args...) \
        LOG->DoLog(neiku::CLog::LOG_LEVEL_MSG, "[%s][pid:%d][%s:%d][%s] MSG: " \
                   format \
                   "\n" \
                   , LOG->GetTime(), getpid() \
                   , __NK_BASENAME(__FILE__), __LINE__, __FUNCTION__ \
                   , ##args);
#define LOG_DBG(format, args...) \
        LOG->DoLog(neiku::CLog::LOG_LEVEL_DBG, "[%s][pid:%d][%s:%d][%s] DBG: " \
                   format \
                   "\n" \
                   , LOG->GetTime(), getpid() \
                   , __NK_BASENAME(__FILE__), __LINE__, __FUNCTION__ \
                   , ##args);
#endif

namespace neiku
{

class CLog
{
    public:
        enum _LOG_LEVEL_ENUM
        {
            LOG_LEVEL_NONE = 0,
            LOG_LEVEL_ERR  = 1,
            LOG_LEVEL_MSG  = 2,
            LOG_LEVEL_DBG  = 3,
        };

    public:
        CLog(): m_iLogLevel(LOG_LEVEL_MSG)
              , m_bLog2Stdout(true)
              , m_bLog2File(false)
        {};

        // 设置/获取日志文件路径
        std::string SetLogFile(std::string sLogFilePath)
        {
            SetLog2Stdout(false);
            SetLog2File(true);

            std::string sPreLogFilePath = m_sLogFilePath;
            m_sLogFilePath = sLogFilePath;
            return sPreLogFilePath;
        };
        std::string GetLogFile()
        {
            return m_sLogFilePath;
        };

        // 输出日志到文件开关
        bool SetLog2File(bool bLog2File)
        {
            bool bPreLog2File = m_bLog2File;
            m_bLog2File = bLog2File;
            return bPreLog2File;
        };

        // 设置/获取日志优先级
        int SetLogLevel(int iLogLevel)
        {
            int iPreLogLevel = m_iLogLevel;
            m_iLogLevel = iLogLevel;
            return iPreLogLevel;
        };
        int GetLogLevel()
        {
            return m_iLogLevel;
        };
        int GetLogLevelErr()
        {
            return LOG_LEVEL_ERR;
        };
        int GetLogLevelMsg()
        {
            return LOG_LEVEL_MSG;
        };
        int GetLogLevelDbg()
        {
            return LOG_LEVEL_DBG;
        };

        // 输出日志到标准输出设备开关
        bool SetLog2Stdout(bool bLog2Stdout)
        {
            bool bPreLog2Stdout = m_bLog2Stdout;
            m_bLog2Stdout = bLog2Stdout;
            return bPreLog2Stdout;
        };

        // 根据日志优先级输出格式化日志
        int DoLog(int iLogLevel, const char* szFormat, ...)
        {
            // 当前配置的日志优先级过低，不输出日志
            if (m_iLogLevel < iLogLevel)
            {
                return 0;
            }

            // 输出日志到标准输出设备
            if (m_bLog2Stdout == true)
            {
                va_list vArgs;
                va_start(vArgs, szFormat);
                vprintf(szFormat, vArgs);
                va_end(vArgs);
            }

            // 输出日志到文件
            if (m_bLog2File == true)
            {
                FILE* pFile = fopen(m_sLogFilePath.c_str(), "a");
                if (pFile != NULL)
                {
                    va_list vArgs;
                    va_start(vArgs, szFormat);
                    vfprintf(pFile, szFormat, vArgs);
                    va_end(vArgs);
                    fclose(pFile);
                }
            }

            return 0;
        };

        // 获取当前时间串，格式 => yyyy-mm-dd hh:mm:ss.us
        const char* GetTime()
        {
            // 时间精确到微秒
            struct timeval stTv;
            gettimeofday(&stTv, NULL);
            // 线程安全为重
            struct tm stTm;
            localtime_r(&stTv.tv_sec, &stTm);
            // 不会返回脏数据
            size_t iLen = strftime(m_szTime, sizeof(m_szTime)
                                   , "%F %T", &stTm);
            snprintf(m_szTime+iLen, sizeof(m_szTime)-iLen
                     , ".%ld", stTv.tv_usec);
            return m_szTime;
        };

    private:
        char m_szTime[32];
        int  m_iLogLevel;
        bool m_bLog2Stdout;
        bool m_bLog2File;
        std::string m_sLogFilePath;
};

};

#endif
