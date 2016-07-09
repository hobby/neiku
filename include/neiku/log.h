
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
 *        v10: 支持自动创建日志文件路径中缺少的子目录(类似mkdir -p)
 *        v11: 优先级做为日志元信息，不混合在日志中，方便日志分析
 *             日志输出设备包含标准输出设备、文件，只输出就行了，不用返回什么
 *        v12: 提供独立的GetFileName来从__FILE__中获取文件名
 *        v13: 编译期使用-Wall或者-Wformat，结合__attribute__设置类似printf的参数检查
 *             可以提前检测到日志输出是否有问题（core、未知行为）
 *             需要注意的是，成员函数的参数列表其实隐藏了一个this指针（在首位）
 *             https://gcc.gnu.org/onlinedocs/gcc-4.4.7/gcc/Function-Attributes.html
 *             (3.x.y~5.x.y版本目测都可用)
 *        v14: 自动创建日志路径子目录之前，检查一下（一般只需要创建一次）
 *        v15: 支持外部文件级日志开关
 *             日志文件由日志名和.log后缀组成，外部关闭日志文件由日志名和.nolog后缀组成
 *             (关闭日志文件存在时，即不输出日志到文件，但不影响输出日志到标准输出设备)
 *        v16: 支持基于字节大小&个数的日志文件滚动，对于文件名为filename，大小为N、个数为M的滚动配置。
 *             当M<=1时，日志只在filename.log文件中滚动
 *             当M>1时，最新日志总在filename.log中，越旧的日志按index=[1,M-1]存储，文件名为
 *             filename{index}.log，index越大，意味着日志越旧(方便扩充)
 *             注意：文件大小默认限制20MB，个数限制20个
 *        v17: 支持区分suseconds_t的实际打印格式串，osx平台是int，linux是long int
 *             osx平台使用stat而不是stat64
 * usage:
 *       #include <neiku/log.h>
 *
 * compile:
 *
 * vim:ts=4;sw=4;expandtab
 */

#ifndef __NK_LOG_H__
#define __NK_LOG_H__ 1

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <cerrno>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <string>

// 默认情况下，提供neiku版日志单例
#ifndef __NO_NEIKU_SINGLETON_LOG__
#include <neiku/singleton.h> 

#define LOG SINGLETON(neiku::CLog)

