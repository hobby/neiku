
#include <errno.h>
#include <iconv.h>
#include <cstring>
#include <string>

#include <neiku/charset.h>

/*
 * ------- 字符集编码速查 -------
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
 * charset_convert: 字符集编码转换(迭代算法)
 */
int charset_convert(const char* charset_from, const char* charset_to
                    , const std::string& string_from, std::string& string_to)
{
    string_to = "";
    iconv_t cd = iconv_open(charset_to, charset_from);
    if(cd)
    {
        int ret = 0;
        char buffer[1024] = {0};
        char *inbuf= const_cast<char*>(string_from.c_str()), *outbuf = buffer;
        size_t inbytes = string_from.size(), outbytes =(sizeof(buffer) - 1);
        while (inbytes > 0)
        {
            ret = iconv(cd, &inbuf, &inbytes, &outbuf, &outbytes);
            if(ret == 0 || errno == E2BIG)
            {
                string_to.append(buffer);
                memset(buffer, 0, sizeof(buffer));
                outbuf = buffer;
                outbytes = sizeof(buffer) - 1;
            }
            else
            {
                break;
            }
        }
        iconv_close(cd);
        return ret;
    }
    return -1;
}

/* g2u: gb18030/gbk/gb2312 => utf-8
 * 导致结果变长的是2字节的中文字符转成3字节的utf-8字符，但长度不会超出原来的0.5倍
 * 因此g2u预留的输出长度=输入长度*1.5+1 (最后的1做为'\0'做保护)
 */
std::string charset_g2u(const std::string &gbk)
{
    std::string utf8;
    charset_convert("GB18030", "UTF-8//IGNORE", gbk, utf8);
    return utf8;
}

/* ug2: utf-8 => gb18030/gbk/gb2312
 * 导致结果变长的是3字节的中文字符转成4字节的gb18030字符，但长度不会超出原来的0.5倍
 * 因此u2g预留的输出长度=输入长度*1.5+1 (最后的1做为'\0'做保护)
 */
std::string charset_u2g(const std::string &utf8)
{
    std::string gbk;
    charset_convert("UTF-8", "GB18030//IGNORE", utf8, gbk);
    return gbk;
}

};
