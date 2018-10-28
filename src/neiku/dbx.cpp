// vim:ts=4:sw=4:expandtab

#ifndef __STDC_FORMAT_MACROS
    #define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include <cstdlib>
#include <string>
#include <vector>

#include "mysql.h"
#include "errmsg.h"
#include "neiku/dbx.h"

#define CHECK_INIT(RET) if (!m_bInit) { m_sWhat = "dbx init fail"; return RET; }

using namespace neiku;

DbX::DbX(): m_pMysql(NULL)
          , m_pResult(NULL)
          , m_bInit(false)
          , m_bAutoCommit(true)
          , m_iErrNo(0)
{}

DbX::~DbX()
{
    if (m_bInit)
    {
        mysql_close(m_pMysql);
        m_pMysql = NULL;

        mysql_library_end();
        mysql_thread_end();
    }
    
    m_bInit = false;
}

int DbX::init(const std::string& host, uint32_t port
              , const std::string& user, const std::string& pass
              , const std::string& name, const std::string& cset)
{     
    if (m_bInit)
    {
        m_sWhat = "dbx already init";
        return -1;
    }

    // FIXME: mariadb api 5.5.x 以上会报错，临时去掉
    //if (mysql_thread_init())
    //{
    //    m_sWhat = "mysql thread init fail";
    //    return -1;
    //}

    if (mysql_library_init(0, NULL, NULL))
    {
        m_sWhat = "mysql library init fail";
        return -1;
    }

    m_pMysql = mysql_init(NULL);
    if (!m_pMysql)
    {
        m_sWhat = "mysql init fail";
        return -1;
    }

    // 连接超时阈值
    uint32_t maxsecs = 5;
    if (mysql_options(m_pMysql, MYSQL_OPT_CONNECT_TIMEOUT, &maxsecs))
    {
        m_sWhat  = "mysql options(connet_timeout) fail";
        m_sWhat += ", msg:[";
        m_sWhat += mysql_error(m_pMysql);
        m_sWhat += "]";
        m_iErrNo = mysql_errno(m_pMysql);

        mysql_close(m_pMysql);
        m_pMysql = NULL;
        mysql_library_end();
        mysql_thread_end();
        return -1;
    }

    // 自动重连
    my_bool reconnect = 1;
    if (mysql_options(m_pMysql, MYSQL_OPT_RECONNECT, &reconnect))
    {
        m_sWhat  = "mysql options(reconnect) fail";
        m_sWhat += ", msg:[";
        m_sWhat += mysql_error(m_pMysql);
        m_sWhat += "]";
        m_iErrNo = mysql_errno(m_pMysql);

        mysql_close(m_pMysql);
        m_pMysql = NULL;
        mysql_library_end();
        mysql_thread_end();
        return -1;
    }

    MYSQL* ptr = mysql_real_connect(m_pMysql
                                    , host.c_str(), user.c_str()
                                    , pass.c_str(), name.c_str()
                                    , port, NULL, 0);
    if (ptr != m_pMysql)
    {
        m_sWhat  = "mysql real connect fail";
        m_sWhat += ", msg:[";
        m_sWhat += mysql_error(m_pMysql);
        m_sWhat += "]";
        m_iErrNo = mysql_errno(m_pMysql);

        mysql_close(m_pMysql);
        m_pMysql = NULL;
        mysql_library_end();
        mysql_thread_end();
        return -1;
    }

    if (mysql_set_character_set(m_pMysql, cset.c_str()))
    {
        m_sWhat  = "mysql set character-set fail";
        m_sWhat += ", msg:[";
        m_sWhat += mysql_error(m_pMysql);
        m_sWhat += "]";
        m_iErrNo = mysql_errno(m_pMysql);

        mysql_close(m_pMysql);
        m_pMysql = NULL;
        mysql_library_end();
        mysql_thread_end();
        return -1;
    }

    m_bInit = true;
    return 0;
}

int DbX::doQuery(const std::string& sql)
{
    CHECK_INIT(-1);

    int ret = mysql_real_query(m_pMysql, sql.c_str(), sql.length());
    m_iErrNo = mysql_errno(m_pMysql);
    if (m_iErrNo == CR_SERVER_GONE_ERROR || m_iErrNo == CR_SERVER_LOST)
    {
        int tmp = mysql_ping(m_pMysql);
        if (tmp != 0)
        {
            m_iErrNo = mysql_errno(m_pMysql);
            m_sWhat += "mysql ping(auto-reconnect) fail";
            m_sWhat += ", msg:[";
            m_sWhat += mysql_error(m_pMysql);
            m_sWhat += "]";
            return -1;
        }

        // 重连后，事务被自动回滚，此时不能重试
        if (m_bAutoCommit)
        {
            ret = mysql_real_query(m_pMysql, sql.c_str(), sql.length());
        }
        else
        {
            m_iErrNo = 0;
            m_sWhat  = "connection gone, and transcation lost too";
            return -1;
        }
    }
    if (ret != 0)
    {
        m_iErrNo = mysql_errno(m_pMysql);

        m_sWhat  = "mysql real query fail";
        m_sWhat += ", msg:[";
        m_sWhat += mysql_error(m_pMysql);
        m_sWhat += "], sql:[";
        m_sWhat += sql;
        m_sWhat += "]";

        return -1;
    }

    m_pResult = mysql_store_result(m_pMysql);
    if (!m_pResult && (mysql_errno(m_pMysql) != 0))
    {
        m_sWhat  = "mysql store result fail";
        m_sWhat += ", msg:[";
        m_sWhat += mysql_error(m_pMysql);
        m_sWhat += "], sql:[";
        m_sWhat += sql;
        m_sWhat += "]";
        m_iErrNo = mysql_errno(m_pMysql);
        return -1;
    }

    return mysql_affected_rows(m_pMysql);
}

