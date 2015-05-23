
#include <iconv.h>
#include <string>

#include <neiku/charset.h>

/*
 * 
 * gbk2312: Ӣ��ռ1���ֽڣ�����ռ2���ֽ�
 * gbk: Ӣ��ռ1���ֽڣ�����ռ2���ֽ�
 * gb18030: Ӣ�ĵ�1���ֽڣ�����ռ2��(gb2312/gbk)����4���ֽ�(cjk-a/b)
 * utf-8:
 *       0000~007f: 1xxx xxxx ��ռ��1�ֽ�
 *       0080~07ff: 110x xxxx 10xx xxxx ��ռ��2���ֽ�
 *       0800~ffff: 1110 xxxx 10xx xxxx 10xx xxxx ��ռ��3���ֽ�(����4E00~9FA5�����ģ����������ַ�ռ3���ֽ�)
 *
 * */
namespace neiku
{

/*
 * convert_charset: iconv_open/iconv/iconv_close�ļ򵥷�װ
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
 * ���½���䳤����2�ֽڵ������ַ�ת��3�ֽڵ�utf-8�ַ��������Ȳ��ᳬ��ԭ����0.5��
 * ���g2uԤ�����������=���볤��*1.5+1 (����1��Ϊ'\0'������)
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
 * ���½���䳤����3�ֽڵ������ַ�ת��4�ֽڵ�gb18030�ַ��������Ȳ��ᳬ��ԭ����0.5��
 * ���u2gԤ�����������=���볤��*1.5+1 (����1��Ϊ'\0'������)
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
