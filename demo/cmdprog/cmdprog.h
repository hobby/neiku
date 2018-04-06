// vim:ts=4:sw=4:expandtab

#ifndef __NK_CMDPROG_H__
#define __NK_CMDPROG_H__

#include <map>
#include <string>
#include <iostream>
#include "neiku/log.h"

using namespace neiku;

typedef int (*ROUTINE)();

class CCmd
{
public:
    int execute(int argc, char* argv[])
    {
        if (routine != NULL)
        {
            return routine();
        }
        return 0;
    }

public:
    const char* name;
    ROUTINE routine;
};

class CCmdProg
{
public:
    CCmdProg(): progName(""), cmdName("")
    {}

    int execute(int argc, char* argv[])
    {
        progName = argv[0];
        cmdName = argv[1];

        std::map<const char*, CCmd>::iterator cmd = cmds.begin();
        for (; cmd != cmds.end(); ++cmd)
        {
            if (strncmp(cmdName, cmd->first, strlen(cmd->first)) == 0)
            {
                return cmd->second.execute(argc, argv);               
            }
        }

        return help();
    }

    void add(CCmd& cmd)
    {
        cmds.insert(std::make_pair(cmd.name, cmd));
    }

    int help()
    {
        std::cerr << "this is description for " << progName << std::endl
                  << std::endl
                  << "Usage:" << std::endl
                  << "  " << progName << " command" << std::endl
                  << std::endl
                  << "Available Commands:" << std::endl;
        for (std::map<const char*, CCmd>::iterator cmd = cmds.begin(); cmd != cmds.end(); ++cmd)
        {
            std::cerr << "  " << cmd->first << "\t\t" << "short intro for " << cmd->first << std::endl;
        }
        std::cerr << std::endl;

        std::cerr << "Use \"" << progName << " [command] --help\" for more information about a command." << std::endl;
        return 0;
    }

public:
    const char* progName;
    const char* cmdName;
    std::map<const char*, CCmd> cmds;
};

template <typename T>
class CCmdProgSingleton
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
                
                // FIXME: 在C++0X前，局部静态变更初始化不是原子操作
                if (!m_pInstance)
                {
                    static T oInstance;
                    m_pInstance = &oInstance;
                }
            }
            
            return m_pInstance;
        };
        
    private:
        static T* m_pInstance;
        static pthread_mutex_t m_mutex;
        
    private:
        CCmdProgSingleton();
        CCmdProgSingleton(const CCmdProgSingleton&);
        CCmdProgSingleton& operator=(const CCmdProgSingleton&);
        ~CCmdProgSingleton()
        {
            m_pInstance = NULL;
        };
};
template <typename T> T* CCmdProgSingleton<T>::m_pInstance = NULL;

#define PROG CCmdProgSingleton<CCmdProg>::Instance()

#endif