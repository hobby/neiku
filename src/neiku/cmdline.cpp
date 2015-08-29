
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <libgen.h>
#include <map>
#include <vector>
#include <string>

#include "neiku/cmdline.h"

using namespace neiku;

int CCmdLine::Init(int iArgc, char** pArgv, const char* pszOptStr)
{
    if (iArgc <= 0 || pArgv == NULL)
    {
        snprintf(m_szErrMsg, sizeof(m_szErrMsg)
                 , "check argc/argv fail, argc:[%d], argv:[%p]"
                 , iArgc, pArgv);
        return ERR_PARSE_ARGC_ARGV;
    }
    
    // 获取程序名
    m_sProgName = basename(pArgv[0]);
    
    // 保存选项格式串
    m_pszOptStr = pszOptStr;

    // 解析选项格式串
    int iRet = ParseOptStr(m_pszOptStr);
    if (iRet != 0)
    {
        return iRet;
    }

    // 解析命令行参数
    iRet = ParseCmdLine(iArgc, pArgv);
    if(iRet != 0)
    {
        return iRet;
    }
    
    return 0;
}

std::string CCmdLine::GetCwd()
{
    char szCwdPath[FILENAME_MAX] = {0};
    if (getcwd(szCwdPath, sizeof(szCwdPath)) != NULL)
    {
        return szCwdPath;
    }
    return "";
}

const char* CCmdLine::GetOpt(const char* pszOptName, const char* pszDefault /* default NULL */)
{
    std::map<std::string, COption*>::iterator it = m_mOption.find(pszOptName);
    if (it != m_mOption.end() && it->second->m_pszValue != NULL)
    {
        // 选项存在，选项值也存在
        return it->second->m_pszValue;
    }

    // 选项不存在，或者选项值不存在
    return pszDefault;
}

bool CCmdLine::HasOpt(const char* pszOptName)
{
    return (GetOpt(pszOptName, NULL) != NULL);
}

int32_t CCmdLine::GetOptI(const char* pszOptName, int32_t iDefault)
{
    int iRet = iDefault;
    const char* pszValue = GetOpt(pszOptName, NULL);
    if (pszValue != NULL)
    {
        iRet = strtol(pszValue, NULL, 0);
    }
    return iRet;
}

int64_t CCmdLine::GetOptII(const char* pszOptName, int64_t lDefault)
{
    int64_t lRet = lDefault;
    const char* pszValue = GetOpt(pszOptName, NULL);
    if (pszValue != NULL)
    {
        lRet = strtoll(pszValue, NULL, 0);
    }
    return lRet;
}

uint32_t CCmdLine::GetOptUI(const char* pszOptName, uint32_t dwDefault)
{
    uint32_t dwRet = dwDefault;
    const char* pszValue = GetOpt(pszOptName, NULL);
    if (pszValue != NULL)
    {
        dwRet = strtoul(pszValue, NULL, 0);
    }
    return dwRet;
}

uint64_t CCmdLine::GetOptUII(const char* pszOptName, uint64_t ddwDefault)
{
    uint64_t ddwRet = ddwDefault;
    const char* pszValue = GetOpt(pszOptName, NULL);
    if (pszValue != NULL)
    {
        ddwRet = strtoull(pszValue, NULL, 0);
    }
    return ddwRet;
}

