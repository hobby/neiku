// vim:ts=4:sw=4:expandtab

#include <string.h>
#include <stdlib.h>

#include "neiku/cgi.h"

using namespace neiku;

static NEOERR* RenderOutputCallback(void *ctx, char *buf)
{
    return nerr_pass(string_append(static_cast<STRING*>(ctx), buf));
}

#define CHECK_INIT(RET) if (!m_bInit && !Init()) { SetErrMsg("cgi util init fail"); return RET; }

CCgi::CCgi(): m_pCGI(NULL), m_bInit(false)
{
    Init();
}

CCgi::~CCgi()
{
    Destroy();
}

const char* CCgi::GetErrMsg()
{
    return m_sLastErrMsg.c_str();
}

const char* CCgi::GetValue(const char* szName, const char* szDefVal)
{
    CHECK_INIT(szDefVal);

    return hdf_get_value(m_pCGI->hdf, szName, szDefVal);
}

int32_t CCgi::GetValueI(const char* szName, int32_t iDefVal)
{
    const char* szValue = GetValue(szName, NULL);
    if (szValue == NULL)
    {
        return iDefVal;
    }

    return strtol(szValue, NULL, 0);
}

int64_t CCgi::GetValueII(const char* szName, int64_t lDefVal)
{
    const char* szValue = GetValue(szName, NULL);
    if (szValue == NULL)
    {
        return lDefVal;
    }

    return strtoll(szValue, NULL, 0);
}

uint32_t CCgi::GetValueUI(const char* szName, uint32_t dwDefVal)
{
    const char* szValue = GetValue(szName, NULL);
    if (szValue == NULL)
    {
        return dwDefVal;
    }

    return strtoul(szValue, NULL, 0);
}

uint64_t CCgi::GetValueUII(const char* szName, uint64_t ddwDefVal)
{
    const char* szValue = GetValue(szName, NULL);
    if (szValue == NULL)
    {
        return ddwDefVal;
    }

    return strtoull(szValue, NULL, 0);
}

int CCgi::SetValue(const char* szName, const char* szValue)
{
    CHECK_INIT(-1);
    
    NEOERR *pError = hdf_set_value(m_pCGI->hdf, szName, szValue);
    if (pError != STATUS_OK)
    {
        SetErrMsg(pError);
        return -1;
    }

    return 0;
}

int CCgi::SetValue(const char* szName, int iValue)
{
    CHECK_INIT(-1);

    NEOERR *pError = hdf_set_int_value(m_pCGI->hdf, szName, iValue);
    if (pError != STATUS_OK)
    {
        SetErrMsg(pError);
        return -1;
    }

    return 0;
}

// do not fclose it
FILE* CCgi::GetFile(const char* szName)
{
    CHECK_INIT(NULL);
    return cgi_filehandle(m_pCGI, szName);
}

// 302 to http(s)://domain/path/to/xxx
void CCgi::Redirect302Url(const char* fmt, ...)
{
    CHECK_INIT();

    va_list ap;
    va_start(ap, fmt);
    cgi_vredirect(m_pCGI, 0, fmt, ap);
    va_end(ap);
}

// 302 to /path/to/xxx
void CCgi::Redirect302Uri(const char* fmt, ...)
{
    CHECK_INIT();

    va_list ap;
    va_start(ap, fmt);
    cgi_vredirect(m_pCGI, 1, fmt, ap);
    va_end(ap);
}

int CCgi::Render(const char* szTplPath)
{
    CHECK_INIT(-1);

    STRING stOutput;
    string_init(&stOutput);
    
    if (Render(szTplPath, stOutput) != 0)
    {
        char szErrMsg[1024] = {0};
        snprintf(szErrMsg, sizeof(szErrMsg)
                , "render fail, tpl:[%s], msg:[%s]"
                , szTplPath, GetErrMsg());
        SetErrMsg(szErrMsg);
        return -1;
    }

    NEOERR *pError = cgi_output(m_pCGI, &stOutput);
    string_clear(&stOutput);
    if(pError != STATUS_OK)
    {
        
        SetErrMsg(pError);
        return -2;
    }

    return 0;
}

