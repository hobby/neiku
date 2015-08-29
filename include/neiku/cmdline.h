
/*
 * file:   yaf/cmdline.h
 * author: yifee
 * date:   2014/09/21 15:46:00
 * desc:
 * CCmdLine v1.1
 * (1) 基于序列化思想，实现命令行参数到内存数据对象的自动转换。
 *     让命令行解析及参数获取全部自动化完成，推荐使用！
 * usage:
 * -----------------cmdline_test.cpp--------------------
 * #include <yaf/cmdline.h>
 * class COptions
 * {
 *      public:
 *          std::string username;
 *          std::string password;
 *           COptions(): username("default username")
 *                     , password("default password")
 *          {};
 *          template <typename AR>
 *          AR& Serialize(AR& ar)
 *          {
 *              SERIALIZE(ar, username);
 *              SERIALIZE(ar, password);
 *              return ar;
 *          };
 * }
 * int main(int argc, char** argv)
 * {
 *     // 初始化命令行(解析器)
 *     COptions options;
 *     iRet = CMDLINE->Init(argc, argv, "-u|--username:,-p|--password:", options);
 *     
 *     // 获取命令行选项
 *     printf("username:[%s]\n", options.username.c_str();
 *     printf("password:[%s]\n", options.password.c_str());
 *
 *     return 0;
 * }
 * ----------------------------------------------------
 * CCmdLine v1.0
 * (1) 基于getopt(3) API 实现，支持短选项、长选项、非选项参数
 * (2) 支持开关选项、必要选项、可选选项，定义分别如下:
 *     -- 开关选项()，只要选项出现，则表示用户打开该选项，选项值非NULL
 *     -- 必要选项(:)，只要选项出现，则要求用户输入1个选项值，选项值默认为空串，不存在时为NULL
 *     -- 可选选项(::)，选项出现时，用户可选地输入选项值(短选项时，选项值紧近选项名；长选项时，选项值与选项名使用=链接)
 * (3) 选项不存在时，选项值为NULL，否则为const char*。当然，当选项不存在时可以指定返回值(!NULL)
 * (4) 同一组选项的选项值只保留一份，当同组选项出现多次时，选项值取最后一个选项的选项值
 * (5) 非选项参数获取可通过CCmdLine::GetArgList():std:;vector<const char*>*得到
 * (6) 提供GetCwd()及GetProgName()接口，方便上层获取当前工作目录及当前程序名.
 *
 * remark:
 * (1) 长选项建议使用=符号分隔选项名与参数值，例如: --project="YAF proj"
 * (2) 短选项建议选项名与参数值紧密连接，例如: -p"YAF proj"
 * 
 * usage:
 * -----------------cmdline_test.cpp--------------------
 * #include <yaf/cmdline.h>
 * int main(int argc, char** argv)
 * {
 *     // 初始化命令行(解析器)
 *     iRet = CMDLINE->Init(argc, argv, "-u|--username:,-p|--password::,--verbose,--help|-h");
 *     if (iRet != 0)
 *     {
 *         // error occur when parse cmdline
 *         // now exist.
 *     }
 *     
 *     // 获取命令行选项
 *     printf("username:[%s]\n", CMDLINE->GetOpt("--username", "default name"));
 *     printf("password:[%s]\n", CMDLINE->GetOpt("--password", "default password"));
 *     printf("verbose:[%s]\n",  CMDLINE->GetOpt("--verbose") ? "true" : "false");
 *     printf("help:[%s]\n",     CMDLINE->GetOpt("-h") ? "true" : "false");
 *
 *     return 0;
 * }
 * ----------------------------------------------------
 * $ g++ -o cmdline_test cmdline_test -I$(YAF_PROJ_PATH)/include -L$(YAF_PROJ_PATH)/lib -lyaf_util
 * $ ./cmdline_test --username="yifee" --verbose
 * usrname:[yifee]
 * password:[default password]
 * verbose:[1]
 * help:[0]
 * $
 */

#ifndef __NK_CMDLINE_H_
#define __NK_CMDLINE_H_

#include <stdint.h>
#include <getopt.h>
#include <map>
#include <vector>
#include <string>

#include "neiku/serialize.h"
#include "neiku/singleton.h"
#define CMDLINE SINGLETON(neiku::CCmdLine)

namespace neiku
{

class CCmdLine
{
    public:
        enum _CmdLineErrCode
        {
            ERR_PARSE_OPTSTR     = -1,   ///< 解析选项格式串失败
            ERR_PARSE_ARGC_ARGV  = -2,   ///< argc/argv非法
            ERR_NEW_OPTION       = -3,   ///< 申请struct option数组失败
            ERR_NEW_COPTION      = -4,   ///< 申请COption失败
            ERR_OPT_INVALID      = -5,   ///< 无效选项
            ERR_OPT_REQUIRE_ARG  = -6,   ///< 选项没有参数(必要选项才有)
        };

    public:
        // 初始化
        // iArgc     -> main函数的argc
        // pArgv     -> main函数的argv
        // pszOptStr -> 选项格式串，例 => "-u|--username:,-p::,--verbose"
        //              -u/-username 为(参数值)必要选项，且为同一组选项
        //              -p 为可选选项
        //              --verbose 为开关选项
        int Init(int iArgc, char** pArgv, const char* pszOptStr);
        
        // 支持自定义选项结构体自动序列化(自动提取选项参数值)，推荐使用
        template <typename option_t>
        int Init(int iArgc, char** pArgv, const char* pszOptStr, option_t& oOption)
        {
            int iRet = Init(iArgc, pArgv, pszOptStr);
            if (iRet == 0)
            {
                *this >> oOption;
            }
            return iRet;
        };

