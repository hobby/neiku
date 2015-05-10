
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "singleton_class.h"

using namespace std;

void* thread_entry(void*)
{
    for (int i = 0; i <  200; i++)
    {
        TEST->Do();
    }
}

int main(int argc, char **argv)
{
    printf("test singleton in mutil thread\n");
    TEST->Do();
    
    pthread_t tid1, tid2;
    //void* tret;
    pthread_create(&tid1, NULL, thread_entry, NULL);
    pthread_create(&tid2, NULL, thread_entry, NULL);
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    return 0;
}

