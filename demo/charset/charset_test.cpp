
#include <cstdio>
#include <neiku/charset.h>

using namespace std;

int main(int argc, char **argv)
{
    std::string gbk  = "我是GBK中文!"; 
    std::string utf8 = neiku::charset_g2u(gbk);
    printf("gbk:[%s], utf8:[%s]\n"
           , gbk.c_str(), utf8.c_str());
    return 0;
}

