// vim:ts=4:sw=4:expandtab

#include "neiku/log.h"
#include "neiku/cs.h"

using namespace neiku;

int main()
{
    int ret = 0;

    CCS cs;
    cs.Init();

    cs.SetValue("user.qq", "8281845");
    cs.SetValue("user.name", "YIF");
    cs.SetValue("user.desc", "line1\nline2\r\n");

    std::string sResult;
    ret = cs.Render("./template.cs", sResult);
    if(ret == 0)
    {
        LOG_MSG("render from file => [\n%s]", sResult.c_str());
    }

    std::string sDump;
    cs.Dump(sDump);
    LOG_MSG("dump => [\n%s]", sDump.c_str());



    return 0;
}
