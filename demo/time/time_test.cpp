// vim:ts=4:sw=4:expandtab

#include "neiku/log.h"
#include "neiku/time.h"

#include <iostream>

using namespace std;
using namespace neiku::util;

int main(int argc, char* argv[])
{
    // 毫秒/微秒时间戳
    time_t t = timeus();
    LOG_MSG("current timems:[%lu], timeus:[%lu], sizeof(time_t):[%lu]"
            , timems(), t, sizeof(time_t));

    // 时间戳转时间串
    t = time(NULL);
    LOG_MSG("timestamp:[%lu], timestr:[%s]"
            , t
            , timestr(t, "%Y-%m-%d %H:%M:%S").c_str());

    time_t src = time(NULL);
    std::string str = timestr(src, "%Y-%m-%d %H:%M:%S");
    LOG_MSG("src:[%lu], src timestr:[%s]"
            , src, str.c_str());

    // 时间串转时间戳
    time_t dst = timestamp(str.c_str(), "%Y-%m-%d %H:%M:%S");
    str = timestr(dst, "%Y-%m-%d %H:%M:%S");
    LOG_MSG("dst:[%lu], dst timestr:[%s]"
            , dst, str.c_str());    
    return 0;
}
