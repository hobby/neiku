
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

    const char* logfile = "/tmp/logs/log/test_log.log";
    LOG->SetLogFile(logfile);
    LOG->SetLog2Stdout(true);
    LOG_MSG("set log file, auto create dir, path:[%s]"
            , logfile);

    return 0;
}
