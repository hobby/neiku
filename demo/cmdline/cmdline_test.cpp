
#include <stdio.h>
#include <string.h>
#include <neiku/cmdline.h>

#define OPTIONSTR \
"-u|--username:\
,-p|--password:\
,-v|--verbose::\
,-h|--help::\
,-i|--int32:\
,-l|--int64:\
,-u|--uint32:\
,-q|--uint64:"

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
        printf("%s : cmdline init fail, ret:[%d], msg:[%s]\n"
                , CMDLINE->GetProgName().c_str(), iRet, CMDLINE->GetErrMsg());
        return -1;
    }

    printf("username:[%s], password:[%s], verbose:[%d], help:[%d]"
            ", int32:[%d], int64:[%ld], uint32:[%u], uint64:[%lu]"
            ", int32:[%d], int64:[%ld], uint32:[%u], uint64:[%lu]\n"
            , options.username.c_str(), options.password.c_str()
            , options.verbose, options.help
            , options.int32, options.int64, options.uint32, options.uint64
            , CMDLINE->GetOptI("--int32"), CMDLINE->GetOptII("--int64")
            , CMDLINE->GetOptUI("--uint32"), CMDLINE->GetOptUII("--uint64"));

    // 输出三类选项参数值
    printf("username:[%s]\n",  CMDLINE->GetOpt("--username", "not set username"));
    printf("password:[%s]\n",  CMDLINE->GetOpt("--password", "not set password"));
    printf("verbose:[%s]\n",   CMDLINE->GetOpt("--verbose") ? "set verbose" : "not set verbose");
    printf("help:[%s]\n",      CMDLINE->GetOpt("-h") ? "set help" : "not set help");

    // 输出非选项参数值
    printf("remainng args:\n");
    std::vector<const char*>* pArgList = CMDLINE->GetArgList();
    std::vector<const char*>::iterator it = pArgList->begin();
    for ( ; it != pArgList->end(); ++it)
    {
        printf("arg:[%s]\n", *it);
    }
    
    // 输出当前工作目录(绝对路径，无/符号结束)
    printf("current wroking directory:[%s]\n", CMDLINE->GetCwd().c_str());
    
    // 输出程序名
    printf("current program name:[%s]\n", CMDLINE->GetProgName().c_str());

    // 判断是否存在某参数
    if (CMDLINE->HasOpt("--verbose"))
    {
        printf("--verbose is exist\n");
    }
    else
    {
        printf("--verbose is not exit\n");
    }

    return 0;
}
