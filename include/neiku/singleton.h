
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

/* ��������ģʽ
 * ʵ�֣�ʱ�任ȡ�ռ䣬�̰߳�ȫ(����)���״���������ʱ��ʵ�����������˳����Զ��ͷţ�
 * ������1) Order of initialization (and destruction) problem. (ȫ�־�̬����)
 *       2) Construct on frist use. (��Ҫʱ��ʵ����)
 *       3) ������������ȫ�ֱ���һ�������Ƽ�ʹ��
 *       4) ˫�ؼ�⣬������Դ����Ч��ռ�����Ч��
 *       5) �ֲ���̬�������̰߳�ȫ����
 * ���ã�http://stackoverflow.com/questions/1463707/c-singleton-vs-global-static-object
 */
template <typename T>
class CLazySingleton
{
    public:
        // ���ض���ָ��ɷ���ʵ��˫�ؼ��
        static T* Instance()
        {
            if (!m_pInstance)
            {
                // ��ֹ�Ż�
                ++m_pInstance;
                --m_pInstance;
                
                // ��C++0Xǰ���ֲ���̬�����ʼ������ԭ�Ӳ���
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



/* �߳���������(�ֲ߳̾��洢)����ģʽ
 * ʵ�֣��̼߳��������������̼߳����һ�ݣ�
 *
 * ���ã�http://blog.csdn.net/cywosp/article/details/26469435
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
            // ע�����߳���ֹ�¼�
            atexit(KillMainThreadSingleton);
            
            // ����TSD����ע�����߳����¼�
            pthread_key_create(&m_thread_key, KillSubThreadSingleton);
        };
        
        // ���߳���ֹ���ͷ���Դ
        static void KillMainThreadSingleton()
        {
            T* pInstance = (T*)pthread_getspecific(m_thread_key);
            if (pInstance != NULL)
            {
                delete (T*)pInstance;
            }
        };
                
        // ���߳���ֹ���ͷ���Դ
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
