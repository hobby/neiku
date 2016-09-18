// vim:ts=4:sw=4:expandtab

#include <unistd.h>
#include "neiku/log.h"
#include "neiku/cgi.h"

using namespace neiku;

int main()
{
    CCgi cgi;
    while (cgi.Accept())
    {
        cgi.SetValue("pid", getpid());
        int ret = cgi.Render("fcgi_hello.cs");
        if (ret != 0)
        {
            LOG_MSG("render fail, red:[%d], msg:[%s]", ret, cgi.GetErrMsg());
        }
    }

    return 0;
}
