// vim:ts=4:sw=4:expandtab

#include "neiku/log.h"
#include "neiku/cgx.h"

using namespace neiku;

int main()
{
	LOG->SetLogFile("/Users/YIF/apache/logs/cgx_getbody");

    CGX->setParseHttpBodyManually(true);

    std::string body = CGX->getHttpBody();
    LOG_MSG("http body:[%s]", body.c_str());

    CGX->setValue("body", body);
    CGX->setValue("ip", CGX->getRemoteAddr());
    CGX->render("/Users/YIF/apache/docs/cgx_getbody.cs");

    return 0;
}
