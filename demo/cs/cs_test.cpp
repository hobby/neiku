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

    std::string HDF;
    //HDF.append("hdf=   value for hdf!\n");
    HDF.append("hdf.0 << EOM\nvalue for hdf.0! \nand this is anther line\nEOM\n");
    HDF.append("hdf.1 = this is value for hdf.1\n");
    HDF.append("hdf.2=   value for hdf.2!\n");
    
    cs.SetValueHDF(HDF.c_str());

    std::string sResult;
    ret = cs.Render("./template.cs", sResult);
    if(ret == 0)
    {
        LOG_MSG("render from file => [\n%s]", sResult.c_str());
    }

    std::string sDump;
    cs.Dump(sDump);
    LOG_MSG("dump => [\n%s]", sDump.c_str());


    LOG_MSG("hdf:[%s]", cs.GetValue("hdf", "unknow"));
    LOG_MSG("hdf.0:[%s]", cs.GetValue("hdf.0", "unknow"));

    return 0;
}
