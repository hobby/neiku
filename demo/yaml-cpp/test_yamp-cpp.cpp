#include "yaml-cpp/yaml.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "neiku/serialize.h"
#include "neiku/json_serialize.h"

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

#define PARSE_INTEGER(TYPE) \
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

    PARSE_INTEGER(uint32_t);
    PARSE_INTEGER(uint64_t);
    PARSE_INTEGER(int32_t);
    PARSE_INTEGER(int64_t);
    PARSE_INTEGER(float);
    PARSE_INTEGER(double);

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

// our data types
struct Vec3 {
   int x, y, z;

   template<typename AR>
   AR& serialize(AR& ar)
   {
       SERIALIZE(ar, x);
       SERIALIZE(ar, y);
       SERIALIZE(ar, z);

       return ar;
   }
};

struct Power {
   std::string name;
   int damage;

   template<typename AR>
   AR& serialize(AR& ar)
   {
       SERIALIZE(ar, name);
       SERIALIZE(ar, damage);

       return ar;
   }
};

struct Monster {
   std::string name;
   Vec3 position;
   std::vector <Power> powers;
   uint32_t qq;

   template<typename AR>
   AR& serialize(AR& ar)
   {
       SERIALIZE(ar, name);
       SERIALIZE(ar, qq);
       SERIALIZE(ar, position);
       SERIALIZE(ar, powers);

       return ar;
   }
};

// now the extraction operators for these types
void operator >> (const YAML::Node& node, Vec3& v) {
   node[0] >> v.x;
   node[1] >> v.y;
   node[2] >> v.z;
}

void operator >> (const YAML::Node& node, Power& power) {
   node["name"] >> power.name;
   node["damage"] >> power.damage;
}

void operator >> (const YAML::Node& node, Monster& monster) {
   node["name"] >> monster.name;
   node["position"] >> monster.position;
   const YAML::Node& powers = node["powers"];
   for(unsigned i=0;i<powers.size();i++) {
      Power power;
      powers[i] >> power;
      monster.powers.push_back(power);
   }
}

int main(int argc, char* argv[])
{
    // std::ifstream fin("monsters.yaml");
    // YAML::Parser parser(fin);
    // YAML::Node doc;
    // parser.GetNextDocument(doc);
    // for (unsigned i = 0; i < doc.size(); i++)
    // {
    //     Monster monster;
    //     doc[i] >> monster;
    //     std::cout << monster.name << "\n";
    // }

    neiku::YamlParser parser("monsters.yaml");
    if (!parser)
    {
        std::cout << "parser fail" << std::endl;
        return -1;
    }

    // Monster monster;
    // parser >> monster;
    // std::cout << monster.name << std::endl;
    // std::cout << monster.qq << std::endl;

    std::vector<Monster> monsters;
    parser >> monsters;
    std::cout << monsters.size() << std::endl;
    for (size_t i = 0; i < monsters.size(); ++i)
    {
        std::cout << monsters[i].name << std::endl;
    }
    std::cout << json_encode_ml(monsters) << std::endl;
    return 0;
}
