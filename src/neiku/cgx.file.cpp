// vim:ts=4:sw=4:expandtab

#include <cstdio>
#include "neiku/cgx.h"

#define CHECK_INIT(RET) if (!m_bInit && !init()) { setErrMsg("cgx init fail"); return RET; }

FILE* neiku::CgX::getFile(const char* szName)
{
    CHECK_INIT(NULL);
    return cgi_filehandle(m_pCGI, szName);
}