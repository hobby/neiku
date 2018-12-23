
/*
 * file:   neiku/yaml_serialize.h
 * desc:   yaml serialize tool for C++
 * author: YIF
 * date:   2015/05/30 22:49:00
 *
 * vim:ts=4;sw=4;expandtab
 */

#ifndef __NK_YAML_SERIALIZE_H__
#define __NK_YAML_SERIALIZE_H__ 1

#include <inttypes.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "neiku/serialize.h"
#include "yaml-cpp/yaml.h"

namespace neiku
{

class YamlParser
{
public:
    YamlParser(const char* filename): _good(false)
    {
        std::ifstream fin(filename);
        try
        {
            _parser.Load(fin);
            _good = _parser.GetNextDocument(_root);
            _current = &_root;
        }
        catch (...)
        {
            _good = false;
        }
    }

    bool operator ! ()
    {
        return !_good;
    }

    const char* what()
    {
        return _what.c_str();
    }

public:
    template<typename OBJ>
    YamlParser& operator >> (OBJ& obj)
    {
        return obj.serialize(*this);
    }

    YamlParser& operator & (Key& key)
    {
        _key = key.c_str();
        return *this;
    }

    YamlParser& operator & (bool& str)
    {
        // TODO:
        return *this;
    }

#define PARSE_INTEGER_YAML(TYPE) \
    YamlParser& operator & (TYPE& num) \
    { \
        YAML::Node* current = NULL; \
        if (_key.empty()) \
        { \
            current = _current; \
        } \
        else \
        { \
            current = const_cast<YAML::Node*>(_current->FindValue(_key.c_str())); \
        } \
        num = 0; \
        if (current != NULL) \
        { \
            *current >> num; \
        } \
        return *this; \
    }

    PARSE_INTEGER_YAML(uint32_t);
    PARSE_INTEGER_YAML(uint64_t);
    PARSE_INTEGER_YAML(int32_t);
    PARSE_INTEGER_YAML(int64_t);
    PARSE_INTEGER_YAML(float);
    PARSE_INTEGER_YAML(double);

    YamlParser& operator & (std::string& str)
    {
        YAML::Node* current = NULL;
        if (_key.empty())
        {
            current = _current;
        }
        else
        {
            current = const_cast<YAML::Node*>(_current->FindValue(_key.c_str()));
        }

        str = "";
        if (current != NULL)
        {
            *current >> str;
        }
        return *this;
    }

    template<typename OBJ>
    YamlParser& operator >> (std::vector<OBJ>& objs)
    {
        return *this & objs;
    }

    template<typename OBJ>
    YamlParser& operator & (std::vector<OBJ>& objs)
    {
        std::string key = _key;
        YAML::Node* current = _current;
        YAML::Node* child = NULL;
        if (key.empty())
        {
            child = current;
        }
        else
        {
            child = const_cast<YAML::Node*>(current->FindValue(_key.c_str()));
        }

        if (child->Type() == YAML::NodeType::Sequence)
        {
            for (YAML::Iterator it = child->begin(); it != child->end(); ++it)
            {
                _key = "";
                _current = const_cast<YAML::Node*>(&(*it));

                OBJ obj;
                *this & obj;
                objs.push_back(obj);
            }
        }

        _key = key;
        _current = current;
        return *this;
    }

    template<typename OBJ>
    YamlParser& operator & (OBJ& obj)
    {
        YAML::Node* current = _current;
        if (_key.empty())
        {
        }
        else
        {
            _current = const_cast<YAML::Node*>(current->FindValue(_key.c_str()));
        }

        std::string key = _key;
        _key = "";

        obj.serialize(*this);

        _key   = key;
        _current = current;
        return *this;
    }

private:
    YAML::Parser _parser;
    bool         _good;
    std::string  _what;

private:
    std::string _key;
    YAML::Node  _root;

    YAML::Node* _current;
};

};

#endif
