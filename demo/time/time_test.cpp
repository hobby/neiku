// vim:ts=4:sw=4:expandtab

#include "neiku/log.h"
#include "neiku/time.h"

using namespace neiku::util;

int main(int argc, char* argv[])
{
    LOG_MSG("current timems:[%lu], timeus:[%lu]"
            , timems(), timeus());

    return 0;
}
