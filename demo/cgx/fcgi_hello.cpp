// vim:ts=4:sw=4:expandtab

#include <unistd.h>
#include "neiku/log.h"
#include "neiku/cgx.h"

using namespace neiku;

int main()
{
    CCgx *CGX = new CCgx();
    while (CGX->Accept())
    {
        CGX->SetValue("pid", getpid());
        int ret = CGX->Render("fcgi_hello.cs");
        if (ret != 0)
        {
            LOG_MSG("render fail, red:[%d], msg:[%s]", ret, CGX->GetErrMsg());
        }
    }

    return 0;
}
