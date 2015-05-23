
/*
 * file:   neiku/charset.h
 * desc:   charset tools for C++
 * author: hobby
 * date:   2015/05/23 11:55:00
 *
 * usage:
 *       #include <neiku/charset.h>
 *
 *       std::string sGbk  = neiku::u2g("utf-8 string");
 *       std::string sUtf8 = neiku::g2u("gb2312/gbk/gb18030 string");
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

// 封装iconv.h中的iconv_open/iconv/iconv_close
int convert_charset(const char *from, const char *to
                    , char *inbuf, size_t inlen
                    , char *outbuf, size_t outlen);

// gb18030(兼容gb2312/gbk)编码转utf-8
std::string g2u(const std::string &from);

// utf-8编码转gb18030(兼容gb2312/gbk)
std::string u2g(const std::string &from);

};

#endif
