// vim:ts=4:sw=4:expandtab

#include <unistd.h>
#include "neiku/log.h"
#include "neiku/cgx.h"

using namespace neiku;

int main(int argc, char* argv[])
{
    CGX->main(argc, argv);

    while (CGX->accept() >= 0)
    {
        CGX->setValue("key", CGX->getValue("Query.key", "default"));
        CGX->setValue("pid", getpid());
        int ret = CGX->render("fcgi_hello.cs");
        if (ret != 0)
        {
            LOG_MSG("render fail, red:[%d], msg:[%s]", ret, CGX->getErrMsg());
        }
    }

    return 0;
}
