// vim:ts=4:sw=4:expandtab

#include <unistd.h>     // NULL
#include <time.h>       // localtime_r strftime strptime mktime
#include <sys/time.h>   // gettimeofday

#include "neiku/time.h"

time_t neiku::util::timems()
{
    timeval tv;
    int ret = gettimeofday(&tv, NULL);
    if (ret != 0)
    {
        return -1;
    }

    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

time_t neiku::util::timeus()
{
    timeval tv;
    int ret = gettimeofday(&tv, NULL);
    if (ret != 0)
    {
        return -1;
    }

    return (tv.tv_sec * 1000000 + tv.tv_usec);
}

std::string neiku::util::timestr(time_t sec, const char* fmt)
{
    struct tm tms;
    localtime_r(&sec, &tms);

    // always keep \0 inside
    char buf[33] = {0};
    strftime(buf, sizeof(buf), fmt, &tms);
    return std::string(buf);
}

time_t neiku::util::timestamp(const char* str, const char* fmt)
{
    struct tm tms;
    strptime(str, fmt, &tms);
    return mktime(&tms);
}