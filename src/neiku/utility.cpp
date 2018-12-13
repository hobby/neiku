// vim:ts=4:sw=4:expandtab

#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <cstdio>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <iconv.h>

#include <fstream>

#include <functional>
#include <algorithm>

#include "neiku/utility.h"

uint64_t neiku::timeus()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 * 1000 + tv.tv_usec;
}

uint64_t neiku::timems()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

std::string neiku::timestr(time_t sec, const char* fmt)
{
    if (sec <= 0 || fmt == NULL)
    {
        return std::string("");
    }

    struct tm tms;
    localtime_r(&sec, &tms);

    // always keep \0 inside
    char buf[33] = {0};
    strftime(buf, sizeof(buf), fmt, &tms);
    return std::string(buf);
}

time_t neiku::timestamp(const char* str, const char * fmt)
{
    if (!str || !fmt)
    {
        return -1;
    }

    struct tm tms;
    memset(&tms, 0, sizeof(tms));
    if (!strptime(str, fmt, &tms))
    {
        return -1;
    }

    return mktime(&tms);
}

#define _CHAR_TO_INT(FROM, TO, FUNC) void neiku::_to(FROM from, TO to) { to = FUNC(from, NULL, 0); }
_CHAR_TO_INT(const char*, int32_t&, strtol);
_CHAR_TO_INT(const char*, int64_t&, strtoll);
_CHAR_TO_INT(const char*, uint32_t&, strtoul);
_CHAR_TO_INT(const char*, uint64_t&, strtoull);
#define _STR_TO_INT(FROM, TO, FUNC) void neiku::_to(FROM from, TO to) { to = FUNC(from.c_str(), NULL, 0); }
_STR_TO_INT(const std::string&, int32_t&, strtol);
_STR_TO_INT(const std::string&, int64_t&, strtoll);
_STR_TO_INT(const std::string&, uint32_t&, strtoul);
_STR_TO_INT(const std::string&, uint64_t&, strtoull);

#define _CHAR_TO_FLT(FROM, TO, FUNC) void neiku::_to(FROM from, TO to) { to = FUNC(from, NULL); }
_CHAR_TO_FLT(const char*, float&, strtof);
_CHAR_TO_FLT(const char*, double&, strtod);
#define _STR_TO_FLT(FROM, TO, FUNC) void neiku::_to(FROM from, TO to) { to = FUNC(from.c_str(), NULL); }
_STR_TO_FLT(const std::string&, float&, strtof);
_STR_TO_FLT(const std::string&, double&, strtod);

std::string neiku::bit2str(uint32_t bits, char sep)
{
    if (bits <= 0)
    {
        return std::string("");
    }

    uint32_t code = 0;
    std::string str;
    for (int idx = 1; idx <= 32; ++idx)
    {
        code = (1U << (idx - 1));
        if ((bits & code) != 0)
        {
            str.append(to<std::string>(idx)).append(1, sep);
        }
    }
    if (!str.empty())
    {
        str.resize(str.size() - 1);
    }
    return str;
}

std::string neiku::bit2str(uint64_t bits, char sep)
{
    if (bits <= 0)
    {
        return std::string("");
    }

    uint64_t code = 0;
    std::string str;
    for (int idx = 1; idx <= 64; ++idx)
    {
        code = (1UL << (idx - 1));
        if ((bits & code) != 0)
        {
            str.append(to<std::string>(idx)).append(1, sep);
        }
    }
    if (!str.empty())
    {
        str.resize(str.size() - 1);
    }
    return str;
}

uint64_t neiku::str2bit(const std::string& str, char sep)
{
    std::vector<uint32_t> list;
    neiku::explode(list, str, sep);

    uint64_t bits = 0;
    for (std::vector<uint32_t>::iterator it = list.begin();
         it != list.end();
         ++it)
    {
        if (1 <= *it && *it <= 64)
        {
            bits = bits | (1UL << (*it - 1));
        }
    }
    return bits;
}

