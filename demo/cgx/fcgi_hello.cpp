// vim:ts=4:sw=4:expandtab

#include <unistd.h>
#include "neiku/log.h"
#include "neiku/cgx.h"

using namespace neiku;

class Data
{
public:
    std::string data;

public:
    template <typename T>
    T& serialize(T& t)
    {
        SERIALIZE(t, data);
        return t;
    }
};

class Person
{
public:
    int32_t id;
    std::string name;
    std::string addr;
    std::string wechat;
    bool flag;

    std::vector<int> list;
    std::vector<std::string> strs;
    std::vector<Data> datas;

public:
    Person(): id(0), flag(false)
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
        SERIALIZE(t, datas);
        return t;
    }
};

int main(int argc, char* argv[])
{
    CGX->main(argc, argv);

    Person person;
    person.id = 123;
    person.name = "YIF";
    person.addr = "SZ";
    person.wechat = "WX2";
    person.flag = false;
    person.list.push_back(1);
    person.list.push_back(2);
    person.list.push_back(3);
    person.list.push_back(-1);
    person.strs.push_back("abc");
    person.strs.push_back("d\r\t\n\nef");

    while (CGX->accept() >= 0)
    {
        CGX->setValue("key", CGX->getValue("Query.key", "default"));
        CGX->setValue("pid", getpid());
        CGX->setValue("person", person);
        int ret = CGX->render("fcgi_hello.cs");
        if (ret != 0)
        {
            LOG_MSG("render fail, red:[%d], msg:[%s]", ret, CGX->getErrMsg());
        }
    }

    return 0;
}
