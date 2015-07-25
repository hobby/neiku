
/*
 * file:   neiku/singleton.h
 * desc:   (lazy) singleton template class for C++.
 * author: hobby
 * date:   2014/9/14 1:32:58
 *
 * usage:
 *       #include <neiku/singleton.h>
 *       #define OBJECT SINGLETONE(CObject);
 *       class CObject
 *       {};
 * 
 *       OBJECT->function();
 *
 * vim:ts=4;sw=4;expandtab
 */

#ifndef __NK_SINGLETON_H__
#define __NK_SINGLETON_H__

#include <stdlib.h>
#include <pthread.h>

#define SINGLETON(CLASSNAME)      neiku::CLazySingleton<CLASSNAME>::Instance()
#define SINGLETON_TSD(CLASSNAME)  neiku::CTSDSingleton<CLASSNAME>::Instance()

namespace neiku
{

/* 懒汉单例模式
 * 实现：时间换取空间，线程安全(共享)，首次真正访问时才实例化，程序退出后自动释放！
 * 背景：1) Order of initialization (and destruction) problem. (全局静态变量)
 *       2) Construct on frist use. (需要时才实例化)
 *       3) 饿汉单例就像全局变量一样，不推荐使用
 *       4) 双重检测，减少资源的无效抢占，提高效率
 *       5) 局部静态变量的线程安全问题
 * 引用：http://stackoverflow.com/questions/1463707/c-singleton-vs-global-static-object
 */
template <typename T>
class CLazySingleton
{
    public:
        // 返回对象指针可方便实现双重检测
        static T* Instance()
        {
            if (!m_pInstance)
            {
                // 防止优化
                ++m_pInstance;
                --m_pInstance;
                
                // 在C++0X前，局部静态变更初始化不是原子操作
                pthread_mutex_lock(&m_mutex);
                if (!m_pInstance)
                {
                    static T oInstance;
                    m_pInstance = &oInstance;
                }
                pthread_mutex_unlock(&m_mutex);
            }
            
            return m_pInstance;
        };
        
    private:
        static T* m_pInstance;
        static pthread_mutex_t m_mutex;
        
    private:
        CLazySingleton();
        CLazySingleton(const CLazySingleton&);
        CLazySingleton& operator=(const CLazySingleton&);
        ~CLazySingleton()
        {
            m_pInstance = NULL;
        };
};
template <typename T>
T* CLazySingleton<T>::m_pInstance = NULL;
template <typename T>
pthread_mutex_t CLazySingleton<T>::m_mutex = PTHREAD_MUTEX_INITIALIZER;



/* 线程特有数据(线程局部存储)单例模式
 * 实现：线程级单例，单例在线程间各有一份！
 *
 * 引用：http://blog.csdn.net/cywosp/article/details/26469435
 *       http://blog.csdn.net/liuxuejiang158blog/article/details/14101645
 *
 */
template <typename T>
class CTSDSingleton
{
    public:
        static T* Instance()
        {
            pthread_once(&m_thread_once, CreateKey);
            
            T* pInstance = (T*)pthread_getspecific(m_thread_key);
            if (pInstance == NULL)
            {
                pthread_setspecific(m_thread_key, new T);
                pInstance = (T*)pthread_getspecific(m_thread_key);
            }
            return pInstance;
        };
        
    private:
        static pthread_key_t  m_thread_key;
        static pthread_once_t m_thread_once;
        
    private:
        CTSDSingleton();
        CTSDSingleton(const CTSDSingleton&);
        CTSDSingleton& operator=(const CTSDSingleton&);
        ~CTSDSingleton()
        {};
        
    private:
        static void CreateKey()
        {
            // 注册主线程终止事件
            atexit(KillMainThreadSingleton);
            
            // 创建TSD，并注册子线程终事件
            pthread_key_create(&m_thread_key, KillSubThreadSingleton);
        };
        
        // 主线程终止，释放资源
        static void KillMainThreadSingleton()
        {
            T* pInstance = (T*)pthread_getspecific(m_thread_key);
            if (pInstance != NULL)
            {
                delete (T*)pInstance;
            }
        };
                
        // 子线程终止，释放资源
        static void KillSubThreadSingleton(void* pInstance)
        {
            if (pInstance != NULL)
            {
                delete (T*)pInstance;
            }
        };
};
template <typename T>
pthread_key_t CTSDSingleton<T>::m_thread_key = 0;
template <typename T>
pthread_once_t CTSDSingleton<T>::m_thread_once = PTHREAD_ONCE_INIT;

};

#endif
