
#include <cstdio>
#include <neiku/charset.h>

using namespace std;
using namespace neiku;

void test_g2u()
{
    std::string gbk  = "我是GBK中文!我是GBK中文!我是GBK中文!我是GBK中文!我是GBK中文!我是GBK中文!我是GBK中文!我是GBK中文!我是GBK中文!"; 
    std::string utf8 = neiku::charset_g2u(gbk);
    printf("gbk:[%s], utf8:[%s]\n"
           , gbk.c_str(), utf8.c_str());
    return ;
}

void test_charset_is()
{
    std::string gbk  = "我是GBK中文!我是GBK中文!我是GBK中文!我是GBK中文!我是GBK中文!我是GBK中文!我是GBK中文!我是GBK中文!我是GBK中文!";
    std::string utf8 = charset_g2u(gbk); 
    bool is = neiku::charset_is(utf8.c_str(), "UTF-8");
    printf("is:[%d]\n", is);
    return ;
}

int main(int argc, char **argv)
{
    test_charset_is();
    return 0;
}

