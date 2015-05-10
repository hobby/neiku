
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "singleton_class.h"

using namespace std;

void module1();
void module2();

int main(int argc, char **argv)
{
    TEST->Do();
    TEST->Do();
    module1();
    
    return 0;
}

