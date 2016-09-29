
#include <string>
#include <string.h>
#include "neiku/cs.h"

static NEOERR* RenderOutputCallback(void *ctx, char *buf)
{
    return nerr_pass(string_append(static_cast<STRING*>(ctx), buf));
}

neiku::CCS::CCS(): m_pHDF(NULL)
                 , m_bInit(false)
{}

neiku::CCS::~CCS()
{
    Destroy();
}

int neiku::CCS::Init()
{
    if(m_bInit == true)
    {
        return 0;
    }

    NEOERR* pError = hdf_init(&m_pHDF);
    if(pError != STATUS_OK)
    {
        SetErrMsg(pError);
        return -1;
    }

    m_bInit = true;
    return 0;
}

int neiku::CCS::Destroy()
{
    m_bInit = false;
    if(m_pHDF != NULL)
    {
        hdf_destroy(&m_pHDF);
        m_pHDF = NULL;
    }
    m_sLastErrMsg = "";
    return 0;
}

int neiku::CCS::SetValue(const char* szName, const char* szValue)
{
    if(m_bInit == false)
    {
        SetErrMsg("cs util not init");
        return -1;
    }

    NEOERR* pError = hdf_set_value(m_pHDF, szName, szValue);
    if(pError != STATUS_OK)
    {
        SetErrMsg(pError);
        return -1;
    }

    return 0;
}

int neiku::CCS::Render(const char* szTplStr, int iTplLen, std::string& sOutput)
{
    int iRet = 0;

    if(m_bInit == false)
    {
        SetErrMsg("cs util not init");
        return -1;
    }

    CSPARSE* pParse = NULL;
    NEOERR* pError = cs_init(&pParse, m_pHDF);
    if(pError != STATUS_OK)
    {
        SetErrMsg(pError);
        return -1;
    }

    STRING str;
    string_init(&str);

    iRet = -1;
    char* szString = NULL;
    do
    {
        szString = static_cast<char*>(malloc(iTplLen));
        if(szString == NULL)
        {
            SetErrMsg("no more memory");
            return -1;
        }
        memcpy(szString, szTplStr, iTplLen);

        pError = cs_parse_string(pParse, szString, iTplLen);
        if(pError != STATUS_OK)
        {
            SetErrMsg(pError);
            break;
        }

        pError = cs_render(pParse, &str, RenderOutputCallback);
        if(pError != STATUS_OK)
        {
            SetErrMsg(pError);
            break;
        }
        sOutput.assign(str.buf, str.len);

        iRet = 0;
    } while (0);

    string_clear(&str);

    if(pParse != NULL)
    {
        cs_destroy(&pParse);
        pParse = NULL;
    }

    return iRet;
}

int neiku::CCS::Render(const char* szTplPath, std::string& sOutput)
{
    int iRet = 0;

    if(m_bInit == false)
    {
        SetErrMsg("cs util not init");
        return -1;
    }

    CSPARSE* pParse = NULL;
    NEOERR* pError = cs_init(&pParse, m_pHDF);
    if(pError != STATUS_OK)
    {
        SetErrMsg(pError);
        return -1;
    }

    STRING str;
    string_init(&str);

    iRet = -1;
    do
    {
        pError = cs_parse_file(pParse, szTplPath);
        if(pError != STATUS_OK)
        {
            SetErrMsg(pError);
            break;
        }

        pError = cs_render(pParse, &str, RenderOutputCallback);
        if(pError != STATUS_OK)
        {
            SetErrMsg(pError);
            break;
        }
        sOutput.assign(str.buf, str.len);

        iRet = 0;
    } while (0);

    string_clear(&str);

    if(pParse != NULL)
    {
        cs_destroy(&pParse);
        pParse = NULL;
    }

    return iRet;
}

int neiku::CCS::Dump(std::string& sOutput)
{
    STRING str;
    string_init(&str);

    NEOERR *pError = hdf_dump_str(m_pHDF, NULL, 0, &str);
    if(pError != STATUS_OK)
    {
        string_clear(&str);
        SetErrMsg(pError);
        return -1;
    }

    sOutput.assign(str.buf, str.len);
    string_clear(&str);
    return 0;
}

const char* neiku::CCS::GetErrMsg()
{
    return m_sLastErrMsg.c_str();
}

void neiku::CCS::SetErrMsg(const char* szErrMsg)
{
    if(szErrMsg != NULL)
    {
        m_sLastErrMsg = szErrMsg;
        return;
    }

    return;
}

void neiku::CCS::SetErrMsg(NEOERR* pError)
{
    if(pError == STATUS_OK)
    {
        m_sLastErrMsg = "";
        return;
    }

    STRING str;
    string_init(&str);
    nerr_error_string(pError, &str);
    m_sLastErrMsg.assign(str.buf, str.len);
    string_clear(&str);
    return;
}

