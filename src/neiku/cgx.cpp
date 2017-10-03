// vim:ts=4:sw=4:expandtab

#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "neiku/cgx.h"

#include "fcgios.h"
#include "fcgi_stdio.h"

extern char** environ;

#define CHECK_INIT(RET) if (!m_bInit && !init()) { setErrMsg("cgx init fail"); return RET; }

using namespace neiku;

static NEOERR* HandleSSI(void *ctx, HDF *hdf, const char *filename,char **content)
{
    char fpath[_POSIX_PATH_MAX];
    NEOERR * err = hdf_search_path (hdf, filename, fpath);
    if ( err != STATUS_OK )
    {
        return nerr_pass(err);
    }
    filename = fpath;

    err = ne_load_file (filename, content);
    if ( err != STATUS_OK )
    {
        return nerr_pass(err);
    }

    //将<!--#include virtual="/footer.html" -->转换成
    //  <?cs        linclude:"/footer.html"  ?>
    char szInclude[]    = "<!--#include";
    char szCsInclude[]    = "<?cs linclude:";

    char szIncludeEnd[]   = "-->";
    char szCsIncludeEnd[] = " ?>";

    char * pStart = NULL;
    while ( (pStart = strstr(*content,szInclude)) != NULL )
    {
        char * pEnd = strstr(pStart + sizeof(szInclude),szIncludeEnd);
        char * pQuote = strstr(pStart + sizeof(szInclude),"\"");
        if ( !pEnd || !pQuote )
        {
            pStart += sizeof(szInclude);
            continue;
        }

        memset(pStart,0x20,pQuote - pStart);
        strncpy(pStart,szCsInclude,sizeof(szCsInclude) - 1);
        strncpy(pEnd,szCsIncludeEnd,sizeof(szCsIncludeEnd) - 1);
        pStart = pEnd + sizeof(szIncludeEnd);
    }
    return STATUS_OK;
}

// json escape (utf-8 only)
// http://json.org/
static NEOERR* json_escape(const char *in, char **out)
{
    if (!in)
    {
        return nerr_raise(NERR_ASSERT, "in is null, in:[%p]", in);
    }

    size_t len = strlen(in);
    const char* beg = in;
    const char* end = beg + len;

    char* esc = (char*)malloc(2 * len + 1);
    char* cur = esc;
    while ( beg < end )
    {
        switch (*beg)
        {
            case '"' : *(cur++) = '\\'; *(cur++) = '"';  beg++; break;
            case '\\': *(cur++) = '\\'; *(cur++) = '\\'; beg++; break;
            case '/' : *(cur++) = '\\'; *(cur++) = '/';  beg++; break;
            case '\b': *(cur++) = '\\'; *(cur++) = 'b';  beg++; break;
            case '\f': *(cur++) = '\\'; *(cur++) = 'f';  beg++; break;
            case '\n': *(cur++) = '\\'; *(cur++) = 'n';  beg++; break;
            case '\r': *(cur++) = '\\'; *(cur++) = 'r';  beg++; break;
            case '\t': *(cur++) = '\\'; *(cur++) = 't';  beg++; break;
            default:
                // 不管那33个控制字符
                *(cur++) = *beg;
                beg++;
                break;
        }
    }
    *cur = '\0';

    *out = esc;
    return STATUS_OK;
}

static NEOERR* RenderOutputCallback(void *ctx, char *buf)
{
    return nerr_pass(string_append(static_cast<STRING*>(ctx), buf));
}

// enable cgi & fastcgi with libfcgi
static int cs_read(void *ctx, char *s, int n)
{
    return FCGI_fread(s, 1, n, FCGI_stdin);
}
static int cs_vprintf(void *ctx, const char *s, va_list args)
{
    FCGI_vprintf(s, args);
    return 0;
}
static int cs_write(void *ctx, const char *s, int n)
{
    return FCGI_fwrite(const_cast<char *>(s), 1, n, FCGI_stdout);
}

// copy from clearsilver/cgi/cgi.c
static NEOERR* add_cgi_env_var(CGI* cgi, const char* env, const char* name)
{
    NEOERR* err;
    char* s;

    err = cgiwrap_getenv(env, &s);
    if (err != STATUS_OK) return nerr_pass(err);
    if (s != NULL)
    {
        err = hdf_set_buf(cgi->hdf, name, s);
        if (err != STATUS_OK)
        {
            free(s);
            return nerr_pass(err);
        }
    }
    return STATUS_OK;
}

