
#ifndef __SINGLETON_CLASS_H_
#define __SINGLETON_CLASS_H_


#include <iostream>
#include <neiku/singleton.h>

using namespace std;

#define TEST     SINGLETON(neiku::CTest)
#define TSDTEST  SINGLETON_TSD(neiku::CTest)
#define TSDTEST2 SINGLETON_TSD(neiku::CTest2)

   
namespace neiku
{
    
class CTest
{
    public:
        CTest(): m_iNumber(0)
        {
            cout << pthread_self() << " executing CTest::CTest()" << endl;
        };
        
        ~CTest()
        {
            cout << pthread_self() << " executing CTest::~CTest()" << endl;
        };
        
    public:
        void Do()
        {
            m_iNumber++;
            cout << "TEST: current number is " << m_iNumber << "!" << endl;
        };
        
        int GetNumber()
        {
            return m_iNumber++;
        };
    private:
        int  m_iNumber;
};

class CTest2
{
    public:
        CTest2(): m_iNumber(0)
        {
            cout << pthread_self() << " executing CTest2::CTest2()" << endl;
        };
        
        ~CTest2()
        {
            cout << pthread_self() << " executing CTest2::~CTest2()" << endl;
        };
        
    public:
        void Do()
        {
            m_iNumber++;
            cout << "TEST2: current number is " << m_iNumber << "!" << endl;
        };
        
    private:
        int  m_iNumber;
};

};

#endif
