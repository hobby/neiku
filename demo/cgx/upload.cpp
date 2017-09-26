// vim:ts=4:sw=4:expandtab

#include "neiku/log.h"
#include "neiku/cgx.h"

using namespace neiku;

int main()
{
    CgX cgi;

    FILE* fp = cgi.getFile("file");
    cgi.setValue("is_file_exist", fp ? "yes" : "no");

    int nRead = 0;
    char szBuf[1024] = {0};
    if(fp)
    {
        nRead = fread(szBuf, 1, 1024, fp);
    }

    const char* szRoot = "/tmp/file.upload";
    if(szRoot)
    {
        FILE* fpTmp = fopen(szRoot, "wb");
        if(fpTmp)
        {
            fwrite(szBuf, 1, nRead, fpTmp);
        }
    }

    cgi.render("filehandle.cs");
    
    return 0;
}