// copy from clearsilver/cgi/cgi.c@376
static NEOERR* GetHttpBody(CGI* cgi, std::string* ptr)
{
    const char* val = hdf_get_value(cgi->hdf, "CGI.ContentLength", NULL);
    if (!val)
    {
        return STATUS_OK;
    }

    int len = strtol(val, NULL, 0);
    if (len <= 0)
    {
        return STATUS_OK;
    }

    cgi->data_expected = len;

    char* buf = (char*)malloc(sizeof(char)*(len));
    if (!buf)
    {
        return nerr_raise_errno(NERR_NOMEM, "Unable to allocate memory to read POST input of length %d", len);
    }

    int offset = 0, count = 0;
    while (offset < len)
    {
        cgiwrap_read(buf + offset, len - offset, &count);
        if (count <= 0)
        {
            break;
        }
        offset += count;
    }
    if (count < 0)
    {
        free(buf);
        return nerr_raise_errno(NERR_IO, "Short read on CGI POST input (%d < %d)", offset, len);
    }
    if (offset != len)
    {
        free(buf);
        return nerr_raise_errno(NERR_IO, "Short read on CGI POST input (%d < %d)", offset, len);
    }

    ptr->assign(buf, len);
    free(buf);
    return STATUS_OK;
}

// split string to string-list
static void explode(std::vector<std::string>& list, const std::string& str
                    , const char sep = '|', const bool drop_empty_sec = true)
{
    std::string::size_type beg = 0;
    std::string::size_type end = 0;
    while (end != std::string::npos)
    {
        std::string sec;

        end = str.find_first_of(sep, beg);
        if (end != std::string::npos)
        {
            sec = str.substr(beg, (end - beg));
        }
        else
        {
            sec = str.substr(beg, std::string::npos);
        }

        if (!sec.empty() || !drop_empty_sec)
        {
            list.push_back(sec);
        }
        beg = end + 1;
    }
}

CgX::CgX(): m_pCGI(NULL), m_bInit(false), m_bAccepted(false), m_bParseHttpBodyManually(false)
{
    // enable cgi & fastcgi with libfcgi(fcgi_stdio.h)
    cgiwrap_init_std(0, NULL, environ);
    cgiwrap_init_emu(NULL, cs_read, cs_vprintf, cs_write, NULL, NULL, NULL);
}

CgX::~CgX()
{
    // always cleanup
    destroy();

    // free memory allocated in OS_LibInit()
    // inside the FCGI_Accept()
    OS_LibShutdown();
}

int CgX::accept()
{
    if(!FCGX_IsCGI())
    {
        destroy();
        return FCGI_Accept();
    }

    if(!m_bAccepted)
    {
        FCGI_Accept();
        m_bAccepted = true;
        return 0;
    }

    return -1;
}

const char* CgX::getErrMsg()
{
    return m_sLastErrMsg.c_str();
}

const char* CgX::getValue(const char* szName, const char* szDefVal)
{
    CHECK_INIT(szDefVal);

    return hdf_get_value(m_pCGI->hdf, szName, szDefVal);
}

int32_t CgX::getValue(const char* szName, int32_t iDefVal)
{
    CHECK_INIT(iDefVal);

    const char* szValue = hdf_get_value(m_pCGI->hdf, szName, NULL);
    if (szValue == NULL)
    {
        return iDefVal;
    }

    return strtol(szValue, NULL, 0);
}

int64_t CgX::getValue(const char* szName, int64_t lDefVal)
{
    CHECK_INIT(lDefVal);

    const char* szValue = hdf_get_value(m_pCGI->hdf, szName, NULL);
    if (szValue == NULL)
    {
        return lDefVal;
    }

    return strtoll(szValue, NULL, 0);
}

uint32_t CgX::getValue(const char* szName, uint32_t dwDefVal)
{
    CHECK_INIT(dwDefVal);

    const char* szValue = hdf_get_value(m_pCGI->hdf, szName, NULL);
    if (szValue == NULL)
    {
        return dwDefVal;
    }

    return strtoul(szValue, NULL, 0);
}