#define LOG_ERR(format, args...) \
        LOG->DoLog(  neiku::CLog::LOG_LEVEL_ERR \
                   , "[%s][pid:%d][%s:%d][%s][ERR]: "format"\n" \
                   , LOG->GetTimeStr(), getpid() \
                   , LOG->GetFileName(__FILE__), __LINE__, __FUNCTION__ \
                   , ##args);
#define LOG_MSG(format, args...) \
        LOG->DoLog(  neiku::CLog::LOG_LEVEL_MSG \
                   , "[%s][pid:%d][%s:%d][%s][MSG]: "format"\n" \
                   , LOG->GetTimeStr(), getpid() \
                   , LOG->GetFileName(__FILE__), __LINE__, __FUNCTION__ \
                   , ##args);
#define LOG_DBG(format, args...) \
        LOG->DoLog(  neiku::CLog::LOG_LEVEL_DBG \
                   , "[%s][pid:%d][%s:%d][%s][DBG]: "format"\n" \
                   , LOG->GetTimeStr(), getpid() \
                   , LOG->GetFileName(__FILE__), __LINE__, __FUNCTION__ \
                   , ##args);
#endif

namespace neiku
{

// stat64在osx里面使用stat替代了
#if defined (__APPLE__) || defined (__MACH__)
    #define STAT64 stat
#else
    #define STAT64 stat64
#endif

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
              , m_llLogFileMaxSize(20*1024*1024)
              , m_dwLogFileMaxNum(20)
        {};

        // 设置日志文件路径
        void SetLogFile(const std::string& sLogFileName
                        , int64_t llLogFileMaxSize = 20*1024*1024
                        , uint32_t dwLogFileMaxNum = 20)
        {
            m_sLogFileName.assign(sLogFileName);
            m_llLogFileMaxSize = llLogFileMaxSize;
            m_dwLogFileMaxNum  = dwLogFileMaxNum;

            // 设置输出日志到文件
            SetLog2File(true);
            m_sLogFilePath.assign(m_sLogFileName);
            m_sLogFilePath.append(".log");

            // 关闭日志输出文件
            m_sNoLogFilePath.assign(m_sLogFileName);
            m_sNoLogFilePath.append(".nolog");

            // 可能缺少中间目录，自动创建之
            struct STAT64 st64;
            if (STAT64(m_sLogFilePath.c_str(), &st64) != 0)
            {
                MakeSubDir(m_sLogFilePath.c_str());
            }

            // 默认不再输出到标准输出
            SetLog2Stdout(false);
        };

        // 设置日志文件滚动
        void SetLogFile(int64_t llLogFileMaxSize, uint32_t dwLogFileMaxNum)
        {
            m_llLogFileMaxSize = llLogFileMaxSize;
            m_dwLogFileMaxNum  = dwLogFileMaxNum;
        };

        // 输出日志到文件开关
        void SetLog2File(bool bLog2File)
        {
            m_bLog2File = bLog2File;
        };

        // 设置日志优先级
        void SetLogLevel(int iLogLevel)
        {
            m_iLogLevel = iLogLevel;
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
        void SetLog2Stdout(bool bLog2Stdout)
        {
            m_bLog2Stdout = bLog2Stdout;
        };

        // 根据日志优先级输出格式化日志，支持检查参数与format是否相符
        void DoLog(int iLogLevel, const char* szFormat, ...)
                         __attribute__((format(printf,3,4)))
        {
            // 当前配置的日志优先级过低，不输出日志
            if (m_iLogLevel < iLogLevel)
            {
                return;
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
                // 外部没有关闭日志，继续输出
                struct STAT64 st64;
                if (STAT64(m_sNoLogFilePath.c_str(), &st64) != 0)
                {
                    FILE* pFile = fopen(m_sLogFilePath.c_str(), "a");
                    if (pFile != NULL)
                    {
                        va_list vArgs;
                        va_start(vArgs, szFormat);
                        vfprintf(pFile, szFormat, vArgs);
                        va_end(vArgs);
                        fclose(pFile);

                        // 日志文件变大，尝试滚动
                        LogFileRotate();
                    }
                }
            }
        };

        // 获取当前时间串，格式 => yyyy-mm-dd hh:mm:ss.us
        const char* GetTimeStr()
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
            #if defined (__gnu_linux__)
                // stTv.tv_usec is long int on Centos(linux 2.6.x with x86_64 gcc 4.4.7)
                snprintf(m_szTime+iLen, sizeof(m_szTime)-iLen
                         , ".%ld", stTv.tv_usec);
            #elif defined (__APPLE__) || defined (__MACH__)
                // stTv.tv_usec is int on Darwin(osx 10.11.5 with x86_64 gcc 4.8.5)
                snprintf(m_szTime+iLen, sizeof(m_szTime)-iLen
                         , ".%d", stTv.tv_usec);
            #else
                #warning "is this apple or linux ? why not define __gnu_linux__/__APPLE__/__MACH__"
                #warning "now i think it is linux (default)"
                snprintf(m_szTime+iLen, sizeof(m_szTime)-iLen
                         , ".%ld", stTv.tv_usec);
            #endif
            return m_szTime;
        };

        // 创建文件路径中的子目录(效果类似mkdir -p)
        int MakeSubDir(const char* szPath)
        {
            if (szPath == NULL)
            {
                return -1;
            }

            std::string sPath = szPath;
            size_t iLen = sPath.size();
            for (size_t i = 1; i < iLen; ++i)
            {
                if (sPath[i] == '/')
                {
                    sPath[i] = '\0';
                    int iRet = mkdir(sPath.c_str(), 0777);
                    if (iRet != 0 && errno != EEXIST)
                    {
                        return -1;
                    }
                    sPath[i] = '/';
                }
            }

            return 0;
        };

        // 获取路径中的文件名(取代gnu/posix的basename)
        const char* GetFileName(const char* szPath)
        {
            if(szPath[0] == '.' || szPath[0] == '/')
            {
                const char* szFileName = strrchr(szPath, '/');
                return szFileName ? szFileName + 1 : szPath;
            }
            else
            {
                return szPath;
            }
        };

        // 日志文件滚动
        void LogFileRotate()
        {
            // 对于出错情况，也要尝试滚动
            struct STAT64 st64;
            int iRet = STAT64(m_sLogFilePath.c_str(), &st64);
            if (iRet == 0 && st64.st_size < m_llLogFileMaxSize)
            {
                // 还没有到滚动的时候
                return;
            }

            // 先转存当前日志文件，避免多进程同时滚动，造成混乱
            char szTmpPath[FILENAME_MAX] = {0};
            snprintf(szTmpPath, sizeof(szTmpPath), "%s.tmp.log"
                     , m_sLogFileName.c_str());
            iRet = rename(m_sLogFilePath.c_str(), szTmpPath);
            if (iRet != 0)
            {
                // 可能已有进程在滚动了
                return;
            }

            // 将{filename}[1~M-2].log文件滚到{filename}[2~M-1].log
            for (int i = m_dwLogFileMaxNum - 2; i > 0; --i)
            {
                char szOldPath[FILENAME_MAX] = {0};
                char szNewPath[FILENAME_MAX] = {0};
                snprintf(szOldPath, sizeof(szOldPath), "%s%d.log"
                         , m_sLogFileName.c_str(), i);
                snprintf(szNewPath, sizeof(szNewPath), "%s%d.log"
                         , m_sLogFileName.c_str(), i+1);
                rename(szOldPath, szNewPath);
            }

            // 将{filename}.tmp.log文件滚到{filename}1.log
            char szLog1Path[FILENAME_MAX] = {0};
            snprintf(szLog1Path, sizeof(szLog1Path), "%s%d.log"
                     , m_sLogFileName.c_str(), 1);
            rename(szTmpPath, szLog1Path);
        };

    private:
        char m_szTime[32];
        int  m_iLogLevel;
        bool m_bLog2Stdout;
        bool m_bLog2File;
        std::string m_sLogFileName;
        std::string m_sLogFilePath;   // filename + .log
        std::string m_sNoLogFilePath; // filename + .nolog
        int64_t  m_llLogFileMaxSize;  // in bytes
        uint32_t m_dwLogFileMaxNum;
};

};

#endif
