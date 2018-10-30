
/*
 * file:   neiku/json.h
 * desc:   json tools for C++
 * author: hobby
 * date:   2015/05/30 22:49:00
 *
 * version: v1  -- json encoder
 *              基于C++的重载、宏技术，简单实现C++对象自动转成json字符串
 *              简单支持int32/64,uint32/64,std::string,std::vector
 *          v2  -- json encoder
 *              支持std::map，生成json对象；std::vector生成json数组
 *              新增json_encode(obj)宏，方便使用dump数据
 *          v3  -- json encoder
 *              引入jsoncpp第三方库，用于支持json字符串转义输出、后续
 *              json decoder实现基础
 *              TODO: 后续使用std::string来代替std::stringstream
 *          v4  将jsoncpp库放到3rd下面
 *          v5  -- json encoder
 *              支持json格式化(添加换行、缩进)，提高可读性
 *          v6  -- json encoder
 *              支持bool类型
 *          v7  -- json encoder
 *              区分单行json和格式化json生成工具，不要撸在一起
 *          v8  -- json decoder
 *              基于jsoncpp、重载、宏技术，简单实现json字符串自动转成C++对象
 *　　　　　　　支持对象(嵌套)、bool/int32/64,uint32/64,std::string
 *          v8  -- json decoder
 *              支持std::vector<std::string>，对json的字符串数组
 * usage:
 *       #include "neiku/json.h"
 *
 * compile: $(neiku_config --include --libs json)
 *
 * vim:ts=4;sw=4;expandtab
 */

#ifndef __NK_JSON_H__
#define __NK_JSON_H__ 1

#include <inttypes.h>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include "jsoncpp/jsoncpp.h"
#include "neiku/serialize.h"

#define json_encode(obj)      ((neiku::CJsonEncoder() << obj).str().c_str())
#define json_encode_ml(obj)   ((neiku::CJsonEncoderMl() << obj).str().c_str())

namespace neiku
{

// JsonEncoder -- 支持C++对象自动生成JSON对象（字符串）
class CJsonEncoder
{
    public:
        CJsonEncoder(): m_bIsFirstKey(false)
        {};

        // 返回json
        std::string str()
        {
            return m_ssJson.str();
        }

        // json encode
        template <class T>
        CJsonEncoder& operator << (const T& o)
        {
            return *this & o;
        }

	public:
        CJsonEncoder& operator & (Key& key)
        {
            if (m_bIsFirstKey)
            {
                m_bIsFirstKey = false;
                m_ssJson << "\"" << key.c_str()<< "\":";
            }
            else
            {
                m_ssJson << ",\"" << key.c_str() << "\":";
            }
            return *this;
        }

        CJsonEncoder& operator & (bool b)
        {
            if (b == true)
            {
                m_ssJson << "true";
            }
            else
            {
                m_ssJson << "false";
            }
            return *this;
        }

        CJsonEncoder& operator & (int32_t i32)
        {
            m_ssJson << i32;
            return *this;
        }

        CJsonEncoder& operator & (int64_t i64)
        {
            m_ssJson << i64;
            return *this;
        }

        CJsonEncoder& operator & (uint32_t u32)
        {
            m_ssJson << u32;
            return *this;
        }

        CJsonEncoder& operator & (uint64_t u64)
        {
            m_ssJson << u64;
            return *this;
        }

        // 字符串两边由对引号包住
        CJsonEncoder& operator & (std::string& str)
        {
             m_ssJson << Json::valueToQuotedString(str.c_str());
            return *this;
        }

        // vector 视为 json 数组
        template <class T>
        CJsonEncoder& operator & (std::vector<T>& vector)
        {
            m_ssJson << "[";
            if (!vector.empty())
            {
                size_t index = 0;
                *this & vector[index];
                for (++index; index < vector.size(); ++index)
                {
                    m_ssJson << ",";
                    *this & vector[index];
                }
            }
            m_ssJson << "]";
            return *this;
        }

