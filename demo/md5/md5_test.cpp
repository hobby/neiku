
#include <iostream>
#include <stdio.h>
#include <neiku/md5.h>

using namespace std;

int main(int argc, char **argv)
{
    std::string sTemp(argv[1]);

    printf("orig:[%s], md5:[%s]\n"
           , argv[1], yaf::util::Md5Sum(sTemp).c_str());
    return 0;
}