std::string neiku::localIp(const char* eth_name)
{
    std::string ip("127.0.0.1");

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        return ip;
    }

    struct ifreq ifq;
    memset(&ifq, 0, sizeof(ifq));
    snprintf(ifq.ifr_name, IFNAMSIZ, "%s", eth_name);
    int ret = ioctl(fd, SIOCGIFADDR, &ifq);
    close(fd);
    if (ret < 0)
    {
        return ip;
    }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    memcpy(&sin, &ifq.ifr_addr, sizeof(sin));

    char buffer[INET_ADDRSTRLEN + 1];
    const char * ptr = inet_ntop(AF_INET, &sin.sin_addr, buffer, sizeof(buffer));
    if (ptr == NULL)
    {
        return ip;
    }
    ip = buffer;
    return ip;
}

std::string neiku::u2g(const std::string& utf8)
{
    std::string gb18030;
    neiku::convert_charset("UTF-8", "GB18030//IGNORE", utf8, gb18030);
    return gb18030;
}

std::string neiku::g2u(const std::string& gb18030)
{
    std::string utf8;
    neiku::convert_charset("GB18030", "UTF-8//IGNORE", gb18030, utf8);
    return utf8;
}

int neiku::convert_charset(const char* from_charset, const char* to_charset
                            , const std::string& from_string, std::string& to_string)
{
    iconv_t cd = iconv_open(to_charset, from_charset);
    if (!cd)
    {
        return -1;
    }

    to_string = "";
    int ret = 0;
    size_t inbytes = from_string.size();
    char* inbuf = const_cast<char*>(from_string.c_str());
    while (inbytes > 0)
    {
        char buffer[1024] = {0};
        char *outbuf = buffer;
        size_t outbytes = sizeof(buffer) - 1;
        ret = iconv(cd, &inbuf, &inbytes, &outbuf, &outbytes);
        if (ret == 0 || errno == E2BIG)
        {
            to_string.append(buffer);
        }
        else
        {
            break;
        }
    }
    ret = iconv_close(cd);
    if (inbytes != 0 || ret != 0)
    {
        return -1;
    }
    
    return 0;
}

std::string neiku::rawurlencode(const std::string& str)
{
    if (str.empty())
    {
        return std::string("");
    }

    const char* code = "0123456789ABCDEF";

    uint32_t length = str.length();
    char* buffer = new char[length * 3 + 1];
    char* p = buffer;
    const uint8_t* ptr = (const uint8_t*)str.data();
    for (uint32_t idx = 0; idx < length; ++idx)
    {
        uint8_t c = ptr[idx];
        if (   (c == '-')
            || (c == '.')
            || (c == '_')
            || ('0' <= c && c <= '9')
            || ('a' <= c && c <= 'z')
            || ('A' <= c && c <= 'Z'))
        {
            *(p++) = c;
        }
        else
        {
            *(p++) = '%';
            *(p++) = code[c >> 4];
            *(p++) = code[c & 0x0F];
        }
    }

    std::string encoded(buffer, p - buffer);
    delete []buffer;
    return encoded;
}

std::string neiku::urlencode(const std::string& str)
{
    if (str.empty())
    {
        return std::string("");
    }

    const char* code = "0123456789ABCDEF";

    uint32_t length = str.length();
    char* buffer = new char[length * 3 + 1];
    char* p = buffer;
    const uint8_t* ptr = (const uint8_t*)str.data();
    for (uint32_t idx = 0; idx < length; ++idx)
    {
        uint8_t c = ptr[idx];
        if (   (c == '-')
            || (c == '.')
            || (c == '_')
            || ('0' <= c && c <= '9')
            || ('a' <= c && c <= 'z')
            || ('A' <= c && c <= 'Z'))
        {
            *(p++) = c;
        }
        else if (c == ' ')
        {
            *(p++) = '+';
        }
        else
        {
            *(p++) = '%';
            *(p++) = code[c >> 4];
            *(p++) = code[c & 0x0F];
        }
    }

    std::string encoded(buffer, p - buffer);
    delete []buffer;
    return encoded;
}

