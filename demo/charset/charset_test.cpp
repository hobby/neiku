
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <neiku/charset.h>

using namespace std;

int main(int argc, char **argv)
{
    //std::string sTo = u2g(argv[1]);
    std::string sTo = neiku::g2u(argv[1]);
    printf("to:[%s]\n",  sTo.c_str());

    return 0;
}

