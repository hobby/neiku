
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <neiku/log.h>
#include <string>

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

    if(argc != 4)
    {
        LOG_ERR("usage: %s filename maxsize maxnum", argv[0]);
        return -1;
    }

    const char* filename = argv[1];
    int64_t maxsize = strtoull(argv[2], NULL, 0);
    uint32_t maxnum = strtoul(argv[3], NULL, 0);
    LOG->SetLogFile(filename, maxsize, maxnum);
    LOG->SetLog2Stdout(true);
    LOG_MSG("set log file, auto create dir, path:[%s]"
            ", maxsize:[%"PRId64"], maxnum:[%u]"
            , filename
            , maxsize, maxnum);

    // 编译期检查参数列表类型
    std::string msg("this is string");
    uint64_t ret(0);
    LOG_MSG("ret:[%"PRIu64"], msg:[%s]", ret, msg.c_str());

    // 测试外部日志开关
    while (true)
    {
        LOG_MSG("test outer log switch");
        sleep(1);
    }

    return 0;
}
