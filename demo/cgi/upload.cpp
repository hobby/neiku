// vim:ts=4:sw=4:expandtab

#include "neiku/log.h"
#include "neiku/cgi.h"

using namespace neiku;

int main()
{
    CCgi cgi;

    FILE* fp = cgi.GetFile("file");
    cgi.SetValue("is_file_exist", fp ? "yes" : "no");

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

    cgi.Render("filehandle.cs");
    
    return 0;
}
