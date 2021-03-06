// vim:ts=4:sw=4:expandtab

/*
 * file:   neiku/cgx.h
 * desc:   基于ClearSilver/fastcgi(libfcgi)的CGI工具，支持CGI协议、FastCGI协议
 * author: YIF
 * date:   2016/09/17 16:00:00
 * version:
 *         2016/09/17 支持简单的get/set方法
 *                    支持获取上传文件FILE*(无需fclose)
 *                    支持url/uri跳转(302)
 *                    支持模板渲染输出(cs)
 *                    支持在线dump/debug数据
 *                    支持压缩输出(gzip)
 *         2016/09/18 支持FastCGI协议(需外部wrapper拉起)
 *                    支持自动兼容CGI/FastCGI运行模式
 *                    支持通过nerr_error_traceback获取更多出错现场
 *         2016/10/03 改名为CGX (X=> cgi+fastcgi+cpp)
 *         2017/09/26 支持单例，方便使用
 *                    支持<!--#include virtual=""-->方式包含页面片
 *                    支持直接获取http body自行解析
 *                    支持json_escape
 *         2017/10/03 支持setValue任意对象
 *         2018/10/28 支持render任意对象(返回application/json)
 *         2018/10/30 支持parse JSON POST到任意对象
 *         2018/11/04 支持getHeader()获取任意HTTP 头(不用硬编码于代码中)
 *         2018/11/22 支持parse XML POST到任意对象(content-type要求textxml或者application/xml)
 *         2018/11/23 支持render任意对象到XML(返回application/xml)
 *
 * link: -I~/opt/clearsilver/include/ClearSilver/
 *       -L~/opt/clearsilver/lib/ -lneo_cgi -lneo_cs -lneo_utl -lz
 *       -I~/opt/fastcgi/include/
 *       -L~/opt/fastcgi/lib/ -lfcgi
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
 * 3、自动兼容CGI/FastCGI的示例
 *    CCgx *CGX = new CCgx;
 *    while (CGX->Accept())
 *    {
 *         // handle request
 *         // CGI->Render();
 *    }
 */

#ifndef __NEIKU_CGX_H__
#define __NEIKU_CGX_H__ 1

#include <cstdlib>
#include <string>
#include "ClearSilver.h"

#include "neiku/serialize.h"
#include "neiku/json_serialize.h"
#include "neiku/xml_serialize.h"
#include "neiku/singleton.h"
#define CGX SINGLETON(neiku::CgX)

namespace neiku
{

#define CHECK_INIT(RET) if (!m_bInit && !init()) { setErrMsg("cgx init fail"); return RET; }

class CgX
{

public:
    CgX();
    ~CgX();

    void main(int argc, char* argv[]);
    int accept();

    const char* getErrMsg();

public:
    const char* getValue(const char* szName, const char* szDefVal = "");
    int32_t  getValue(const char* szName, int32_t iDefVal = 0);
    int64_t  getValue(const char* szName, int64_t lDefVal = 0);
    uint32_t getValue(const char* szName, uint32_t dwDefVal = 0);
    uint64_t getValue(const char* szName, uint64_t ddwDefVal = 0);

    const std::string getHeader(const char* szName, const char* szDefVal = "");

    template<typename OBJ>
    int parse(OBJ& obj)
    {
        CHECK_INIT(-1);

        // JSON
        if (m_bIsJsonPost && !m_sHttpBody.empty())
        {
            CJsonDecoder decoder;
            int ret = decoder.Parse(m_sHttpBody);
            if (ret != 0)
            {
                return ret;
            }
            decoder >> obj;
            return 0;
        }

        // XML
        if (m_bIsXmlPost && !m_sHttpBody.empty())
        {
            XmlParser parser(m_sHttpBody.data(), m_sHttpBody.size());
            if (!parser)
            {
                return -1;
            }
            parser >> obj;
            return 0;
        }

        //
        obj.serialize(*this);
        return 0;
    }

    CgX& operator & (Key& key)
    {
        m_pKey = key.c_str();
        return *this;
    }

    CgX& operator & (std::string& d)
    {
        std::string key;
        key.append("Query.").append(m_pKey);

        d = getValue(key.c_str(), "");
        return *this;
    }

    template <typename T>
    CgX& operator & (T& o)
    {
        return o.serialize(*this);
    }

    // 不支持GET/FORM-POST自动转std::vector
    // 此时请求JSON或者XML POST
    template <typename T>
    CgX& operator & (std::vector<T>& v)
    {
        return *this;
    }

#define PARSE_NUM(T) \
    CgX& operator & (T& d) \
    { \
        std::string key; \
        key.append("Query.").append(m_pKey); \
      \
        d = getValue(key.c_str(), 0); \
        return *this; \
    }
    PARSE_NUM(int32_t);
    PARSE_NUM(int64_t);
    PARSE_NUM(uint32_t);
    PARSE_NUM(uint64_t);

    void setValue(const char* szName, int iValue);
    void setValue(const char* szName, const char* szValue);
    void setValue(const char* szName, const std::string& sValue);

    template<typename OBJ>
    void setValue(const char* szName, OBJ& obj)
    {
        CHECK_INIT();

        HdfDumper dumper(szName);
        dumper << obj;
        hdf_read_string_ignore(m_pCGI->hdf, dumper.str().c_str(), 1);
    }


public:
    void setParseHttpBodyManually(bool bFlag = true);
    const std::string getHttpBody();

public:
    std::string getRemoteAddr();

public:
    void setContentType(const char* szContentType);

public:
    // do not fclose it
    FILE* getFile(const char* szName);

public:
    // 302 to http(s)://domain/path/to/xxx
    void redirect302Url(const char* fmt, ...);
    // 302 to /path/to/xxx
    void redirect302Uri(const char* fmt, ...);

public:
    int render(const char* szTplPath);

    template<typename OBJ>
    int render(OBJ& obj)
    {
        CHECK_INIT(-1);

        CJsonEncoder encoder;
        encoder << obj;

        setContentType("application/json");
        return render(encoder.str());
    }

    template<typename OBJ>
    int renderXML(OBJ& obj, const std::string& root = "")
    {
        CHECK_INIT(-1);

        XmlDumper dumper(root.c_str());
        dumper << obj;

        setContentType("application/xml");
        return render(dumper.str());
    }

    int render(const std::string& output);

private:
    bool init();
    void destroy();

    void setErrMsg(NEOERR* pError);
    void setErrMsg(const char* szErrMsg);

    int render(const char* szTplPath, STRING& stOutput);

private:
    void setValue(const char * pHdfDoc);
    bool isJsonPost(const char* szContentType);
    bool isXmlPost(const char* szContentType);

private:
    CGI *m_pCGI;
    bool m_bInit;
    std::string m_sLastErrMsg;

    bool m_bAccepted;

    bool m_bParseHttpBodyManually;
    std::string m_sHttpBody;

    const char* m_pKey;
    bool m_bIsJsonPost;

    bool m_bIsXmlPost;
};

};

#endif