uint64_t CgX::getValue(const char* szName, uint64_t ddwDefVal)
{
    CHECK_INIT(ddwDefVal);

    const char* szValue = hdf_get_value(m_pCGI->hdf, szName, NULL);
    if (szValue == NULL)
    {
        return ddwDefVal;
    }

    return strtoull(szValue, NULL, 0);
}

void CgX::setValue(const char* szName, const std::string& sValue)
{
    setValue(szName, sValue.c_str());
}

void CgX::setValue(const char* szName, const char* szValue)
{
    CHECK_INIT();

    hdf_set_value(m_pCGI->hdf, szName, szValue);
}

void CgX::setValue(const char* szName, int iValue)
{
    CHECK_INIT();

    hdf_set_int_value(m_pCGI->hdf, szName, iValue);
}

void CgX::setValue(const char * pHdfDoc)
{
    CHECK_INIT();

    hdf_read_string(m_pCGI->hdf, pHdfDoc);
}

void CgX::setContentType(const char* szContentType)
{
    setValue("cgiout.ContentType", szContentType);
}

// 302 to http(s)://domain/path/to/xxx
void CgX::redirect302Url(const char* fmt, ...)
{
    CHECK_INIT();

    va_list ap;
    va_start(ap, fmt);
    cgi_vredirect(m_pCGI, 1, fmt, ap);
    va_end(ap);
}

// 302 to /path/to/xxx
void CgX::redirect302Uri(const char* fmt, ...)
{
    CHECK_INIT();

    va_list ap;
    va_start(ap, fmt);
    cgi_vredirect(m_pCGI, 0, fmt, ap);
    va_end(ap);
}

bool CgX::init()
{
    NEOERR *pError = NULL;

    if (m_bInit)
    {
        return true;
    }

    if (FCGX_IsCGI() && !m_bAccepted && (FCGI_Accept() < 0))
    {
        return false;
    }

    // parse http header
    pError = cgi_init(&m_pCGI, NULL);
    if (pError != STATUS_OK)
    {
        setErrMsg(pError);
        return false;
    }

    // add header
    add_cgi_env_var(m_pCGI, "HTTP_X_REAL_IP", "HTTP.X-Real-IP");
    add_cgi_env_var(m_pCGI, "HTTP_X_FORWARDED_FOR", "HTTP.X-Forwarded-For");

    // enable online debug/dump
    hdf_set_int_value(m_pCGI->hdf, "Config.DebugEnabled", 1);
    hdf_set_value(m_pCGI->hdf, "Config.DebugPassword", "neiku@debug");
    hdf_set_value(m_pCGI->hdf, "Config.DumpPassword", "neiku@dump");

    // enable gzip
    hdf_set_int_value(m_pCGI->hdf, "Config.CompressionEnabled", 1);

    // disable timefooter
    hdf_set_int_value(m_pCGI->hdf, "Config.TimeFooter", 0);

    // tmp directory
    hdf_set_value(m_pCGI->hdf, "Config.Upload.TmpDir", "/tmp");

    // template's search paths
    const char* pRoot = hdf_get_value(m_pCGI->hdf, "CGI.DocumentRoot", "./");
    hdf_set_value(m_pCGI->hdf, "hdf.loadpaths.0", pRoot);

    // default content-type
    hdf_set_value(m_pCGI->hdf, "cgiout.ContentType", "text/html; charset=utf-8");

    // parse http body
    // parse http body
    if (m_bParseHttpBodyManually)
    {
        // here you can parse http body yourself
        // with CGX->setParseHttpBodyManually(true) frist
        // and CGX->getHttpBody() second
        pError = GetHttpBody(m_pCGI, &m_sHttpBody);
    }
    else
    {
        // only application/x-www-form-urlencoded & multipart/form-data
        pError = cgi_parse(m_pCGI);
    }
    if (pError != STATUS_OK)
    {
        setErrMsg(pError);
        return false;
    }

    m_bInit = true;
    return true;
}

void CgX::destroy()
{
    m_bInit = false;

    if(m_pCGI != NULL)
    {
        cgi_destroy(&m_pCGI);
        m_pCGI = NULL;
    }

    m_bAccepted = false;
}

