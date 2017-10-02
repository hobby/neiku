
/*
 * file:   neiku/serialize.h
 * author: YIF
 * date:   2014/11/23 19:04:00
 * desc:   序列化工具，实现一种较通用、简单的序列化技术
 *         支持字节流(反)序列化、JSON序列化、XML序列化、DB数据序列化
 *
 * 对象转字符串: ObjDumper
 * 转换后字符串：{key}:[{value}], {key}:[{value}]
 *
 * 对像转HDF: HdfDumper
 * 转换后字符串: {key} = {value} \n 或者 {key} { \n objkey1 = objval1 \n } \n
 */

#ifndef __SERIALIZE_H_
#define __SERIALIZE_H_

#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>

#define SERIALIZE(T, VAL)  { neiku::Key KEY(#VAL); T & KEY & VAL; };

namespace neiku
{

class Key
{
public:
    Key(const char* key): _key(key)
    {}

    const char* c_str()
    {
        return _key;
    };

public:
    const char* _key;
};

template <typename OBJ>
static std::string& implode(std::string& str
                            , const std::vector<OBJ>& list
                            , const char sep = '|')
{
    std::stringstream sstream;
    typename std::vector<OBJ>::const_iterator it = list.begin();
    if (it != list.end())
    {
        sstream << *it;
    }
    for (++it; it != list.end(); ++it)
    {
        sstream << sep << *it;
    }
    str = sstream.str();
    return str;
};

class ObjDumper
{
public:
    ObjDumper(): _keyname("")
    {};

    ObjDumper& operator & (Key& key)
    {
        _keyname = key.c_str();
        return *this;
    }

    ObjDumper& operator & (std::string& val)
    {
        _sstream << _postfix
                 << _keyname << ":[" << val << "]";
        if (_postfix.empty())
        {
            _postfix = ", ";
        }
        return *this;
    }

    ObjDumper& operator & (bool val)
    {
        _sstream << _postfix
                 << _keyname
                 << ":["
                 << (val ? "true" : "false")
                 << "]";
        if (_postfix.empty())
        {
            _postfix = ", ";
        }
        return *this;
    }

#define OBJDUMP_NUM(TYPE) \
    ObjDumper& operator & (TYPE val) \
    { \
        _sstream << _postfix \
                 << _keyname << ":[" << val << "]"; \
        if (_postfix.empty()) \
        { \
            _postfix = ", "; \
        } \
        return *this; \
    }

    OBJDUMP_NUM(uint32_t)
    OBJDUMP_NUM(uint64_t)
    OBJDUMP_NUM(int32_t)
    OBJDUMP_NUM(int64_t)

    template <typename OBJ>
    ObjDumper& operator & (std::vector<OBJ>& list)
    {
        std::string str;
        implode(str, list, ',');
        (*this) & str;
        return *this;
    }

    template <typename OBJ>
    ObjDumper& operator & (OBJ& obj)
    {
        obj.serialize(*this);
        return *this;
    }

    template <typename OBJ>
    ObjDumper& operator << (OBJ& obj)
    {
        (*this) & obj;
        return *this;
    }

    std::string str()
    {
        return _sstream.str();
    }

private:
    const char*       _keyname;
    std::string       _postfix;
    std::stringstream _sstream;
};

class HdfDumper
{
public:
    HdfDumper(const char* hdfname = ""): _keyname(hdfname)
    {};

    HdfDumper& operator & (Key& key)
    {
        _keyname = key.c_str();
        return *this;
    }

    std::string str()
    {
        return _sstream.str();
    }

public:

#define HDFDUMP_NUM(TYPE) \
    HdfDumper& operator & (TYPE val) \
    { \
        _sstream << _keyname << "=" << val << "\n"; \
        return *this; \
    }

    HDFDUMP_NUM(uint32_t)
    HDFDUMP_NUM(uint64_t)
    HDFDUMP_NUM(int32_t)
    HDFDUMP_NUM(int64_t)

    HdfDumper& operator & (std::string& val)
    {
        if (val.find_first_of('\n') != std::string::npos)
        {
            _sstream << _keyname << "<<EOM\n" << val << "\nEOM\n";
        }
        else
        {
            _sstream << _keyname << "=" << val << "\n";
        }
        return *this;
    }

    HdfDumper& operator & (bool val)
    {
        _sstream << _keyname << "=" << (val ? "true" : "false") << "\n";
        return *this;
    }

    template <typename OBJ>
    HdfDumper& operator & (std::vector<OBJ>& list)
    {
        if (list.empty())
        {
            return *this;
        }

        const char* keyname = _keyname;
        _sstream << _keyname << "{\n";

        uint32_t index = 0;
        typename std::vector<OBJ>::iterator it = list.begin();
        for (; it != list.end(); ++it)
        {
            _keyname = "";
            _sstream << index++;
            *this & *it;
        }

        _keyname = keyname;
        _sstream << "}\n";
        return *this;
    }

    template <typename OBJ>
    HdfDumper& operator & (OBJ& obj)
    {
        _sstream << _keyname << "{\n";
        obj.serialize(*this);
        _sstream << "}\n";
        return *this;
    }

    template <typename OBJ>
    HdfDumper& operator << (OBJ& obj)
    {
        (*this) & obj;
        return *this;
    }

private:
    const char*       _keyname;
    std::string       _postfix;
    std::stringstream _sstream;
};

};

#endif
