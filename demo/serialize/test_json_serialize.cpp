
#include <inttypes.h>
#include <sstream>
#include <string>
#include <vector>
#include <neiku/log.h>

#define SERIALIZE(ar, obj) { const CKeyName oKeyName(#obj); ar & oKeyName & obj; }

class CKeyName
{
    public:
        CKeyName(const char* name): m_sKeyName(name)
        {};

        const char* c_str()
        {
            return m_sKeyName.c_str();
        }

    private:
        std::string m_sKeyName;
};

// 支持C++对象自动生成JSON对象（字符串）
// demo: sJson = json_encode(obj);
class CJsonEncoder
{
    public:
        CJsonEncoder(): m_bIsFirstKey(false)
        {};

        template <class T>
        CJsonEncoder& operator << (const T& o)
        {
            return *this & o;
        }

        CJsonEncoder& operator & (CKeyName& oKeyName)
        {
            if (m_bIsFirstKey)
            {
                m_bIsFirstKey = false;
                m_ssJson << "\"" << oKeyName.c_str() << "\":";
            }
            else
            {
                m_ssJson << ",\"" << oKeyName.c_str() << "\":";
            }
            return *this;
        }

        CJsonEncoder& operator & (int32_t i)
        {
            m_ssJson << i;
            return *this;
        }

        CJsonEncoder& operator & (int64_t ii)
        {
            m_ssJson << ii;
            return *this;
        }

        CJsonEncoder& operator & (uint32_t u)
        {
            m_ssJson << u;
            return *this;
        }

        CJsonEncoder& operator & (uint64_t uu)
        {
            m_ssJson << uu;
            return *this;
        }
 
        CJsonEncoder& operator & (std::string& s)
        {
             m_ssJson << "\"" << s << "\"";
            return *this;
        }

        template <class T>
        CJsonEncoder& operator & (std::vector<T>& v)
        {
            m_ssJson << "[";
            if (!v.empty())
            {
                size_t index = 0;
                m_ssJson << "\"" << index << "\":";
                *this & v[index];
                for (++index; index < v.size(); ++index)
                {
                    m_ssJson << "," << "\"" << index << "\":";
                    *this & v[index];
                }
            }
            m_ssJson << "]";
            return *this;
        }

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
        
        std::string str()
        {
            return m_ssJson.str();
        }

        const char* c_str()
        {
            return m_ssJson.str().c_str();
        }

    private:
        bool m_bIsFirstKey;
        std::stringstream m_ssJson;
};
template <class T>
std::string json_encode(const T& obj)
{
    CJsonEncoder oEncoder;
    oEncoder << obj;
    return oEncoder.str();
}

class CObj
{
    public:
        int i;
        int j;
        int k;

        std::vector<std::string> vt;

        CObj(): i(100), j(200), k(300)
        {};

        template <class AR>
        AR& serialize(AR& ar) const
        {
            SERIALIZE(ar, i);
            SERIALIZE(ar, j);
            SERIALIZE(ar, vt);
            SERIALIZE(ar, k);
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
            return ar;
        }
};

int main(int argc, char* argv[])
{
    const CObject const_obj;
    LOG_MSG("const_obj:[%s]", json_encode(const_obj).c_str());

    CObject non_const_obj;
    non_const_obj.id   = 8281845;
    non_const_obj.name = "hobby";
    non_const_obj.vi32.push_back(1);
    non_const_obj.vi32.push_back(2);
    non_const_obj.vi32.push_back(3);
    non_const_obj.vt.push_back("string1");
    non_const_obj.vt.push_back("string2");
    LOG_MSG("non_const_obj:[%s]", json_encode(non_const_obj).c_str());

    const CObject const_obj2;
    CJsonEncoder oEncoder;
    oEncoder << const_obj2;
    LOG_MSG("const_obj2:[%s]", oEncoder.c_str())
    return 0;
}
