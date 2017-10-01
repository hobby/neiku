// vim:ts=4:sw=4:expandtab

#include "neiku/log.h"
#include "neiku/cgx.h"
#include "neiku/serialize.h"

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
    char* p = NULL;

    HDF *hdf = NULL;
    hdf_init(&hdf);

    hdf_write_string(hdf, &p);
    LOG_MSG("p:[%s]", p);

    //hdf_set_value(hdf, "set1.key1", "set1.key1:\r\n\nvalue");
    hdf_set_value(hdf, "set1.key2", "set1.key2:value");
    hdf_set_value(hdf, "set2.key1", "set2.key1:value");
    hdf_set_value(hdf, "set3.key1.1", "set3.key1.1:value");
    hdf_set_value(hdf, "set3.key1.2", "set3.key1.2:value");
    hdf_set_value(hdf, "set3.key1.3", "set3.key1.3:value");
    hdf_set_value(hdf, "name", "{中文}");
    hdf_write_string(hdf, &p);
    LOG_MSG("p:[%s]", p);

    STRING str3;
    string_init(&str3);
    hdf_dump_str(hdf, NULL, 1, &str3);
    LOG_MSG("str3:[%s]", str3.buf);

    //const char *str = "set{\nkey=value\n1=2\n}";
    const char *str = "key = value\nkey1=value2\n set{\nkey=value\n1=2\n}\n";
    if (str)
    {
        HDF *hdf1 = NULL;
        hdf_init(&hdf1);
        hdf_read_string_ignore(hdf1, str, 1);
        
        STRING str4;
        string_init(&str4);
        hdf_dump_str(hdf1, NULL, 1, &str4);
        LOG_MSG("str4:[%s]", str4.buf);
    }

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
    person.strs.push_back("def");

    Data data;
    // data.data = "data1";
    // person.datas.push_back(data);
    // data.data = "data2";
    // person.datas.push_back(data);
      
    HdfDumper dumper("data");
    dumper << person;
    HDF *hdf2 = NULL;
    hdf_init(&hdf2);
    hdf_read_string_ignore(hdf2, dumper.str().c_str(), 1);
    hdf_read_string_ignore(hdf2, str, 1);

    STRING str5;
    string_init(&str5);
    hdf_dump_str(hdf2, NULL, 1, &str5);
    LOG_MSG("str5:[%s], dumper:[%s]", str5.buf, dumper.str().c_str());

    return 0;
}
