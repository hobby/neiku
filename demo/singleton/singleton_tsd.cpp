
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "singleton_class.h"

using namespace std;

void* thread_entry(void* pArg)
{
    for (int i = 0; i <  2; i++)
    {
        printf("thread:[%s], number:[%d]\n"
               , (char*)pArg, TSDTEST->GetNumber());
    }
}

void func()
{
    TSDTEST2->Do();
    TSDTEST2->Do();
}

int main(int argc, char **argv)
{
    printf("test tsd singleton in mutil thread\n");
    //printf("thread:[main], number:[%d]\n", TSDTEST->GetNumber());
    //printf("thread:[main], number:[%d]\n", TSDTEST->GetNumber());
    func();

    /*
    pthread_t tid1, tid2;
    //void* tret;
    pthread_create(&tid1, NULL, thread_entry, (void*)"tid1");
    pthread_create(&tid2, NULL, thread_entry, (void*)"tid2");
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    */
    return 0;
}

