
#include "neiku/log.h"
#include "neiku/json.h"

class CObj
{
    public:
        int i;
        int j;
        int k;
        bool b_true;
        bool b_false;

        std::vector<std::string> vt;
        std::map<uint32_t, std::string> map;

        CObj(): i(100), j(200), k(300), b_true(true), b_false(false)
        {};

        template <class AR>
        AR& serialize(AR& ar) const
        {
            SERIALIZE(ar, i);
            SERIALIZE(ar, j);
            SERIALIZE(ar, vt);
            SERIALIZE(ar, k);
            SERIALIZE(ar, map);
            SERIALIZE(ar, b_true);
            SERIALIZE(ar, b_false);
            return ar;
        };
};

class CObject
{
    public:
        int id;
        std::string name;
        int id2;

        CObj o1;
        int id3;
        CObj o2;

        int32_t i32;
        int64_t i64;
        uint32_t u32;
        uint64_t u64;

        std::vector<int32_t> vi32;
        std::vector<std::string> vt;

        std::string const_name;
        std::map<uint32_t, std::string> map;
        
    public:
        CObject(): id(0), id2(2), id3(3), i32(-1), i64(-1), u32(0xfffffff), u64(123)
                 , const_name("const name")
        {};

        template <class AR>
        AR& serialize(AR& ar)
        {
            SERIALIZE(ar, id);
            SERIALIZE(ar, name);
            SERIALIZE(ar, id2);
            SERIALIZE(ar, o1);
            SERIALIZE(ar, id3);
            SERIALIZE(ar, o2);
            SERIALIZE(ar, i32);
            SERIALIZE(ar, i64);
            SERIALIZE(ar, u32);
            SERIALIZE(ar, u64);
            SERIALIZE(ar, vi32);
            SERIALIZE(ar, vt);
            SERIALIZE(ar, const_name);
            SERIALIZE(ar, map);
            return ar;
        }
};

class CData
{
public:
    int32_t id2;
    std::string name2;
    bool b;

    CData(): id2(0), b(false)
    {}

    template <typename AR>
    AR& serialize(AR& ar)
    {
        SERIALIZE(ar, id2);
        SERIALIZE(ar, name2);
        SERIALIZE(ar, b);
        return ar;
    }
};

class CPerson
{
public:
    int32_t id;
    std::string name;
    CData data;

    CPerson(): id(0)
    {}

    template <typename AR>
    AR& serialize(AR& ar)
    {
        SERIALIZE(ar, id);
        SERIALIZE(ar, name);
        SERIALIZE(ar, data);
        return ar;
    }
};

int main(int argc, char* argv[])
{
    // 自定义对象
    CObject non_const_obj;
    non_const_obj.id   = 8281845;
    non_const_obj.name = "hobby";
    non_const_obj.vi32.push_back(1);
    non_const_obj.vi32.push_back(2);
    non_const_obj.vi32.push_back(3);
    non_const_obj.vt.push_back("string1");
    non_const_obj.vt.push_back("string\r\t\n\'\"/\\\f\b2");
    non_const_obj.map.insert(std::pair<uint32_t, std::string>(1, "number1"));
    non_const_obj.map.insert(std::pair<uint32_t, std::string>(2, "number2"));
    LOG_MSG("json_encode:[%s]", json_encode(non_const_obj));

    LOG_MSG("json_encode_ml:\n%s", json_encode_ml(non_const_obj));

    // 反序列化
    std::string sJson = "{\"id\":123456, \"name\":\"\'hobby\'\",\"data\":{\"id2\":98765,\"name2\":\"哈哈\",\"b\":true}}";
    neiku::CJsonDecoder oDecoder;
    if (oDecoder.Parse(sJson) != 0)
    {
        return -1;
    }

    CPerson person;
    oDecoder >> person;
    LOG_MSG("person:\n%s", json_encode_ml(person));
    return 0;
}
