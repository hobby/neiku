// vim:ts=4:sw=4:expandtab

/*
 * file:   neiku/cs.h
 * desc:   基于ClearSilver的模板引擎工具
 * author: YIF
 * date:   2016/07/22 22:12:00
 * version: 
 *     2016/07/11: 支持字符串/文件模板渲染
 *                 支持dump出数据集
 *
 *
 * link: -I~/opt/clearsilver/include/ClearSilver/
 *       -L~/opt/clearsilver/lib/ -lneo_cs -lneo_utl
 */

#ifndef __NEIKU_CS_H__
#define __NEIKU_CS_H__ 1

#include <string>
#include "ClearSilver.h"

namespace neiku
{

class CCS
{
public:
    CCS();
    ~CCS();

public:
    int Init();
    
    int SetValue(const char* szName, const char* szValue);
    
    int Dump(std::string& sOutput);
    
    int Render(const char* szTplPath, std::string& sOutput);
    int Render(const char* szTplStr, int iTplLen, std::string& sOutput);
    
    const char* GetErrMsg();

private:
    int Destroy();
    
    void SetErrMsg(NEOERR* pError);
    void SetErrMsg(const char* szErrMsg);

private:
    HDF *m_pHDF;
    bool m_bInit;
    std::string m_sLastErrMsg;
};

};



#endif
