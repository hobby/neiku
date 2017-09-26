// vim:ts=4:sw=4:expandtab

#include "neiku/log.h"
#include "neiku/cgx.h"

using namespace neiku;

int main()
{
    // std::string output;
    //int iRet = cgi.Dump(output);
    //LOG_MSG("ret:[%d], output:[%s]", iRet, output.c_str());

    ///LOG_MSG("Query.name:[%s]", cgi.GetValue("Query.name", NULL));

    //cgi.SetValue("name", "YIF");
    //LOG_MSG("name:[%s]", cgi.GetValue("name"));

    //int ret = cgi.Render("template.cs");
    //LOG_MSG("ret:[%d], msg:[%s]", ret, cgi.GetErrMsg());

    //cgi.Redirect302Url("%s", "/");

    //cgi.Redirect302Uri("%s", "/");

    //cgi.SetValue("age", 0x111111111111);
    //cgi.SetValue("age", 18);
    //cgi.Render("template.cs");

    // LOG_MSG("i:[%d], ii:[%lld], ui:[%u], uii:[%llu]"
    //        , cgi.GetValueI("Query.i")
    //        , cgi.GetValueII("Query.ii")
    //        , cgi.GetValueUI("Query.ui")
    //        , cgi.GetValueUII("Query.uii"));

    CGX->setValue("key", CGX->getValue("Query.key", ""));
    CGX->setValue("pid", getpid());
    CGX->render("cgi_test.cs");
    
    return 0;
}