// FIXME: 同一个选项名不允许在不同组出现，这会导致选项值混乱！
int CCmdLine::ParseOptStr(const char* pszOptStr)
{
    COption oOption;
    COptionGroup oOptionGroup;

    int iErrIndex = 0;
    int iState = OPTSTR_PARSE_STATE_BEGIN;
    for (const char* pCurr = pszOptStr; *pCurr != '\0'; ++pCurr)
    {
        // 遇到选项组分隔符(,)或者选项分隔符(|)时，表示
        // 完成一个选项组或选项的解析
        if      (*pCurr == '|') { iState = OPTSTR_PARSE_STATE_SPLIT; }
        else if (*pCurr == ',') { iState = OPTSTR_PARSE_STATE_END;   }
        else
        {
            if(iState == OPTSTR_PARSE_STATE_BEGIN)
            {
                // 当前为[开始]状态时，遇到-符号继续，否则
                // 进入[名字]状态，[开始]状态确定短、长选项
                if (*pCurr == '-')
                {
                    oOption.m_sBegin.append(1, *pCurr);
                    if (oOption.m_sBegin.size() > 2)
                    {
                        iErrIndex = pCurr - pszOptStr + 1;
                        break;
                    }
                }
                else
                { 
                    // 当选项类型未确定时，无法进入[名字]状态
                    if (oOption.m_sBegin.empty())
                    {
                        iErrIndex = pCurr - pszOptStr + 1;
                        break;
                    }

                    // 进入[名字]状态
                    oOption.m_sName.append(std::string(1, *pCurr));
                    iState = OPTSTR_PARSE_STATE_NAME;
                }
            }
            else if (iState == OPTSTR_PARSE_STATE_NAME)
            {
                // 当前为[名字]状态时，遇到:则进入[标志]状态
                // [名字]状态确定短、长选项的选项名
                // 对于短选项，选项名只有1个字符，且限制为字母和数字
                // 对于长选项，选项名至少1个字符，且限制为字母和数字
                if (*pCurr == ':')
                {
                    // 进入[标志]状态
                    oOption.m_sFlag.append(1, *pCurr);
                    iState = OPTSTR_PARSE_STATE_FLAG;
                }
                else
                {
                    oOption.m_sName.append(1, *pCurr);
                    if (oOption.m_sBegin.size() < 2 && oOption.m_sName.size() > 1)
                    {
                        iErrIndex = pCurr - pszOptStr + 1;
                        break;
                    }
                }
            }
            else if (iState == OPTSTR_PARSE_STATE_FLAG)
            {
                // 当前为[标志]状态时，遇到:时继续，否则
                // 表示出错了(当前字符不是|或者,)
                // [标志]状态确认选项标志，定义如下：
                // ""    -> 选项无参数
                // ":"   -> 选项有参数，且存在时必需提供参数
                // "::"  -> 选项有参数，但不是必须的(默认为空串)
                if (*pCurr == ':')
                {
                    oOption.m_sFlag.append(1, *pCurr);
                    if (oOption.m_sFlag.size() > 2)
                    {
                        iErrIndex = pCurr - pszOptStr + 1;
                        break;
                    }
                }
                else
                {
                    // 非(:)、(|)、(,)三种符号在[标志]状态下是
                    // 非法的
                    iErrIndex = pCurr - pszOptStr + 1;
                    break;
                }
            }
            else if (iState == OPTSTR_PARSE_STATE_SPLIT)
            {
                // 在[选项分隔]状态下，表示已得到一个选项，此时需
                // 将当前选项加入到当前选项组中，只有选项类型和选项
                // 名字同时非空时，才表示是一个有效的选项
                if (!oOption.m_sBegin.empty() && !oOption.m_sName.empty())
                {
                    // FIXME: 同一选项仅允许在同一组中重复出现
                    oOptionGroup.m_vOption.push_back(oOption);
                    oOptionGroup.m_sFlag = oOption.m_sFlag;

                    oOption.m_sBegin = "";
                    oOption.m_sName  = "";
                    oOption.m_sFlag  = "";
                }

                // 当前字符是选项分隔符的下一个字符，当回到
                // [开始]状态时，需回滚当前字符，使其用于下一个
                // 选项的解析
                pCurr--;
                iState = OPTSTR_PARSE_STATE_BEGIN;
            }
            else if (iState == OPTSTR_PARSE_STATE_END)
            {
                // 在[选项组分隔]状态下，表示已得到同一组选项的最后
                // 一个选项，此时需将当前选项加入到当前选项组中，并
                // 将当前选项组加入到选项组列表中，最后清空当前选项
                // 组列表&选项组标志
                if (!oOption.m_sBegin.empty() && !oOption.m_sName.empty())
                {
                    oOptionGroup.m_vOption.push_back(oOption);
                    oOptionGroup.m_sFlag = oOption.m_sFlag;
                    m_vOptionGroup.push_back(oOptionGroup);
                    oOptionGroup.m_vOption.clear();
                    oOptionGroup.m_sFlag = "";

                    oOption.m_sBegin = "";
                    oOption.m_sName  = "";
                    oOption.m_sFlag  = "";
                }

                // 当前字符是选项分隔符的下一个字符，当回到
                // [开始]状态时，需回滚当前字符，使其用于下一个
                // 选项的解析
                pCurr--;
                iState = OPTSTR_PARSE_STATE_BEGIN;
            }
            else
            {
                // 非法状态，马上终止
                iErrIndex = pCurr - pszOptStr + 1;
                break;
            }
        }
    }
    if (iErrIndex > 0)
    {
        // 选项格式串存在问题，指出问题所在位置(字符)
        std::string sSpace(iErrIndex-1, ' ');
        snprintf(m_szErrMsg, sizeof(m_szErrMsg)
                , "parse optstr error, position:[\n%s\n%s^\n]\n"
                , pszOptStr, sSpace.c_str());
        return ERR_PARSE_OPTSTR;
    }

    // 若最后一个选项有效，则补充到选项组列表的最后一个选项组
    if (   iState == OPTSTR_PARSE_STATE_NAME
        || iState == OPTSTR_PARSE_STATE_FLAG
        || iState == OPTSTR_PARSE_STATE_SPLIT
        || iState == OPTSTR_PARSE_STATE_END)
    {
        oOptionGroup.m_vOption.push_back(oOption);
        oOptionGroup.m_sFlag = oOption.m_sFlag;
        m_vOptionGroup.push_back(oOptionGroup);
    }

    // 历遍所有选项组及其选项列表，生成
    // (1) 短、长选项列表
    // (2) 选项与参数值关系表(同一组选项的参数值相同)
    std::vector<COptionGroup>::iterator it = m_vOptionGroup.begin();
    for ( ; it != m_vOptionGroup.end(); ++it)

    {
        // 同组选项的参数值一样，在cmdline析构时释放资源
        COption *pOption = new COption;
        if (pOption == NULL)
        {
            snprintf(m_szErrMsg, sizeof(m_szErrMsg)
                    , "new coption fail, total-new:[%zu]"
                    , m_vOptionPtr.size());
            return ERR_NEW_COPTION;
        }
        m_vOptionPtr.push_back(pOption);

        std::vector<COption>::iterator itOpt = it->m_vOption.begin();
        for ( ; itOpt != it->m_vOption.end(); ++itOpt)
        {
            if (itOpt->m_sBegin.compare("-") == 0)
            {
                // 构造短选项
                m_sShortOptions.append(itOpt->m_sName).append(it->m_sFlag);

                // 创建选项与参数值关系
                std::string sOption = itOpt->m_sBegin + itOpt->m_sName;
                m_mOption[sOption] = pOption;
            }
            else if (itOpt->m_sBegin.compare("--") == 0)
            {
                // 构造长选项
                struct option stOpt;
                stOpt.name    = itOpt->m_sName.c_str();
                stOpt.has_arg = no_argument;
                stOpt.flag    = NULL;
                stOpt.val     = 0;
                if (itOpt->m_sFlag.compare(":") == 0)
                {
                    stOpt.has_arg = required_argument;
                }
                else if (itOpt->m_sFlag.compare("::") == 0)
                {
                    stOpt.has_arg = optional_argument;
                }
                // else 解析过程保证没有非空且非:和非::的标志
                m_vLongOptions.push_back(stOpt);

                // 创建选项与参数值关系
                std::string sOption = itOpt->m_sBegin + itOpt->m_sName;
                m_mOption[sOption] = pOption;
            }
            // else 解析过程保证没有非空且非:和非::的标志
        }
    }

    return 0;
}

