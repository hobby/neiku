// vim:ts=4:sw=4:expandtab

/*
 * file:   neiku/sqlbind.h
 * desc:   sql bind工具，方便业务拼装安全可用的sql
 * author: YIF
 * date:   2017-04-12
 */

#ifndef _NEIKU_SQLBIND_H_
#define _NEIKU_SQLBIND_H_

#include <stdint.h>
#ifndef __STDC_FORMAT_MACROS
    #define __STDC_FORMAT_MACROS
#endif
#include <cstdio>
#include <cstring>
#include <inttypes.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <time.h>

#include "neiku/serialize.h"

namespace neiku
{

class SqlBind
{
public:
    SqlBind(const std::string & sSqlPattern = ""):sPattern(sSqlPattern){}
    SqlBind(const char * szSqlPattern):sPattern(szSqlPattern){}

public:
    void setValue(uint32_t dwIndex,int32_t iValue)
    {
        mpInt.insert(std::map<uint32_t,int32_t>::value_type(dwIndex,iValue));
    }

    void setValue(uint32_t dwIndex,uint32_t dwValue)
    {
        mpUint.insert(std::map<uint32_t,uint32_t>::value_type(dwIndex,dwValue));
    }
    
    void setValue(uint32_t dwIndex,int64_t llValue)
    {
        char szValue[32] = {0};
        snprintf(szValue,sizeof(szValue),"%"PRId64,llValue);
        mpStr.insert(std::map<uint32_t,std::string>::value_type(dwIndex,szValue));
    }
    
    void setValue(uint32_t dwIndex,uint64_t ullValue)
    {
        char szValue[32] = {0};
        snprintf(szValue,sizeof(szValue),"%"PRIu64,ullValue);
        mpStr.insert(std::map<uint32_t,std::string>::value_type(dwIndex,szValue));
    }
    
    void setValue(uint32_t dwIndex,const std::string & sValue)
    {
        mpStr.insert(std::map<uint32_t,std::string>::value_type(dwIndex,sValue));
    }

public:
    SqlBind & operator << (uint32_t dwIndex)
    {
        char szBuff[20] = {0};
        snprintf(szBuff,sizeof(szBuff),"%u",dwIndex);
        sPattern.append(szBuff);
        return *this;
    }
    
    SqlBind & operator << (uint64_t dwIndex)
    {
        char szBuff[20] = {0};
        snprintf(szBuff,sizeof(szBuff),"%"PRIu64,dwIndex);
        sPattern.append(szBuff);
        return *this;
    }
    
    SqlBind & operator << (int64_t dwIndex)
    {
        char szBuff[20] = {0};
        snprintf(szBuff,sizeof(szBuff),"%"PRId64,dwIndex);
        sPattern.append(szBuff);
        return *this;
    }
    
    SqlBind & operator << (const std::string & s)
    {
        sPattern.append(s);
        return *this;
    }
    
template <typename T>
std::string getSql(T* engine) const
{
    std::string sSql;
    int n = mpInt.size() + mpUint.size() + mpStr.size();
    std::string::size_type p1 = 0;
    std::string::size_type p2 = 0;
    for (int i = 1; i <= n; i++)
    {
        char szIndex[20]={0};
        snprintf(szIndex,sizeof(szIndex),":%u",i);
        p2 = sPattern.find(szIndex,p1);
        if (p2 == std::string::npos)
        {
            break;
        }
        sSql.append(sPattern,p1,p2-p1);
        std::map<uint32_t,int32_t>::const_iterator it1 = mpInt.find(i);
        if (it1 != mpInt.end())
        {
            char szTmp[20]={0};
            snprintf(szTmp,sizeof(szTmp),"%d",it1->second);
            sSql.append(szTmp);
            p1 = p2 + strlen(szIndex);
            continue;
        }
        std::map<uint32_t,uint32_t>::const_iterator it2 = mpUint.find(i);
        if (it2 != mpUint.end())
        {
            char szTmp[20]={0};
            snprintf(szTmp,sizeof(szTmp),"%u",it2->second);
            sSql.append(szTmp);
            p1 = p2 + strlen(szIndex);
            continue;
        }
        std::map<uint32_t,std::string>::const_iterator it3 = mpStr.find(i);
        if (it3 != mpStr.end())
        {
            sSql.append(1,'\'');
            sSql.append(engine->escape(it3->second));
            sSql.append(1,'\'');
            p1 = p2 + strlen(szIndex);
            continue;
        }
        sSql.append(szIndex);
        p1 = p2 + strlen(szIndex);
    }

    if (p1 < sPattern.size())
    {
        sSql.append(sPattern,p1,sPattern.size()-p1);
    }
    return sSql;
}

public:
    std::string sPattern;
    std::map<uint32_t,int32_t> mpInt;
    std::map<uint32_t,uint32_t> mpUint;
    std::map<uint32_t,std::string> mpStr;
    
    template<typename AR>
    AR& serialize(AR &ar)
    {
        _SERIALIZE_(ar, sPattern);
        _SERIALIZE_(ar, mpInt);
        _SERIALIZE_(ar, mpUint);
        _SERIALIZE_(ar, mpStr);
        
        return ar;
    }
};
}

#endif

