
#include <neiku/log.h>

void testv4()
{
    int pid = getpid();
    LOG_MSG("LOG_MSG meta with pid:[%d], file:[%s], line:[%d], function:[%s]"
        , pid, __FILE__, __LINE__, __FUNCTION__);
}

int main(int argc, char* argv[])
{
    LOG_MSG("simple LOG_MSG here");
    LOG_MSG("this is second line");

    int version = 3;
    const char *name = "neiku::LOG_MSG v3";
    LOG_MSG("LOG_MSG v*printf/variadic macro, version:[%d], name:[%s]"
        , version, name);

    testv4();

    LOG_MSG("v5: no more gnu/posix basename");

    struct timeval tv;
    gettimeofday(&tv, NULL);
    LOG_MSG("v6: add date/time, tv.tv_sec:[%ld], tv.tv_usec:[%ld]"
        , tv.tv_sec, tv.tv_usec);
    struct tm * pTm = localtime(&tv.tv_sec);
    char* szAsctime = asctime(pTm);
    char* szCtime = ctime(&tv.tv_sec);
    char szTimeStr[64] = {0};
    size_t stSize = strftime(szTimeStr, sizeof(szTimeStr), "%F %T", pTm);
    snprintf(szTimeStr+stSize, sizeof(szTimeStr)-stSize, ".%ld", tv.tv_usec);
    LOG_MSG("sizeof(szTimeStr):[%lu], size:[%lu], now:[%ld], astime:[%s], ctime:[%s], strftime:[%s]"
        , sizeof(szTimeStr), stSize, tv.tv_sec, szAsctime, szCtime, szTimeStr);

    // snprintf中第二个参数，其实包括了'\0'
    char buffer[11];
    for(int i = 0; i < 10; i++) buffer[i] = '1';
    buffer[10] = 0;
    snprintf(buffer, 5, "%s", "1234");
    LOG_MSG("buffer:[%s]", buffer);

    LOG_MSG("LOG_MSG time:[%s]", LOG->GetTime());

    LOG->SetLogLevel(LOG->GetLogLevelDbg());
    LOG_MSG("v7: add log level, current level:[%d]", LOG->GetLogLevel());
    LOG_ERR("v7: this is err level");
    LOG_MSG("v7: this is msg level");
    LOG_DBG("v7: this is dbg level");
    return 0;
}