        // map 视为 json 对象
        template <class KEY, class VALUE>
        CJsonEncoder& operator & (std::map<KEY, VALUE>& map)
        {
            m_ssJson << "{";
            if (!map.empty())
            {
                typename std::map<KEY, VALUE>::iterator it = map.begin();
                std::stringstream ss;
                ss << it->first;
                m_ssJson << Json::valueToQuotedString(ss.str().c_str()) << ":";
                *this & it->second;
                for (++it; it != map.end(); ++it)
                {
                    // std::stringstream::str("") 才是真正的清空方法
                    ss.str("");
                    ss << it->first;
                    m_ssJson << "," << Json::valueToQuotedString(ss.str().c_str()) << ":";
                    *this & it->second;
                }
            }
            m_ssJson << "}";
            return *this;
        }

        // 自定义对象 视为 json 对象
        template <class T>
        CJsonEncoder& operator & (T& o)
        {
            bool b = m_bIsFirstKey;
            m_bIsFirstKey = true;
            m_ssJson << "{";
            o.serialize(*this);
            m_ssJson << "}";
            m_bIsFirstKey = b;
            return *this;
        }
        template <class T>
        CJsonEncoder& operator & (const T& o)
        {
            // 兼容const obj
            return *this & const_cast<T&>(o);
        }

    private:
        bool m_bIsFirstKey;
        std::stringstream m_ssJson;
};

// JsonEncoderMl -- 支持C++对象自动生成格式化JSON对象（自动填充换行、缩进）
class CJsonEncoderMl
{
    public:
        CJsonEncoderMl(): m_bIsFirstKey(false)
                        , m_iIndentNum(0)
        {};

        // 返回json
        std::string str()
        {
            return m_ssJson.str();
        }

        // json encode
        template <class T>
        CJsonEncoderMl& operator << (const T& o)
        {
            return *this & o;
        }

    public:
        CJsonEncoderMl& operator & (Key& key)
        {
            if (m_bIsFirstKey)
            {
                m_bIsFirstKey = false;
                m_ssJson << "\n" << Indent() << "\"" << key.c_str() << "\": ";
            }
            else
            {
                m_ssJson << "," << "\n" << Indent() << "\"" << key.c_str() << "\": ";
            }
            return *this;
        }

        CJsonEncoderMl& operator & (bool b)
        {
            if (b == true)
            {
                m_ssJson << "true";
            }
            else
            {
                m_ssJson << "false";
            }
            return *this;
        }

        CJsonEncoderMl& operator & (int32_t i32)
        {
            m_ssJson << i32;
            return *this;
        }

        CJsonEncoderMl& operator & (int64_t i64)
        {
            m_ssJson << i64;
            return *this;
        }

        CJsonEncoderMl& operator & (uint32_t u32)
        {
            m_ssJson << u32;
            return *this;
        }

        CJsonEncoderMl& operator & (uint64_t u64)
        {
            m_ssJson << u64;
            return *this;
        }

        // 字符串两边由对引号包住
        CJsonEncoderMl& operator & (std::string& str)
        {
             m_ssJson << Json::valueToQuotedString(str.c_str());
            return *this;
        }

        // vector 视为 json 数组
        template <class T>
        CJsonEncoderMl& operator & (std::vector<T>& vector)
        {
            m_ssJson << "[";
            if (!vector.empty())
            {
                int i  = m_iIndentNum;
                m_iIndentNum += 1;
                m_ssJson << "\n" << Indent();
                size_t index = 0;
                *this & vector[index];
                for (++index; index < vector.size(); ++index)
                {
                    m_ssJson << "," << "\n" << Indent();
                    *this & vector[index];
                }
                m_iIndentNum -= 1;
                m_ssJson << "\n" << Indent();
                m_iIndentNum = i;
            }
            m_ssJson << "]";
            return *this;
        }

        // map 视为 json 对象
        template <class KEY, class VALUE>
        CJsonEncoderMl& operator & (std::map<KEY, VALUE>& map)
        {
            m_ssJson << "{";
            if (!map.empty())
            {
                int i  = m_iIndentNum;
                m_iIndentNum += 1;
                typename std::map<KEY, VALUE>::iterator it = map.begin();
                std::stringstream ss;
                ss << it->first;
                m_ssJson << "\n" << Indent() << Json::valueToQuotedString(ss.str().c_str()) << ": ";
                *this & it->second;
                for (++it; it != map.end(); ++it)
                {
                    // std::stringstream::str("") 才是真正的清空方法
                    ss.str("");
                    ss << it->first;
                    m_ssJson << "," << "\n" << Indent() << Json::valueToQuotedString(ss.str().c_str()) << ": ";
                    *this & it->second;
                }
                m_iIndentNum -= 1;
                m_ssJson << "\n" << Indent();
                m_iIndentNum  = i;
            }
            m_ssJson << "}";
            return *this;
        }

