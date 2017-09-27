
#include <vector>
#include "neiku/log.h"
#include "neiku/serialize.h"

using namespace neiku;

class CPerson
{
public:
    int32_t id;
    std::string name;
    std::string addr;
    std::string wechat;
    bool flag;

    std::vector<int> list;
    std::vector<std::string> strs;

public:
    CPerson(): id(0), flag(false)
    {}

    template <typename T>
    T& serialize(T& t)
    {
        SERIALIZE(t, id);
        SERIALIZE(t, name);
        SERIALIZE(t, addr);
        SERIALIZE(t, wechat);
        SERIALIZE(t, flag);
        SERIALIZE(t, list);
        SERIALIZE(t, strs);
        return t;
    }
};

int main(int argc, char* argv[])
{
    CPerson person;
    person.id = 123;
    person.name = "YIF";
    person.addr = "SZ";
    person.wechat = "WX2";
    person.flag = true;
    person.list.push_back(1);
    person.list.push_back(2);
    person.list.push_back(3);
    person.list.push_back(-1);
    person.strs.push_back("abc");
    person.strs.push_back("def");

    ObjDumper dumper;
    dumper << person;
    std::string str = dumper.str();
    LOG_MSG("str => %s", str.c_str());
    return 0;
}
