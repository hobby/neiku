// vim:ts=4:sw=4:expandtab

/*
 * file:   neiku/cgi.h
 * desc:   基于ClearSilver的CGI工具，支持CGI协议
 * author: YIF
 * date:   2016/09/17 16:00:00
 * version: 
 *         2016/09/17 支持简单的get/set方法
 *                    支持获取上传文件FILE*(无需fclose)
 *                    支持url/uri跳转(302)
 *                    支持模板渲染输出(cs)
 *                    支持在线dump/debug数据
 *                    支持压缩输出(gzip)
 *
 * link: -I~/opt/clearsilver/include/ClearSilver/
 *       -L~/opt/clearsilver/lib/ -lneo_cgi -lneo_cs -lneo_utl -lz
 *
 * basic:
 * 1、GetValue/SetValue中的name规范
 *    GET、POST数据 => Query.* （例如请求http://localhost/?name=YIF，那么Query.name=YIF）
 *    COOKIE       => Cookie.*
 *    HTTP         => HTTP.*
 *    CGI          => CGI.*
 * 2、在线dump/debug
 *    dump  => http://localhost/?debug=neiku@dump
 *    debug => http://localhost/?debug=neiku@debug
 */

#ifndef __NEIKU_CGI_H__
#define __NEIKU_CGI_H__ 1

#include <cstdlib>
#include <string>
#include "ClearSilver.h"

namespace neiku
{

class CCgi
{

public:
    CCgi();
    ~CCgi();

    const char* GetErrMsg();

public:
    const char* GetValue(const char* szName, const char* szDefVal = "");
    int32_t  GetValueI(const char* szName, int32_t iDefVal = 0);
    int64_t  GetValueII(const char* szName, int64_t lDefVal = 0);
    uint32_t GetValueUI(const char* szName, uint32_t dwDefVal = 0);
    uint64_t GetValueUII(const char* szName, uint64_t ddwDefVal = 0);

    int SetValue(const char* szName, const char* szValue);
    int SetValue(const char* szName, int iValue);

public:
    // do not fclose it
    FILE* GetFile(const char* szName);

public:
    // 302 to http(s)://domain/path/to/xxx
    void Redirect302Url(const char* fmt, ...);
    // 302 to /path/to/xxx
    void Redirect302Uri(const char* fmt, ...);

public:
    int Render(const char* szTplPath);

private:
    bool Init();
    void Destroy();
    
    void SetErrMsg(NEOERR* pError);
    void SetErrMsg(const char* szErrMsg);

    int Render(const char* szTplPath, STRING& stOutput);

private:
    CGI *m_pCGI;
    bool m_bInit;
    std::string m_sLastErrMsg;
};

};



#endif