int CgX::render(const char* szTplPath)
{
    CHECK_INIT(-1);

    STRING stOutput;
    string_init(&stOutput);

    if (render(szTplPath, stOutput) != 0)
    {
        char szErrMsg[1024] = {0};
        snprintf(szErrMsg, sizeof(szErrMsg)
                , "render fail, tpl:[%s], msg:[%s]"
                , szTplPath, getErrMsg());
        setErrMsg(szErrMsg);
        return -1;
    }

    NEOERR *pError = cgi_output(m_pCGI, &stOutput);
    string_clear(&stOutput);
    if(pError != STATUS_OK)
    {

        setErrMsg(pError);
        return -2;
    }

    return 0;
}

int CgX::render(const char* szTplPath, STRING& stOutput)
{
    CHECK_INIT(-1);

    // dump online
    const char* pDebug  = hdf_get_value(m_pCGI->hdf, "Query.debug", NULL);
    const char* pPasswd = hdf_get_value(m_pCGI->hdf, "Config.DumpPassword", NULL);
    if (   pDebug != NULL && pPasswd != NULL && strcmp(pDebug, pPasswd) == 0)
    {
        setValue("CGX.TemplatePath", szTplPath);
        setContentType("text/plain; charset=utf-8");
        NEOERR *pError = hdf_dump_str(m_pCGI->hdf, NULL, 0, &stOutput);
        if(pError != STATUS_OK)
        {
            setErrMsg(pError);
            return -1;
        }

        return 0;
    }

    // no template no 50x
    struct stat st;
    int iRet = stat(szTplPath, &st);
    if (iRet != 0)
    {
        string_append(&stOutput, "template not found");
        return 0;
    }

    // render with template
    CSPARSE* pParse = NULL;
    NEOERR* pError = cs_init(&pParse, m_pCGI->hdf);
    if(pError != STATUS_OK)
    {
        setErrMsg(pError);
        return -1;
    }

    iRet = -1;
    do
    {
        cs_register_fileload(pParse, NULL, HandleSSI);

        pError = cgi_register_strfuncs(pParse);
        if(pError != STATUS_OK)
        {
            setErrMsg(pError);
            break;
        }

        pError = cs_register_strfunc(pParse, const_cast<char*>("json_escape"), json_escape);
        if(pError != STATUS_OK)
        {
            setErrMsg(pError);
            break;
        }

        pError = cs_parse_file(pParse, szTplPath);
        if(pError != STATUS_OK)
        {
            setErrMsg(pError);
            break;
        }

        pError = cs_render(pParse, &stOutput, RenderOutputCallback);
        if(pError != STATUS_OK)
        {
            setErrMsg(pError);
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

void CgX::setErrMsg(NEOERR* pError)
{
    if(pError == STATUS_OK)
    {
        m_sLastErrMsg = "";
        return;
    }

    STRING str;
    string_init(&str);
    nerr_error_traceback(pError, &str);
    nerr_ignore(&pError);
    m_sLastErrMsg.assign(str.buf, str.len);
    string_clear(&str);
}

void CgX::setErrMsg(const char* szErrMsg)
{
    if(szErrMsg != NULL)
    {
        m_sLastErrMsg = szErrMsg;
    }
}

std::string CgX::getRemoteAddr()
{
    // 客户端真实IP获取顺序：
    // 1. X-Real-Ip              -- 直指客户端出口IP（接入层维护）
    // 3. X-Forwarded-For的第1个 -- https://tools.ietf.org/html/rfc7239
    //                              注意考虑边界，客户端可伪造这个头部
    // 4. REMOTE_ADDR环境变量    -- CGI协议

    CHECK_INIT("");

    const char* ptr = getValue("HTTP.X-Real-IP", "");
    if (ptr != NULL && ptr[0] != '\0')
    {
        return std::string(ptr);
    }

    ptr = getValue("HTTP.X-Forwarded-For", "");
    if (ptr != NULL && ptr[0] != '\0')
    {
        std::vector<std::string> list;
        explode(list, ptr, ',');
        if (!list.empty())
        {
            return std::string(list[0]);
        }
    }

    ptr = getValue("CGI.RemoteAddress", "");
    if (ptr != NULL && ptr[0] != '\0')
    {
        return std::string(ptr);
    }

    return std::string("");
}

void CgX::setParseHttpBodyManually(bool bFlag)
{
    m_bParseHttpBodyManually = bFlag;
}

const std::string CgX::getHttpBody()
{
    CHECK_INIT("");
    return m_sHttpBody;
}