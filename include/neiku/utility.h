// vim:ts=4:sw=4:expandtab

/*
 * file:   neiku/utility.h
 * desc:   实用工具
 * author: YIF
 * date:   2017-04-11
 */

#ifndef __NEIKU_UTILITY_H__
#define __NEIKU_UTILITY_H__

#ifndef __STDC_FORMAT_MACROS
    #define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>

namespace neiku
{
// 获取微秒时间戳
uint64_t timeus();

// 获取毫秒时间戳
uint64_t timems();

// 秒级时间戳转字符串
std::string timestr(time_t sec, const char* fmt);

// 字符串转称级时间戳
time_t timestamp(const char* str, const char * fmt);

void _to(const char* from, int32_t& to);
void _to(const char* from, int64_t& to);
void _to(const char* from, uint32_t& to);
void _to(const char* from, uint64_t& to);
void _to(const char* from, float& to);
void _to(const char* from, double& to);
void _to(const std::string& from, int32_t& to);
void _to(const std::string& from, int64_t& to);
void _to(const std::string& from, uint32_t& to);
void _to(const std::string& from, uint64_t& to);
void _to(const std::string& from, float& to);
void _to(const std::string& from, double& to);
template<typename F>
void _to(const F& from, std::string& to)
{
    std::stringstream ss;
    ss << from;
    to = ss.str();
}

// 字符串/数值互转
// 示例：std::string("0x123") ==> uint64_t(291)
//       uint64_t(1234567890) ==> std::string("1234567890")    
#define _TO(FROM) template<typename TO> TO to(FROM from) { TO to; _to(from, to); return to; }
_TO(const char*);
_TO(const std::string&);
_TO(int32_t);
_TO(int64_t);
_TO(uint32_t);
_TO(uint64_t);
_TO(float);
_TO(double);

// 拆解字符串，得到std::vector<T>
// 示例：1|2|8 ==> [1, 2, 8]
template<typename T>
void explode(std::vector<T>& list, const std::string& str
             , const char sep = '|', const bool drop_empty_sec = true)
{
    std::string::size_type beg = 0;
    std::string::size_type end = 0;
    while (end != std::string::npos)
    {
        std::string sec;

        end = str.find_first_of(sep, beg);
        if (end != std::string::npos)
        {
            sec = str.substr(beg, (end - beg));
        }
        else
        {
            sec = str.substr(beg, std::string::npos);
        }
        
        if (!sec.empty() || !drop_empty_sec)
        {
            list.push_back(to<T>(sec));
        }
        beg = end + 1;
    }
}

// 拼接字符串，得到std::string
// 示例：[1, 2, 3]  ==> 1|2|3
template <typename T>
void implode(std::string& str, const std::vector<T>& list, const char sep = '|')
{
    str = "";
    for (typename std::vector<T>::const_iterator it = list.begin();
         it != list.end();
         ++it)
    {
        str.append(to<std::string>(*it)).append(1, sep);
    }
    if (!str.empty())
    {
        str.resize(str.size() - 1);
    }
}

// 字符串/比特位互转
// 示例：1|2|4    ==> 0b1011
//       0b110011 ==> 1|2|5|6
std::string bit2str(uint32_t bits, char sep = '|');
std::string bit2str(uint64_t bits, char sep = '|');
uint64_t str2bit(const std::string& str, char sep = '|');

// 获取网卡IPv4地址字符串（默认eth1即本机ip）
std::string localIp(const char* eth_name = "eth1");

// 转换字符串字符编码
// u2g：utf-8编码转gb18030
// g2u：gb18030编码转utf-8（兼容gbk/gb2312）
// convert_charset：任意字符集编码之间转换
std::string u2g(const std::string& utf8);
std::string g2u(const std::string& gb18030);
int convert_charset(const char* from_charset, const char* to_charset
                    , const std::string& from_string, std::string& to_string);

// URL编码/解码
// rawurlencode与urlencode的区别：前者将空格编码为%20，后者将空格编码为+
std::string rawurlencode(const std::string& str);
std::string rawurldecode(const std::string& str);
std::string urlencode(const std::string& str);
std::string urldecode(const std::string& str);

// Base64编码/解码
std::string base64encode(const std::string& str);
std::string base64decode(const std::string& str);

// 读取/保存文件数据
std::string cat(const std::string& filename);
std::string cat(FILE* fp);
int64_t filesize(FILE* fp);
int tee(const std::string& data, const std::string& filename);

// MD5摘要
std::string md5sum(const std::string& data);

// SHA1摘要
std::string sha1sum(const std::string& data);

// 转换字母大小写
std::string toupper(const std::string& str);
std::string tolower(const std::string& str);

// 计算字符串的字符个数，注意仅支持utf-8编码
// https://en.wikipedia.org/wiki/UTF-8
size_t charlen(const std::string& str);

// 字符串trim
std::string& trimleft(std::string& str);
std::string& trimright(std::string& str);
std::string& trim(std::string& sStr);

};

#endif

