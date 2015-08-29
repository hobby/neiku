
/*
 * file:   yaf/serialize.h
 * author: yifee
 * date:   2014/11/23 19:04:00
 * desc:   序列化工具，实现一种较通用、简单的序列化技术
 *         支持字节流(反)序列化、JSON序列化、XML序列化、DB数据序列化
 */

#ifndef __SERIALIZE_H_
#define __SERIALIZE_H_

#include <stdint.h>
#include <string>
#include <sstream>

#include <neiku/string.h>

#define SERIALIZE(AR, OBJ)  { neiku::CSerializeName oSerializeName(#OBJ); AR & oSerializeName & OBJ; };
#define OBJDUMP(OBJ)        ( neiku::CObjDumper() << OBJ).str().c_str()

namespace neiku
{

/*
template<T>
const char* cstr_encode(const T& t)
{
    return OBJDUMP(t);
}
*/

class CSerializeName
{
    public:
        CSerializeName(const char* pszName)
        {
            m_sName.assign(pszName);
        };

        std::string& GetName()
        {
            return m_sName;
        };

    public:
        std::string m_sName;
};

class CObjDumper
{
    public:
        CObjDumper()
        {};

        CObjDumper& operator & (CSerializeName& oName)
        {
            m_sStream << oName.GetName() << ":[";
            return *this;
        }

        CObjDumper& operator & (std::string& sStr)
        {
            m_sStream << sStr << "], ";
            return *this;
        }

        CObjDumper& operator & (bool bValue)
        {
            if (bValue == true)
            {
                m_sStream << "true], ";
            }
            else
            {
                m_sStream << "false], ";
            }
            return *this;
        }

        CObjDumper& operator & (uint32_t dwValue)
        {
            m_sStream << dwValue << "], ";
            return *this;
        }

        CObjDumper& operator & (uint64_t ddwValue)
        {
            m_sStream << ddwValue << "], ";
            return *this;
        }

        CObjDumper& operator & (int64_t lValue)
        {
            m_sStream << lValue << "], ";
            return *this;
        }

        CObjDumper& operator & (int32_t iValue)
        {
            m_sStream << iValue << "], ";
            return *this;
        }

        template <typename object_t>
        CObjDumper& operator & (std::vector<object_t>& vObj)
        {
            std::string sStr;
            STRING->Implode(sStr, vObj, ",");
            (*this) & sStr;
            return *this;
        }

        template <typename object_t>
        CObjDumper& operator & (object_t& obj)
        {
            obj.Serialize(*this);
            return *this;
        }
        
        template <typename object_t>
        CObjDumper& operator << (object_t& obj)
        {
            (*this) & obj;
            return *this;
        }

        std::string str()
        {
            std::string sStr = m_sStream.str();
            if (!sStr.empty())
            {
                sStr.resize(sStr.size() - 2);
            }
            return sStr;
        }

    private:
        std::stringstream m_sStream;
};

};

#endif