std::string neiku::rawurldecode(const std::string& str)
{
    if (str.empty())
    {
        return std::string("");
    }

    const uint8_t code[] = {
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  1,   2,   3,   4,   5,   6,   7,  8,  9,  0,  0,  0,  0,  0,  0,
        0,  10,  11,  12,  13,  14,  15,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  10,  11,  12,  13,  14,  15,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
    };

    uint32_t length = str.length();
    char* buffer = new char[length + 1];
    char* p = buffer;
    const uint8_t* ptr = (const uint8_t*)str.data();
    for (uint32_t idx = 0; idx < length;)
    {
        uint8_t c = ptr[idx];
        if (c == '%' && (idx + 2 < length))
        {
            *(p++) = ((code[ptr[idx+1]]) << 4) | ((code[ptr[idx+2]]) & 0x0f);
            idx += 3;
        }
        else
        {
            *(p++) = c;
            idx += 1;
        }
    }

    std::string decoded(buffer, p - buffer);
    delete []buffer;
    return decoded;
}

std::string neiku::urldecode(const std::string& str)
{
    if (str.empty())
    {
        return std::string("");
    }

    const uint8_t code[] = {
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  1,   2,   3,   4,   5,   6,   7,  8,  9,  0,  0,  0,  0,  0,  0,
        0,  10,  11,  12,  13,  14,  15,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  10,  11,  12,  13,  14,  15,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  0,
    };

    uint32_t length = str.length();
    char* buffer = new char[length + 1];
    char* p = buffer;
    const uint8_t* ptr =  (const uint8_t*)str.data();
    for (uint32_t idx = 0; idx < length;)
    {
        uint8_t c = ptr[idx];
        if (c == '%' && (idx + 2 < length))
        {
            *(p++) = ((code[ptr[idx+1]]) << 4) | ((code[ptr[idx+2]]) & 0x0f);
            idx += 3;
        }
        else if (c == '+')
        {
            *(p++) = ' ';
            idx += 1;
        }
        else
        {
            *(p++) = c;
            idx += 1;
        }
    }

    std::string decoded(buffer, p - buffer);
    delete []buffer;
    return decoded;
}

std::string neiku::base64encode(const std::string& str)
{
    const char* code = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    uint32_t length = str.length();
    char *buffer = new char[(length + 2) / 3 * 4 + 1];
    char *p = buffer;
    const uint8_t* ptr = (const uint8_t*)str.data();
    for (uint32_t idx = 0; idx < length;)
    {
        if (idx + 2 < length)
        {
            *(p++) = code[ptr[idx] >> 2];
            *(p++) = code[0x3f & (ptr[idx+0] << 4 | ptr[idx+1] >> 4)];
            *(p++) = code[0x3f & (ptr[idx+1] << 2 | ptr[idx+2] >> 6)];
            *(p++) = code[0x3f &  ptr[idx+2]];
            idx += 3;
        }
        else if (idx + 1 < length)
        {
            *(p++) = code[ptr[idx] >> 2];
            *(p++) = code[0x3f & (ptr[idx+0] << 4 | ptr[idx+1] >> 4)];
            *(p++) = code[0x3f & (ptr[idx+1] << 2 | ptr[idx+2] >> 6)];
            *(p++) = '=';
            idx += 2;
        }
        else
        {
            *(p++) = code[ptr[idx] >> 2];
            *(p++) = code[0x3f & (ptr[idx+0] << 4 | ptr[idx+1] >> 4)];
            *(p++) = '=';
            *(p++) = '=';
            idx += 1;
        }
    }

    std::string encoded(buffer, p - buffer);
    delete []buffer;
    return encoded;
}

std::string neiku::base64decode(const std::string& str)
{
    const uint8_t code[] = {
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   62,  0,   0,    0,   63,
        52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  0,   0,   0,   0,    0,   0,
        0,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,   13,  14,
        15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  0,   0,   0,    0,   0,
        0,   26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,   39,  40,
        41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  0,   0,   0,    0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,
    };

    uint32_t length = str.length();
    if (length % 4 != 0)
    {
        return std::string("");
    }

    char *buffer = new char[length];
    char *p = buffer;
    const uint8_t* ptr = (const uint8_t*)str.data();

    uint32_t idx = 0;
    for (; idx < length - 4;)
    {
        *(p++) = ((0x3f & code[ptr[idx+0]]) << 2) | (0x03 & (code[ptr[idx+1]] >> 4));
        *(p++) = ((0x0f & code[ptr[idx+1]]) << 4) | (0x0f & (code[ptr[idx+2]] >> 2));
        *(p++) = ((0x03 & code[ptr[idx+2]]) << 6) | (0x3f & (code[ptr[idx+3]] >> 0));
        idx += 4;
    }

    // 最后的=符号为base64编码追加的补齐性数据（0-2个），解码时该丢弃掉
    idx = length - 4;
    *(p++) = ((0x3f & code[ptr[idx+0]]) << 2) | (0x03 & (code[ptr[idx+1]] >> 4));
    if (ptr[idx+2] != '=')
    {
        *(p++) = ((0x0f & code[ptr[idx+1]]) << 4) | (0x0f & (code[ptr[idx+2]] >> 2));
    }
    if (ptr[idx+3] != '=')
    {
        *(p++) = ((0x03 & code[ptr[idx+2]]) << 6) | (0x3f & (code[ptr[idx+3]] >> 0));
    }

    // 二进制数据，必须指定长度
    std::string decoded(buffer, p - buffer);
    delete []buffer;
    return decoded;
}

