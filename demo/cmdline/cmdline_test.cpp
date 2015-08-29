
#include <stdio.h>
#include <string.h>

#include "neiku/log.h"
#include "neiku/cmdline.h"

#define OPTIONSTR "-u|--username:" \
                  ",-p|--password:"\
                  ",-v|--verbose::"\
                  ",-h|--help::"\
                  ",-i|--int32:"\
                  ",-l|--int64:"\
                  ",-u|--uint32:"\
                  ",-q|--uint64:"

class COptions
{
    public:
        std::string  username;
        std::string  password;
        bool         verbose;
        bool         help;
        int32_t      int32;
        int64_t      int64;
        uint32_t     uint32;
        uint64_t     uint64;

    public:
        COptions(): username("default name")
                  , password("default pass")
                  , verbose(false)
                  , help(false)
                  , int32(0)
                  , int64(0)
                  , uint32(0)
                  , uint64(0)
        {};

        template <typename AR>
        AR& Serialize(AR& ar)
        {
            SERIALIZE(ar, username);
            SERIALIZE(ar, password);
            SERIALIZE(ar, verbose);
            SERIALIZE(ar, help);
            SERIALIZE(ar, int32);
            SERIALIZE(ar, int64);
            SERIALIZE(ar, uint32);
            SERIALIZE(ar, uint64);

            return ar;
        };
};


int main(int argc, char **argv)
{
    // 初始化命令行参数解析器
    COptions options;
    int iRet = CMDLINE->Init(argc, argv, OPTIONSTR, options);
    if (iRet != 0)
    {
        LOG_MSG("%s : cmdline init fail, ret:[%d], msg:[%s]"
                , CMDLINE->GetProgName().c_str(), iRet, CMDLINE->GetErrMsg());
        return -1;
    }
    LOG_MSG("options => %s", OBJDUMP(options));
    
    // 输出三类选项参数值
    LOG_MSG("username:[%s]",  CMDLINE->GetOpt("--username", "默认用户名"));
    LOG_MSG("password:[%s]",  CMDLINE->GetOpt("--password", "默认密码"));
    LOG_MSG("verbose:[%s]",   CMDLINE->GetOpt("--verbose") ? "set verbose" : "not set verbose");
    LOG_MSG("help:[%s]",      CMDLINE->GetOpt("-h") ? "set help" : "not set help");

    // 输出非选项参数值
    LOG_MSG("remainng args:");
    std::vector<const char*>* pArgList = CMDLINE->GetArgList();
    std::vector<const char*>::iterator it = pArgList->begin();
    for ( ; it != pArgList->end(); ++it)
    {
        LOG_MSG("arg:[%s]", *it);
    }
    
    // 输出当前工作目录(绝对路径，无/符号结束)
    LOG_MSG("current wroking directory:[%s]", CMDLINE->GetCwd().c_str());
    
    // 输出程序名
    LOG_MSG("current program name:[%s]", CMDLINE->GetProgName().c_str());

    // 判断是否存在某参数
    if (CMDLINE->HasOpt("--verbose"))
    {
        LOG_MSG("--verbose is exist");
    }
    else
    {
        LOG_MSG("--verbose is not exit");
    }

    LOG_MSG("OBJDUMP(options):[%s]", OBJDUMP(options));
    return 0;
}