int CCmdLine::ParseCmdLine(int iArgc, char** pArgv)
{
    // 存在短选项(不能为NULL, 首字符为:让无参数选项返回:符号)
    m_sShortOptions.insert(m_sShortOptions.begin(), ':');
    const char* pShortOptions = m_sShortOptions.c_str(); 

    // 存在长选项
    struct option* pLongOptions = NULL;
    if (!m_vLongOptions.empty())
    {
        // getopt_long() API 使用struct option[]传递长选项配置
        pLongOptions = new struct option[m_vLongOptions.size() + 1];
        if (pLongOptions == NULL)
        {
            snprintf(m_szErrMsg, sizeof(m_szErrMsg)
                    , "new struct option fail, size:[%zu], optstr:[%s]"
                    , m_vLongOptions.size() + 1, m_pszOptStr);
            return ERR_NEW_OPTION;
        }

        // 按api要求，最后一个option结构体各字段全为0，其他
        // 由按用户设置的选项格式串进行初始化
        int iIndex = 0;
        std::vector<struct option>::iterator itOpt = m_vLongOptions.begin();
        for ( ; itOpt != m_vLongOptions.end(); ++itOpt)
        {
            pLongOptions[iIndex].name    = itOpt->name;
            pLongOptions[iIndex].has_arg = itOpt->has_arg;
            pLongOptions[iIndex].flag    = itOpt->flag;
            pLongOptions[iIndex].val     = itOpt->val;
            iIndex++;
        }
        pLongOptions[iIndex].name    = 0;
        pLongOptions[iIndex].has_arg = 0;
        pLongOptions[iIndex].flag    = 0;
        pLongOptions[iIndex].val     = 0;
    }

    // 设置不用getopt/getopt_long API处理无效/无参数选项的处理
    // 当前实现会自行处理错误，上层可通过GetErrMsg()获取错误原因，这里
    // 需同时将m_sShortOptions首字符设置为 ':'
    opterr = 0;

    // 使用getopt_long(3) API解析命令行参数
    int iRet = 0;
    while (true)
    {
        int opt = 0, longindex = 0;
        std::string sShortKey("-");
        std::string sLongKey("--");
        std::map<std::string, COption*>::iterator it = m_mOption.end();

        // 短选项不能为NULL，否则会coredump
        opt = getopt_long(iArgc, pArgv
                , pShortOptions/*!NULL*/, pLongOptions, &longindex);
        if (opt == -1)
        {
            // 解析结束，退出
            break;
        }
        else if (opt == 0)
        {
            // 出现长选项，设置参数值
            sLongKey.append(pLongOptions[longindex].name);
            it = m_mOption.find(sLongKey);
            if (it != m_mOption.end())
            {
                // 可选选项存在，但又没有参数时，设置参数为空串
                it->second->m_pszValue = optarg ? optarg : "";
            }
            // 此映射表在内部构造，正常选项都该找到，此处忽略找不到的情况
        }
        else if (opt == '?')
        {
            // 出现无效选项
            iRet = ERR_OPT_INVALID;
            if (optopt == 0)
            {
                snprintf(m_szErrMsg, sizeof(m_szErrMsg)
                        , "invalid option, option:[%s], optstr:[%s]"
                        , pArgv[optind-1], m_pszOptStr);
            }
            else
            {
                snprintf(m_szErrMsg, sizeof(m_szErrMsg)
                        , "invalid option, option:[-%c], optstr:[%s]"
                        , optopt, m_pszOptStr);
            }
        }
        else if (opt == ':')
        {
            // 出现必要选项没有参数值
            iRet = ERR_OPT_REQUIRE_ARG;
            if (optopt == 0)
            {
                snprintf(m_szErrMsg, sizeof(m_szErrMsg)
                        , "option requires an argument, option:[%s], optstr:[%s]"
                        , pArgv[optind-1], m_pszOptStr);
            }
            else
            {
                snprintf(m_szErrMsg, sizeof(m_szErrMsg)
                        , "option requires an argument, option:[-%c], optstr:[%s]"
                        , optopt, m_pszOptStr);
            }
        }
        else
        {
            // 出现正常短选项，设置参数值
            sShortKey.append(1, opt);
            it = m_mOption.find(sShortKey);
            if (it != m_mOption.end())
            {
                // 可选选项存在，但又没有参数时，设置参数为空串
                it->second->m_pszValue = optarg ? optarg : "";
            }
            // 此映射表在内部构造，正常选项都该找到，此处忽略找不到的情况
        }

        // 出现出错马上终止
        if (iRet != 0)
        {
            break;
        }
    }

    // 成功时
    if (iRet == 0)
    {
        // 继续取出剩余非选项参数列表
        while (optind < iArgc)
        {
            m_vArgment.push_back(pArgv[optind++]);
        }

        // 保存成功信息
        snprintf(m_szErrMsg, sizeof(m_szErrMsg)
                 , "cmdline init succ, optstr:[%s], long-opt count:[%zu]"
                 , m_pszOptStr, m_vLongOptions.size());
    }

    // 释放资源
    if (pLongOptions != NULL)
    {
        delete []pLongOptions;
        pLongOptions = NULL;
    }

    return iRet;
}