std::string neiku::cat(const std::string& filename)
{
    std::ifstream ifs(filename.c_str(), std::ios_base::binary);
    if (!ifs)
    {
        return std::string("");
    }

    ifs.seekg(0, ifs.end);
    int length = ifs.tellg();
    ifs.seekg(0, ifs.beg);

    if (length <= 0)
    {
        ifs.close();
        return std::string("");
    }

    char *buffer = new char[length];
    ifs.read(buffer, length);
    if (!ifs)
    {
        delete []buffer;
        ifs.close();
        return std::string("");
    }

    ifs.close();
    std::string content(buffer, length);
    delete []buffer;
    return content;
}

std::string neiku::cat(FILE* fp)
{
    if (fp == NULL)
    {
        return std::string("");
    }

    fseek(fp, 0, SEEK_END);
    long int length = ftell(fp);
    if (length <= 0)
    {
        return std::string("");
    }
    fseek(fp, 0, SEEK_SET);
    
    char *buffer = new char[length];
    size_t count = fread(buffer, length, 1, fp);
    if (count != 1)
    {
        delete []buffer;
        return std::string("");
    }

    std::string content(buffer, length);
    delete []buffer;
    return content;
}

int64_t neiku::filesize(FILE* fp)
{
    if (fp == NULL)
    {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    return ftell(fp);
}


int neiku::tee(const std::string& data, const std::string& filename)
{
    // TODO: 自动创建filename缺失的子目录

    std::ofstream ofs(filename.c_str(), std::ios_base::binary);
    if (!ofs)
    {
        return -1;
    }

    ofs.write(data.data(), data.length());
    if (!ofs)
    {
        ofs.close();
        return -1;
    }

    ofs.close();
    return 0;
}

std::string neiku::toupper(const std::string& str)
{
    std::string res;
    res.resize(str.size());
    std::transform(str.begin(), str.end(), res.begin(), (int (*)(int))std::toupper);
    return res;
}

std::string neiku::tolower(const std::string& str)
{
    std::string res;
    res.resize(str.size());
    std::transform(str.begin(), str.end(), res.begin(), (int (*)(int))std::tolower);
    return res;
}

size_t neiku::charlen(const std::string& str)
{
    size_t len = 0, pos = 0;
    size_t size = str.size();
    while (pos < size)
    {
        uint8_t code = str[pos];
        if      (0xFC <= code)  { pos += 6; }
        else if (0xF8 <= code)  { pos += 5; }
        else if (0xF0 <= code)  { pos += 4; }
        else if (0xE0 <= code)  { pos += 3; }
        else if (0xC0 <= code)  { pos += 2; }
        else                    { pos += 1; }
        len += 1;
    }

    return len;
}

std::string& neiku::trimleft(std::string& str)
{
    std::string::iterator nonspace = std::find_if(str.begin(), str.end()
                                                  , not1(std::ptr_fun(::isspace)));
    str.erase(str.begin(), nonspace);
    return str;
}

std::string& neiku::trimright(std::string& str)
{
    std::string::reverse_iterator nonspace = std::find_if(str.rbegin(), str.rend()
                                                          , std::not1(std::ptr_fun(::isspace)));
    str.erase(nonspace.base(), str.end());
    return str;
}

std::string& neiku::trim(std::string& str)
{
    return neiku::trimright(neiku::trimleft(str));
};
