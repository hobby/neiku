
#include <iostream>
#include "singleton_class.h"

using namespace std;

void module1()
{
    cout << "in module1()" << endl;
    TEST->Do();
    TEST->Do();
    cout << "out module1()" << endl; 
}