        // 获取选项参数
        // pszOptName --> 选项名，如"-u"、"--username"，同组选项的参数值一样
        // pszDefault --> 当选项不存在时，返回该参数做为默认参数
        //                一般开关参数返回NULL表示关闭选项、非NULL表示开启选项
        const char* GetOpt(const char* pszOptName, const char* pszDefault = NULL);

        // 判断是否存在参数
        bool HasOpt(const char* pszOptName);

        // 获取整数参数值
        // (1) 数值型参数支持8、10、16进制数
        int32_t  GetOptI  (const char* pszOptName, int32_t  iDefault   = 0);
        int64_t  GetOptII (const char* pszOptName, int64_t  lDefault   = 0);
        uint32_t GetOptUI (const char* pszOptName, uint32_t dwDefault  = 0);
        uint64_t GetOptUII(const char* pszOptName, uint64_t ddwDefault = 0);

        // TODO(1):
        // int SetCmdName(const char*)   ///< 设置程序执行时的命令名称，即企图改变argv[0]返回的值

        // 获取(剩余)非选项参数
        inline std::vector<const char*>* GetArgList()
        {
            return &m_vArgment;
        }

        // 获取程序名
        // 如/path/to/program程序，则程序名为: program
        // basename/argv[0]得到
        inline std::string GetProgName()
        {
            return m_sProgName;
        }
        
        // 获取最后出错描述
        inline const char* GetErrMsg()
        {
            return m_szErrMsg;
        }
        
        // 获取当前工作目录（绝对路径，无/符号结束，基于getcwd()实现）
        std::string GetCwd();

        /********************** 自动提取选项数据(序列化技术) ***********************/
    public:
        CCmdLine& operator & (CSerializeName& oName);
        CCmdLine& operator & (std::string& sValue);
        CCmdLine& operator & (bool& bValue);
        CCmdLine& operator & (int32_t& iValue);
        CCmdLine& operator & (int64_t& lValue);
        CCmdLine& operator & (uint32_t& dwValue);
        CCmdLine& operator & (uint64_t& ddwValue);

        // 暂不支持vector参数
        template <class object_t>
        CCmdLine& operator & (std::vector<object_t>& vObj)
        {
            return *this;
        }
        template <class object_t>
        CCmdLine& operator & (const std::vector<object_t>& vObj)
        {
            return *this;
        }

        template <typename object_t>
        CCmdLine& operator & (object_t& obj)
        {
            obj.Serialize(*this);
            return *this;
        };
        template <typename object_t>
        CCmdLine& operator >> (object_t& obj)
        {
            (*this) & obj;
            return *this;
        };

    private:
        std::string m_sOptName;         ///< 当前序列化的选项名称
        /*******************************************************************/
      
    public:
        CCmdLine(): m_pszOptStr(NULL)
        {
            snprintf(m_szErrMsg, sizeof(m_szErrMsg)
                     , "%s", "cmdline not init");
        };

        ~CCmdLine()
        {
            std::vector<COption*>::iterator it = m_vOptionPtr.begin();
            for (; it != m_vOptionPtr.end(); ++it)
            {
                if (*it != NULL)
                {
                    delete *it;
                    *it = NULL;
                }
            }
            m_vOptionPtr.clear();
        };
       
    private:
        // 解析选项格式串
        // (1) 得到短、长选项列表
        // (2) 初始化选项与参数值关系表(同一组选项的参数值相同)
        int ParseOptStr(const char* pszOptStr);

        // 解析命令行参数
        // (1) 得到短、长选项的参数值
        int ParseCmdLine(int iArgc, char** pArgv);

    private:
        enum _OptStrParseState
        {
            OPTSTR_PARSE_STATE_BEGIN = 1,
            OPTSTR_PARSE_STATE_NAME  = 2,
            OPTSTR_PARSE_STATE_FLAG  = 3,
            OPTSTR_PARSE_STATE_SPLIT = 4,
            OPTSTR_PARSE_STATE_END   = 5,
        };

        // 单个选项
        class COption
        {
            public:
                const char* m_pszValue;             ///< 选项值(argv[n])
                std::string m_sBegin;               ///< 选项前缀，如(短选项)单个-、(长选项)两个--
                std::string m_sName;                ///< 选项名，如u、username
                std::string m_sFlag;                ///< 选项标志，如(必要选项)单个:、(可选选项)两个:、(开关选项)为空

            public:
                COption(): m_pszValue(NULL)
                {};
        };

        // 选项组，同一组选项的标志统一
        class COptionGroup
        {
            public:
                std::vector<COption> m_vOption;
                std::string          m_sFlag;       ///< 以最后一个选项的标志为准
        };

    private:
        std::string m_sProgName;                    ///< 程序名称
        char m_szErrMsg[256];                       ///< 最后错误信息

    // 命令行解析
    private:
        std::map<std::string, COption*> m_mOption;  //< 选项参数关系表
        std::vector<const char*> m_vArgment;        ///< 非选项参数列表
        std::vector<COption*> m_vOptionPtr;         ///< 选项参数值(COption)，析构时自动释放资源

    // 选项串解析
    private:
        const char* m_pszOptStr;                    ///< 当前选项格式串
        std::vector<COptionGroup> m_vOptionGroup;   ///< 当前选项组列表
        std::string m_sShortOptions;                ///< 短选项，给getopt_long()使用
        std::vector<struct option> m_vLongOptions;  ///< 长选项，在构造struct option[]后给getopt_long()使用
};

};

#endif
