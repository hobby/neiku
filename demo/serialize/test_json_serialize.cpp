
#include "neiku/log.h"
#include "neiku/json.h"

class CObj
{
    public:
        int i;
        int j;
        int k;

        std::vector<std::string> vt;
        std::map<uint32_t, std::string> map;

        CObj(): i(100), j(200), k(300)
        {};

        template <class AR>
        AR& serialize(AR& ar) const
        {
            SERIALIZE(ar, i);
            SERIALIZE(ar, j);
            SERIALIZE(ar, vt);
            SERIALIZE(ar, k);
            SERIALIZE(ar, map);
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
        CObject(): id(0), id2(2), id3(3), i32(-1), i64(-1), u32(0xffffffff), u64(0xffffffffffffffff)
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
    non_const_obj.vt.push_back("string2");
    non_const_obj.map.insert(std::pair<uint32_t, std::string>(1, "number1"));
    non_const_obj.map.insert(std::pair<uint32_t, std::string>(2, "number2"));
    LOG_MSG("non_const_obj:[%s]", json_encode(non_const_obj));

    // vector of uint32_t
    std::vector<uint32_t> vu;
    vu.push_back(1);
    vu.push_back(2);
    vu.push_back(3);
    LOG_MSG("vector of uint32_t => %s", json_encode(vu));

    // vector of int32_t
    std::vector<int32_t> vi;
    vi.push_back(-100);
    vi.push_back(-200);
    vi.push_back(-300);
    LOG_MSG("vector of int32_t => %s", json_encode(vi));

    // vector of std::string
    std::vector<std::string> vs;
    vs.push_back("hobby");
    vs.push_back("lisa");
    vs.push_back("json ecoder from neiku");
    LOG_MSG("vector of std::string => %s", json_encode(vs));

    // map of uint32t,std::string
    std::map<uint32_t, std::string> mu;
    mu.insert(std::pair<uint32_t, std::string>(1, "number1"));
    mu.insert(std::pair<uint32_t, std::string>(2, "number2"));
    LOG_MSG("map of uint32_t,std::string => %s", json_encode(mu));

    // map of std::string,std::string
    std::map<std::string, std::string> ms;
    ms.insert(std::pair<std::string, std::string>("project", "neiku"));
    ms.insert(std::pair<std::string, std::string>("mobule", "json"));
    ms.insert(std::pair<std::string, std::string>("submodule", "encoder"));
    LOG_MSG("map of std::string,std::string => %s", json_encode(ms));

    return 0;
}
