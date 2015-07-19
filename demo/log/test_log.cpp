
#include <unistd.h>
#include <stdint.h>
#include <cstdio>
#include <string>
#include <cstring>

#include <neiku/log.h>

using namespace std;

void testv4()
{
    int pid = getpid();
    LOG("log meta with pid:[%d], file:[%s], line:[%d], function:[%s]"
        , pid, __FILE__, __LINE__, __FUNCTION__);
}

int main(int argc, char* argv[])
{
    LOG("simple log here");
    LOG("this is second line");

    int version = 3;
    const char *name = "neiku::log v3";
    LOG("log v*printf/variadic macro, version:[%d], name:[%s]"
        , version, name);

    testv4();

    const char* prtnull = NULL;
    LOG("basename('/usr/'):[%s], basename(NULL):[%s]"
        , basename("/usr/")
        , basename(""));
    return 0;
}
