
#include <inttypes.h>
#include <neiku/log.h>

void test_func()
{
#ifndef __NO_NEIKU_SINGLETON_LOG__
    LOG_MSG("not define __NO_NEIKU_SINGLETON_LOG__");
#else
    neiku::CLog oLog;
    oLog.DoLog(oLog.GetLogLevelMsg()
               , "%s\n", "defined __NO_NEIKU_SINGLETON_LOG__");
#endif
}

int main(int argc, char* argv[])
{
    test_func();

    if(argc != 2)
    {
        LOG_ERR("usage: %s logfilepath", argv[0]);
        return -1;
    }

    const char* logfile = argv[1];
    LOG->SetLogFile(logfile);
    LOG->SetLog2Stdout(true);
    LOG_MSG("set log file, auto create dir, path:[%s]"
            , logfile);

    // 编译期检查参数列表类型
    std::string msg("this is string");
    uint64_t ret(0);
    LOG_MSG("ret:[%lu], msg:[%s]", ret, msg.c_str());

    return 0;
}