int DbX::query(const SqlBind& sql)
{
    std::string ssql = sql.getSql(this);
    int ret = doQuery(ssql);
    if (m_pResult)
    {
        mysql_free_result(m_pResult);
        m_pResult = NULL;
    }

    return ret;
}

int DbX::query(const std::string& sql)
{
    int ret = doQuery(sql);
    if (m_pResult)
    {
        mysql_free_result(m_pResult);
        m_pResult = NULL;
    }

    return ret;
}

std::string DbX::escape(const std::string& str)
{
    CHECK_INIT(std::string(""));

    char *buffer = new char[str.length() *2  + 1];
    mysql_real_escape_string(m_pMysql, buffer, str.c_str(), str.length());
    std::string escaped(buffer);
    delete []buffer;
    return escaped;
}

const char* DbX::status()
{
    CHECK_INIT("");

    const char* ptr = mysql_stat(m_pMysql);
    if (!ptr)
    {
        m_sWhat = "mysql get status fail";
        m_sWhat += ", msg:[";
        m_sWhat += mysql_error(m_pMysql);
        m_sWhat += "]";
        m_iErrNo = mysql_errno(m_pMysql);

        return "";
    }

    return ptr;
}

int DbX::errno()
{
    return m_iErrNo;
}

const char* DbX::what()

{
    return m_sWhat.c_str();
}

uint64_t DbX::getConnectionId()
{
    CHECK_INIT(0);

    uint64_t connectionId = 0;
    int ret = query(std::string("SELECT CONNECTION_ID()"), connectionId);
    if (ret < 0)
    {
        return 0;
    }
    return connectionId;
}

uint64_t DbX::getLastInsertId()
{
    CHECK_INIT(0);

    return uint64_t(mysql_insert_id(m_pMysql));
}

int DbX::begin()
{
    CHECK_INIT(-1);

    int ret = mysql_autocommit(m_pMysql, 0);
    if (ret != 0)
    {
        m_sWhat = "mysql autocommit fail";
        m_sWhat += ", msg:[";
        m_sWhat += mysql_error(m_pMysql);
        m_sWhat += "]";
        m_iErrNo = mysql_errno(m_pMysql);
        return -1;
    }
    m_bAutoCommit = false;
    return 0;
}

int DbX::commit()
{
    CHECK_INIT(-1);

    int ret = mysql_commit(m_pMysql);
    if (ret != 0)
    {
        m_sWhat = "mysql commit fail";
        m_sWhat += ", msg:[";
        m_sWhat += mysql_error(m_pMysql);
        m_sWhat += "]";
        m_iErrNo = mysql_errno(m_pMysql);
        return -1;
    }

    mysql_autocommit(m_pMysql, 1);
    m_bAutoCommit = true;
    return 0;
}

int DbX::rollback()
{
    CHECK_INIT(-1);

    int ret = mysql_rollback(m_pMysql);
    if (ret != 0)
    {
        m_sWhat = "mysql commit fail";
        m_sWhat += ", msg:[";
        m_sWhat += mysql_error(m_pMysql);
        m_sWhat += "]";
        m_iErrNo = mysql_errno(m_pMysql);
        return -1;
    }
    mysql_autocommit(m_pMysql, 1);
    m_bAutoCommit = true;
    return 0;
}

DbResult::DbResult(MYSQL_RES* result): m_pResult(result)
                                     , m_pRow(NULL)
                                     , m_pLen(NULL)
                                     , m_ddwIdex(0)
                                     , m_ddwCols(0)
{
    if (m_pResult != NULL)
    {
        m_ddwCols = mysql_num_fields(m_pResult);

        m_ddwIdex = m_ddwCols;
    }
}

DbResult::~DbResult()
{
}

bool DbResult::next()
{
    if (m_ddwCols <= m_ddwIdex)
    {
        m_pRow = mysql_fetch_row(m_pResult);
        if (!m_pRow)
        {
            return false;
        }

        m_pLen = mysql_fetch_lengths(m_pResult);
        if (!m_pLen)
        {
            return false;
        }

        m_ddwIdex = 0;
    }

    return true;
}