bool CCgi::Init()
{
    NEOERR *pError = NULL;

    if (m_bInit)
    {
        return true;
    }

    Destroy();
    
    // parse http header
    pError = cgi_init(&m_pCGI, NULL);
    if (pError != STATUS_OK)
    {
        SetErrMsg(pError);
        return false;
    }

    // parse http body
    pError = cgi_parse(m_pCGI);
    if (pError != STATUS_OK)
    {
        SetErrMsg(pError);
        return false;
    }

    // enable online debug/dump
    hdf_set_int_value(m_pCGI->hdf, "Config.DebugEnabled", 1);
    hdf_set_value(m_pCGI->hdf, "Config.DebugPassword", "neiku@debug");
    hdf_set_value(m_pCGI->hdf, "Config.DumpPassword", "neiku@dump");
    
    // enable gzip
    hdf_set_int_value(m_pCGI->hdf, "Config.CompressionEnabled", 1);
    
    // disable timefooter
    hdf_set_int_value(m_pCGI->hdf, "Config.TimeFooter", 0);
    
    // template's search paths
    const char* pRoot = hdf_get_value(m_pCGI->hdf, "CGI.DocumentRoot", NULL);
    if (pRoot != NULL)
    {
        hdf_set_value(m_pCGI->hdf, "hdf.loadpaths.0", pRoot);
    }

    m_bInit = true;
    return true;
}

void CCgi::Destroy()
{
    m_bInit = false;
    
    if(m_pCGI != NULL)
    {
        cgi_destroy(&m_pCGI);
        m_pCGI = NULL;
    }
}

int CCgi::Render(const char* szTplPath, STRING& stOutput)
{
    CHECK_INIT(-1);

    // dump online
    const char* pDebug  = hdf_get_value(m_pCGI->hdf, "Query.debug", NULL);
    const char* pPasswd = hdf_get_value(m_pCGI->hdf, "Config.DumpPassword", NULL);
    if (   pDebug != NULL
        && pPasswd != NULL
        && strcmp(pDebug, pPasswd) == 0)
    {
        NEOERR *pError = hdf_dump_str(m_pCGI->hdf, NULL, 0, &stOutput);
        if(pError != STATUS_OK)
        {
            SetErrMsg(pError);
            return -1;
        }

        return 0;
    }

    // render with template
    CSPARSE* pParse = NULL;
    NEOERR* pError = cs_init(&pParse, m_pCGI->hdf);
    if(pError != STATUS_OK)
    {
        SetErrMsg(pError);
        return -1;
    }

    int iRet = -1;
    do
    {
        pError = cgi_register_strfuncs(pParse);
        if(pError != STATUS_OK)
        {
            SetErrMsg(pError);
            break;
        }

        pError = cs_parse_file(pParse, szTplPath);
        if(pError != STATUS_OK)
        {
            SetErrMsg(pError);
            break;
        }

        pError = cs_render(pParse, &stOutput, RenderOutputCallback);
        if(pError != STATUS_OK)
        {
            SetErrMsg(pError);
            break;
        }

        iRet = 0;
    } while (0);

    if(pParse != NULL)
    {
        cs_destroy(&pParse);
        pParse = NULL;
    }

    return iRet;
}

void CCgi::SetErrMsg(NEOERR* pError)
{
    if(pError == STATUS_OK)
    {
        m_sLastErrMsg = "";
        return;
    }

    STRING str;
    string_init(&str);
    nerr_error_string(pError, &str);
    nerr_ignore(&pError);
    m_sLastErrMsg.assign(str.buf, str.len);
    string_clear(&str);
}

void CCgi::SetErrMsg(const char* szErrMsg)
{
    if(szErrMsg != NULL)
    {
        m_sLastErrMsg = szErrMsg;
    }
}
