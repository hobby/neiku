/*
 * file:   yaf/string.h
 * author: yifee
 * site:   http://haama.net
 * date:   2014/11/16 21:27:00
 * desc:
 * CString v1.1
 * (1) 基于模板/重裁技术，实现字符串拆词算法模板化，减少冗余的实现代码 
 *     (目前仅支持std::string/uint32_t/uint64_t)
 * (2) 输入参数能用const则用，能够提示使用者输入参数在接口实现内是只读的
 * (3) 统一分拆词与合并词分隔符类型，默认均为|符号
 * CString v1.0 - 收集字符串相关操作工具
 * (1) 字符串拆词
 * (2) 字符串去空白
 *
 *
 * 相关技术
 * (1) <ctype.h>/<cctype>中的int isspace(int c)，检查字符是否为空白字符，如空格、制表符、回车均是空白字符
 * (2) <algorithm>中的std::find_if，贪心的查找算法模板
 * (3) std::ptr_fun，将一元函数指针(地址)转成stl函数对象的利器，如std::ptr_fun(isspace)等价于:
 *     struct SIsSpace()
 *     {
 *         typedef int argument_type;
 *         int operator()(argument_type c)
 *         { return isspace(c); };
 *     };
 * (4) <functional>中的std::not1()，将函数对象返回结果取反后返回，本身是一个函数对象
 *     常用于stl算法模板中，如在字符串中找到第一个非空字符的迭代器:
 *     std::string::iterator it = std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun(isspace)));
 * (5) <stdlib.h>中的strtoul/strtoull()可统一将8/10/16进制数字串转成无符号数值(uint32_t/uint64_t)
 * (6) std::string中的find_first_of()/sutstr()/std::string::npos可实现字符串拆解(split)
 * (7) std::stringstream可用于数值转字符串的实现工具，简单实用
 */

#ifndef __YAF_STRING_H_
#define __YAF_STRING_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <sstream>

#include <neiku/singleton.h>
#define STRING SINGLETON(yaf::util::CString)

namespace yaf
{
namespace util
{

class CString
{
    public:
        /*********************** 字符串分解/合并 ************************/
        /* desc: 使用字符(cSep)分解字符串(sStr)，将得到的子串列表从左到右存到对象列表(vObj)中。
         *       当bDropEmptySec为真时，空(长度为0)的子串将被丢弃，否则依然保存到vStr中。
         * param:  vObj           -- 输出参数，存储数据列表的vector，目前支持std::string, uint32_t, uint64_t
         *         sStr           -- 输入参数，目标数据列表
         *         cSep           -- 输入参数，分隔符，默认|
         *         bDropEmptySec  -- 输入参数，表示是否丢弃空的子串
         * return: vObj的引用
         */
        template <typename object_t>
        static std::vector<object_t>& Explode(std::vector<object_t>& vObj
                                              , const std::string& sStr
                                              , const char cSep = '|'
                                              , const bool bDropEmptySec = true)
        {
            std::string::size_type iPosBeg = 0;
            std::string::size_type iPosEnd = 0;
            while (iPosEnd != std::string::npos)
            {
                std::string sSec;

                iPosEnd = sStr.find_first_of(cSep, iPosBeg);
                if (iPosEnd != std::string::npos)
                {
                    // 找到新的子串
                    sSec = sStr.substr(iPosBeg, (iPosEnd - iPosBeg));
                }
                else
                {
                    // 没有新的子串，则取剩余部分
                    sSec = sStr.substr(iPosBeg, std::string::npos);
                }
                
                if (!sSec.empty() || !bDropEmptySec)
                {
                    VectorPushBack(vObj, sSec);
                }
                iPosBeg = iPosEnd + 1;
            }

            return vObj;
        };

        /* desc: 使用字符(cSep)分并对象列表(vObj，如uint32_t/uint64_t)到字符串(sStr)中。
         * param   sStr           -- 输出参数，目标数据列表
         *         vObj           -- 输入参数，存储数据列表的vector，支持任意通过AllToString()的类型
         *         cSep           -- 输入参数，分隔符，默认|
         * return: sStr的引用
         */
        template <typename object_t>
        static std::string& Implode(std::string& sStr
                                    , const std::vector<object_t>& vObj
                                    , const char cSep = '|')
        {
            sStr = "";
            for (typename std::vector<object_t>::const_iterator it = vObj.begin();
                 it != vObj.end();
                 ++it)
            {
                sStr.append(AllToString(*it)).append(1, cSep);
            }
            if (!sStr.empty())
            {
                // 去掉最后一个cSep字符
                sStr.resize(sStr.size() - 1);
            }
            return sStr;
        };

        /*********************** 字符串去空白 ************************/
        /* desc: 去掉字符串(sStr)左边的空白(‘ ‘, '\n', '\t'等等)，见isspace()
         * param:  sStr -- 输入参数，目标字符串
         * return: 去掉左边空白字符的字符串
         */
        static std::string& TrimLeft(std::string& sStr)
        {
            // 从左到右找到第一个非空字符的位置(迭代器)
            std::string::iterator itNonSpace;
            itNonSpace = std::find_if(sStr.begin(), sStr.end()
                                      , not1(std::ptr_fun(isspace)));
            sStr.erase(sStr.begin(), itNonSpace);
            return sStr;
        };

        static std::string& TrimRight(std::string& sStr)
        {
            // 从右到左找到第一个非空字符的位置(迭代器)
            std::string::reverse_iterator itNonSpace;
            itNonSpace = std::find_if(sStr.rbegin(), sStr.rend()
                                      , std::not1(std::ptr_fun(isspace)));
            sStr.erase(itNonSpace.base(), sStr.end());
            return sStr;
        };

        static std::string& Trim(std::string& sStr)
        {
            // 去掉左边空白，再去掉右边空白
            return TrimRight(TrimLeft(sStr));
        };

        // 字符串转换器
        static const std::string& AllToString(const std::string& sStr)
        {
            return sStr;
        };
        template <typename object_t>
        static std::string AllToString(object_t oObj)
        {
            std::stringstream ss;
            ss << oObj;
            return ss.str();
        };

    private:
        // TODO: 独立到实现文件中，该头文件仅提供接口原型
        static void VectorPushBack(std::vector<std::string>& v, const std::string& s)
        {
            v.push_back(s);
        };
        static void VectorPushBack(std::vector<uint32_t>& v, const std::string& s)
        {
            v.push_back(strtoul(s.c_str(), NULL, 0));
        };
        static void VectorPushBack(std::vector<uint64_t>& v, const std::string& s)
        {
            v.push_back(strtoull(s.c_str(), NULL, 0));
        };
};

};
};

#endif
