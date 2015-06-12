
/*
 * file:   neiku/charset.h
 * desc:   charset tools for C++
 * author: hobby
 * date:   2015/05/23 11:55:00
 *
 * usage:
 *       #include <neiku/charset.h>
 *
 *       std::string sGbk  = neiku::charset_u2g("utf-8 string");
 *       std::string sUtf8 = neiku::charset_g2u("gb2312/gbk/gb18030 string");
 *
 * compile: -lneiku
 *
 * vim:ts=4;sw=4;expandtab
 */

#ifndef __NK_CHARSET_H__
#define __NK_CHARSET_H__ 1

#include <stdint.h>
#include <string>

namespace neiku
{

// TODO: 判断字符串(string)是否为给定字符集编码(charset)
bool charset_is(const char *string, const char *charest);

// gb18030(兼容gb2312/gbk)编码转utf-8
std::string charset_g2u(const std::string &gbk);

// utf-8编码转gb18030(兼容gb2312/gbk)
std::string charset_u2g(const std::string &utf8);

// TODO: 字符串字符集编码探测(枚举算法)
const char* charset_detect(const char *string);

// 任意字符集编码转换(迭代算法)
int charset_convert(const char* charset_from, const char* charset_to
                    , const std::string& string_from, std::string& string_to);
};

#endif
