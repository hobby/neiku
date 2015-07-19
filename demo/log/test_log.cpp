
#include <stdint.h>
#include <cstdio>
#include <string>

#include <neiku/log.h>

using namespace std;

int main(int argc, char* argv[])
{
    LOG("simple log here");
    LOG("this is second line");

    int version = 3;
    const char *name = "neiku::log v3";
    LOG("log v*printf/variadic macro, version:[%d], name:[%s]"
        , version);

    return 0;
}
