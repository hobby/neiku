
#include <iconv.h>
#include <string>

#include <neiku/charset.h>

/*
 * 
 * gbk2312: 英文占1个字节，中文占2个字节
 * gbk: 英文占1个字节，中文占2个字节
 * gb18030: 英文点1个字节，中文占2个(gb2312/gbk)或者4个字节(cjk-a/b)
 * utf-8:
 *       0000~007f: 1xxx xxxx 即占用1字节
 *       0080~07ff: 110x xxxx 10xx xxxx 即占用2个字节
 *       0800~ffff: 1110 xxxx 10xx xxxx 10xx xxxx 即占用3个字节(其中4E00~9FA5是中文，所以中文字符占3个字节)
 *
 * */
namespace neiku
{

/*
 * convert_charset: iconv_open/iconv/iconv_close的简单封装
 */
int convert_charset(const char *from, const char *to
                    , char *inbuf, size_t inlen
                    , char *outbuf, size_t outlen)
{
    iconv_t cd = iconv_open(to, from);
    if (cd)
    {
        iconv(cd, &inbuf, &inlen, &outbuf, &outlen);
        iconv_close(cd);
        return 0;
    }
    return -1;
}

/* g2u: gb18030/gbk/gb2312 => utf-8
 * 导致结果变长的是2字节的中文字符转成3字节的utf-8字符，但长度不会超出原来的0.5倍
 * 因此g2u预留的输出长度=输入长度*1.5+1 (最后的1做为'\0'做保护)
 */
std::string g2u(const std::string &from)
{
    std::string to;

    char  *inbuf  = const_cast<char*>(from.c_str());
    size_t inlen  = from.size();
    size_t outlen = inlen + (inlen>>1) + 1;
    char  *outbuf = new char[outlen]();
    convert_charset("GB18030", "UTF-8//IGNORE", inbuf, inlen, outbuf, outlen);
    to.assign(outbuf);
    delete[]outbuf;

    return to;
}

/* ug2: utf-8 => gb18030/gbk/gb2312
 * 导致结果变长的是3字节的中文字符转成4字节的gb18030字符，但长度不会超出原来的0.5倍
 * 因此u2g预留的输出长度=输入长度*1.5+1 (最后的1做为'\0'做保护)
 */
std::string u2g(const std::string &from)
{
    std::string to;

    char  *inbuf  = const_cast<char*>(from.c_str());
    size_t inlen  = from.size();
    size_t outlen = inlen + (inlen>>1) + 1;
    char  *outbuf = new char[outlen]();
    convert_charset("UTF-8", "GB18030//IGNORE", inbuf, inlen, outbuf, outlen);
    to.assign(outbuf);
    delete[]outbuf;

    return to;
}

};
