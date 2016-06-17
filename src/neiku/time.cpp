
#include <unistd.h>
#include <sys/time.h>

#include "neiku/time.h"

uint64_t neiku::util::timems()
{
    timeval tv;
    int ret = gettimeofday(&tv, NULL);
    if (ret != 0)
    {
        return -1;
    }

    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

uint64_t neiku::util::timeus()
{
    timeval tv;
    int ret = gettimeofday(&tv, NULL);
    if (ret != 0)
    {
        return -1;
    }

    return (tv.tv_sec * 1000000 + tv.tv_usec);
}
