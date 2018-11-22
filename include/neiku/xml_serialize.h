// vim:ts=4:sw=4:expandtab

/*
 * file:   util/xml_serialize.h
 * desc:   基于TinyXML2的XML序列化工具（XML文本与内存对象互转）
 * author: YIF
 * date:   2017-08-12
 *
 * usage:  XmlParser - XML转内存对象，XML标签即字段名、XML标签内文本即字段值
 *         XmlDumper - 内存对象转XML，字段名即XML标签、字段值即XML标签内文本
 */

#ifndef __UTIL_XML_SERIALIZE__
#define __UTIL_XML_SERIALIZE__

#include "tinyxml2.h"
#include "neiku/serialize.h"

namespace neiku
{

class XmlParser
{
public:
    XmlParser(const char* filekey): _handle(NULL)
    {
        _doc.LoadFile(filekey);
        _handle = _doc.RootElement();
    }

    XmlParser(const char* data, int size): _handle(NULL)
    {
        _doc.Parse(data, size);
        _handle = _doc.RootElement();
    }

    bool operator ! ()
    {
        return _doc.Error();
    }

    const char* what()
    {
        snprintf(_what, sizeof(_what)
                , "error:[%d], msg1:[%s], msg2:[%s]"
                , _doc.ErrorID(), _doc.GetErrorStr1(), _doc.GetErrorStr2());
        return _what;
    }

public:
    template<typename OBJ>
    XmlParser& operator >> (OBJ& obj)
    {
        return obj.serialize(*this);
    }

    XmlParser& operator & (Key& key)
    {
        _key = key.c_str();
        return *this;
    }

#define PARSE_INTEGER(TYPE, PARSER) \
    XmlParser& operator & (TYPE& val) \
    { \
        tinyxml2::XMLElement* elm = NULL; \
        if (_key.empty()) \
        { \
            elm = _handle.ToElement(); \
        } \
        else \
        { \
            elm = _handle.LastChildElement(_key.c_str()).ToElement(); \
        } \
        val = elm && elm->GetText() ? PARSER(elm->GetText(), NULL, 0) : 0; \
        return *this; \
    }

    PARSE_INTEGER(int8_t, strtol)
    PARSE_INTEGER(int16_t, strtol)
    PARSE_INTEGER(int32_t, strtol)
    PARSE_INTEGER(int64_t, strtoll)
    PARSE_INTEGER(uint8_t, strtoul)
    PARSE_INTEGER(uint16_t, strtoul)
    PARSE_INTEGER(uint32_t, strtoul)
    PARSE_INTEGER(uint64_t, strtoull)

#define PARSE_FLOAT(TYPE, PARSER) \
    XmlParser& operator & (TYPE& val) \
    { \
        tinyxml2::XMLElement* elm = NULL; \
        if (_key.empty()) \
        { \
            elm = _handle.ToElement(); \
        } \
        else \
        { \
            elm = _handle.LastChildElement(_key.c_str()).ToElement(); \
        } \
        val = elm && elm->GetText() ? PARSER(elm->GetText(), NULL) : 0.0; \
        return *this; \
    }

    PARSE_FLOAT(float, strtof)
    PARSE_FLOAT(double, strtod)

    XmlParser& operator & (std::string& str)
    {
        tinyxml2::XMLElement* elm = NULL;
        if (_key.empty())
        {
            elm = _handle.ToElement();
        }
        else
        {
            elm = _handle.LastChildElement(_key.c_str()).ToElement();
        }
        str = elm && elm->GetText() ? elm->GetText() : "";
        return *this;
    }

    template<typename OBJ>
    XmlParser& operator & (std::vector<OBJ>& objs)
    {
        std::string key = _key;
        tinyxml2::XMLHandle child = NULL;
        if (key.empty())
        {
            child = _handle.FirstChild();
        }
        else
        {
            child = _handle.LastChildElement(key.c_str()).FirstChild();
        }
        _key = "";

        OBJ obj;
        tinyxml2::XMLHandle handle = NULL;
        while (child.ToNode())
        {
            if (child.ToElement())
            {
                handle = _handle;

                _handle = child;
                *this & obj;
                objs.push_back(obj);

                _handle = handle;
            }

            child = child.NextSibling();
        }

        _key = key;
        return *this;
    }