        // 自定义对象 视为 json 对象
        template <class T>
        CJsonEncoderMl& operator & (T& o)
        {
            bool b = m_bIsFirstKey;
            int i  = m_iIndentNum;
            m_bIsFirstKey = true;
            m_ssJson << "{";
            m_iIndentNum += 1;
            o.serialize(*this);
            m_iIndentNum -= 1;
            m_ssJson << "\n" << Indent() << "}";
            m_bIsFirstKey = b;
            m_iIndentNum  = i;
            return *this;
        }
        template <class T>
        CJsonEncoderMl& operator & (const T& o)
        {
            // 兼容const obj
            return *this & const_cast<T&>(o);
        }

    private:
        std::string Indent()
        {
            return std::string(4 * (m_iIndentNum), ' ');
        }

    private:
        bool m_bIsFirstKey;
        std::stringstream m_ssJson;
        int  m_iIndentNum;
};

// JsonDecoder -- 支持JSON字符串向C++对象反序列化
class CJsonDecoder
{
    public:
        CJsonDecoder(): m_pCurrJsonValue(NULL)
                      , m_szKeyName(NULL)
        {}

        int Parse(const std::string& sJson)
        {
            Json::Reader reader;
            if (!reader.parse(sJson.c_str(), m_oJsonValueRoot))
            {
                #ifdef _JSON_SERIALIZE_DEBUG_
                LOG_ERR("parse json fail, json:[%s]", sJson.c_str());
                #endif
                return -1;
            }
            if (!m_oJsonValueRoot.isObject() && !m_oJsonValueRoot.isArray())
            {
                #ifdef _JSON_SERIALIZE_DEBUG_
                LOG_ERR("invalid object/array, json:[%s]", sJson.c_str());
                #endif
                return -1;
            }
            m_pCurrJsonValue = &m_oJsonValueRoot;
            return 0;
        }

        // json decode
        template <class T>
        CJsonDecoder& operator >> (T& o)
        {
            return *this & o;
        }

    public:
        CJsonDecoder& operator & (Key& key)
        {
            m_szKeyName = key.c_str();
            return *this;
        }

#define GEN_OPERATOR(type, method) \
        CJsonDecoder& operator & (type& o) \
        { \
            if (m_pCurrJsonValue->isMember(m_szKeyName)) \
            { \
                const Json::Value& value = (*m_pCurrJsonValue)[m_szKeyName]; \
                o = value.method(); \
            } \
            return *this; \
        }
        GEN_OPERATOR(bool,        asBool);
        GEN_OPERATOR(int32_t,     asInt);
        GEN_OPERATOR(int64_t,     asInt64);
        GEN_OPERATOR(uint32_t,    asUInt);
        GEN_OPERATOR(uint64_t,    asUInt64);
        GEN_OPERATOR(std::string, asString);

        //template <class T>
        CJsonDecoder& operator & (std::vector<std::string>& v)
        {
            if (!m_pCurrJsonValue->isMember(m_szKeyName))
            {
                return *this;
            }

            const Json::Value& value = (*m_pCurrJsonValue)[m_szKeyName];
            if (!value.isArray())
            {
                return *this;
            }

            for (Json::ArrayIndex i = 0; i < value.size(); ++i)
            {
                // TODO: 支持任意对象
                std::string s = value[i].asString();
                v.push_back(s);
            }

            return *this;
        }

        template <class T>
        CJsonDecoder& operator & (T& o)
        {
            Json::Value* pJsonValue = m_pCurrJsonValue;
            if (m_szKeyName != NULL)
            {
                const Json::Value& obj = (*pJsonValue)[m_szKeyName];
                m_pCurrJsonValue = const_cast<Json::Value*>(&obj);
                o.serialize(*this);
            }
            else
            {
                o.serialize(*this);
            }
            m_pCurrJsonValue = pJsonValue;
            return *this;
        }

    private:
        Json::Value  m_oJsonValueRoot;
        Json::Value* m_pCurrJsonValue;
        const char*  m_szKeyName;
};
};

#endif
