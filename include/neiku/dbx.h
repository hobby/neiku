// vim:ts=4:sw=4:expandtab

/*
 * file:   neiku/dbx.h
 * desc:   mysql c api简单封装(非线程安全)，方便业务操作db
 * author: YIF
 * date:   2017-04-03
 */

#ifndef __NEIKU_DBX__
#define __NEIKU_DBX__

#ifndef __STDC_FORMAT_MACROS
    #define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include <string>
#include <vector>

#include "mysql.h"
#include "neiku/sqlbind.h"

// 单例：一个进程一个实例
#include "neiku/singleton.h"
#define DBX  SINGLETON(neiku::DbX)

namespace neiku
{

class DbResult;

class DbX
{
public:
    DbX();
    ~DbX();

    int init(const std::string& host, uint32_t port
             , const std::string& user, const std::string& pass
             , const std::string& name, const std::string& cset);

    int errno();
    const char* what();

public:
    int query(const SqlBind& sql);

    int query(const std::string& sql);

    template<typename T>
    int query(const SqlBind& sql, T& obj)
    {
        std::string ssql = sql.getSql(this);
        int ret = doQuery(ssql);
        if (m_pResult)
        {
            DbResult res(m_pResult);
            res & obj;
            mysql_free_result(m_pResult);
            m_pResult = NULL;
        }
        return ret;
    }

    template<typename T>
    int query(const std::string& sql, T& obj)
    {
        int ret = doQuery(sql);
        if (m_pResult)
        {
            DbResult res(m_pResult);
            res & obj;
            mysql_free_result(m_pResult);
            m_pResult = NULL;
        }
        return ret;
    }

    std::string escape(const std::string& str);

    const char* status();
    uint64_t getConnectionId();
    uint64_t getLastInsertId();

public:
    int begin();
    int commit();
    int rollback();

private:
    int doQuery(const std::string& sql);

private:
    MYSQL *m_pMysql;
    MYSQL_RES *m_pResult;
    bool m_bInit;
    bool m_bAutoCommit;

    int m_iErrNo;
    std::string m_sWhat;
};

class DbResult
{
public:
    DbResult(MYSQL_RES* result);
    ~DbResult();

public:
    DbResult& operator & (Key& key)
    {
        return *this;
    }

    template<typename T>
    DbResult& operator & (T& obj)
    {
        return obj.serialize(*this);
    }
   
    template<typename T>
    DbResult& operator & (std::vector<T>& vec)
    {
        vec.clear();
        while (next())
        {
            T obj;
            *this & obj;
            vec.push_back(obj);
        }

        return *this;
    }

public:
    DbResult& operator & (uint32_t& val)
    {
        if (!next()) return *this;

        if (!m_pRow[m_ddwIdex])
        {
            val = 0;
        }
        else
        {
            val = strtoul(m_pRow[m_ddwIdex], NULL, 0);
        }
        m_ddwIdex += 1;
        return *this;
    }

    DbResult& operator & (uint64_t& val)
    {
        if (!next()) return *this;

        if (!m_pRow[m_ddwIdex])
        {
            val = 0;
        }
        else
        {
            val = strtoull(m_pRow[m_ddwIdex], NULL, 0);
        }
        m_ddwIdex += 1;
        return *this;
    }

    DbResult& operator & (int32_t& val)
    {
        if (!next()) return *this;

        if (!m_pRow[m_ddwIdex])
        {
            val = 0;
        }
        else
        {
            val = strtol(m_pRow[m_ddwIdex], NULL, 0);
        }
        m_ddwIdex += 1;
        return *this;
    }

    DbResult& operator & (int64_t& val)
    {
        if (!next()) return *this;

        if (!m_pRow[m_ddwIdex])
        {
            val = 0;
        }
        else
        {
            val = strtoll(m_pRow[m_ddwIdex], NULL, 0);
        }
        m_ddwIdex += 1;
        return *this;
    }

    DbResult& operator & (std::string& val)
    {
        if (!next()) return *this;

        if (!m_pRow[m_ddwIdex])
        {
            val = "";
        }
        else
        {
            val.assign(m_pRow[m_ddwIdex], m_pLen[m_ddwIdex]);
        }
        m_ddwIdex += 1;
        return *this;
    }

private:
    bool next();

private:
    MYSQL_RES* m_pResult;
    MYSQL_ROW  m_pRow;
    uint64_t*  m_pLen;
    uint64_t   m_ddwIdex;
    uint64_t   m_ddwCols;
};

};

#endif