    template<typename OBJ>
    XmlParser& operator & (OBJ& obj)
    {
        tinyxml2::XMLHandle handle = _handle;
        if (_key.empty())
        {
            _handle = handle.ToElement();
        }
        else
        {
            _handle = handle.LastChildElement(_key.c_str());
        }

        std::string key = _key;
        _key = "";

        obj.serialize(*this);

        _key   = key;
        _handle = handle;
        return *this;
    }

private:
    tinyxml2::XMLDocument _doc;
    char                  _what[256];

private:
    std::string           _key;
    tinyxml2::XMLHandle   _handle;
};

template<typename F>
static std::string to_str(const F& from)
{
    std::stringstream ss;
    ss << from;
    return ss.str();
};

class XmlDumper
{
public:
    XmlDumper(const char* root = ""): _root(root)
    {}

    const char* c_str()
    {
        return _xml.c_str();
    }

    const std::string& str()
    {
        return _xml;
    }


    void reset(const char* root = "")
    {
        _key = "";
        _root = root;
        _xml  = "";
    }

public:
    template<typename OBJ>
    XmlDumper& operator << (OBJ& obj)
    {
        if (!_root.empty())
        {
            _xml.append("<").append(_root).append(">");
        }
        obj.serialize(*this);
        if (!_root.empty())
        {
            _xml.append("</").append(_root).append(">");
        }
        return *this;
    }

    XmlDumper& operator & (Key& key)
    {
        _key = key.c_str();
        return *this;
    }

#define DUMP_NUM(TYPE) \
    XmlDumper& operator & (TYPE val) \
    { \
        std::string key = _key; \
        if (key.empty()) \
        { \
            key = "unknow"; \
        } \
        _xml.append("<").append(key).append(">"); \
        _xml.append(to_str(val)); \
        _xml.append("</").append(key).append(">"); \
        return *this; \
    }

    DUMP_NUM(int32_t)
    DUMP_NUM(int64_t)
    DUMP_NUM(uint32_t)
    DUMP_NUM(uint64_t)

    XmlDumper& operator & (std::string& str)
    {
        std::string key = _key;
        if (key.empty())
        {
            key = "unknow";
        }
        _xml.append("<").append(key).append(">");;
        if (!str.empty())
        {
            _xml.append("<![CDATA[").append(str).append("]]>");
        }
        _xml.append("</").append(key).append(">");
        return *this;
    }

    template<typename OBJ>
    XmlDumper& operator & (std::vector<OBJ>& objs)
    {
        std::string key = _key; _key = "";
        if (!key.empty())
        {
            _xml.append("<").append(key).append(">");;
        }
        for (typename std::vector<OBJ>::iterator it = objs.begin();
             it != objs.end();
             ++it)
        {
            _xml.append("<").append("unknow").append(">");
            *this & *it;
            _xml.append("</").append("unknow").append(">");
        }
        if (!key.empty())
        {
            _xml.append("</").append(key).append(">");
        }
        _key = key;
        return *this;
    }

    template<typename OBJ>
    XmlDumper& operator & (OBJ& obj)
    {
        std::string key = _key; _key = "";
        if (!key.empty())
        {
            _xml.append("<").append(key).append(">");;
        }
        obj.serialize(*this);
        if (!key.empty())
        {
            _xml.append("</").append(key).append(">");
        }
        _key = key;
        return *this;
    }

private:
    std::string _xml;
    std::string _key;
    std::string _root;
};

};

#if 0
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("usage: %s <filename>\n", argv[0]);
        return 0;
    }

    Ojbect obj;
    XmlParser parser(argv[1]);
    if (!parser)
    {
        printf("parse xml fail, msg:[%s]\n", parser.what());
        return -1;
    }
    parser >> obj;

    XmlDumper dumper("xml");
    dumper << obj;
    printf("obj:[%s]", dumper.c_str());
    return 0;
}
#endif

#endif
