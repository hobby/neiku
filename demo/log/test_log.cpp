
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
    return 0;
}