/********************** 自动提取选项数据(序列化技术) beg ***********************/
CCmdLine& CCmdLine::operator & (CSerializeName& oName)
{
    m_sOptName = "--";
    m_sOptName.append(oName.GetName());
    return *this;
}

CCmdLine& CCmdLine::operator & (std::string& sValue)
{
    const char* pszValue = GetOpt(m_sOptName.c_str(), NULL);
    if (pszValue != NULL)
    {
        sValue.assign(pszValue);
    }
    return *this;
}

CCmdLine& CCmdLine::operator & (bool& bValue)
{
    if (HasOpt(m_sOptName.c_str()))
    {
        bValue = true;
    }
    else
    {
        bValue = false;
    }
    return *this;
}

CCmdLine& CCmdLine::operator & (int32_t& iValue)
{
    iValue = GetOptI(m_sOptName.c_str(), iValue);
    return *this;
}

CCmdLine& CCmdLine::operator & (int64_t& lValue)
{
    lValue = GetOptII(m_sOptName.c_str(), lValue);
    return *this;
}

CCmdLine& CCmdLine::operator & (uint32_t& dwValue)
{
    dwValue = GetOptUI(m_sOptName.c_str(), dwValue);
    return *this;
}

CCmdLine& CCmdLine::operator & (uint64_t& ddwValue)
{
    ddwValue = GetOptUII(m_sOptName.c_str(), ddwValue);
    return *this;
}
/********************** 自动提取选项数据(序列化技术) end ***********************/
