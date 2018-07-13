// vim:ts=4:sw=4:expandtab
#ifndef __NK_CMDBAR_H__
#define __NK_CMDBAR_H__

#include <assert.h>
#include <inttypes.h>
#include <cstddef>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

// cmdbar辅助类型
#define CMDBAR_CMD_TYPE static int                               // 命令字入口类型（一个命令字源文件可包含多个命令字，建议保持1个）
#define CMDBAR_INIT_TYPE static __attribute__((constructor)) int // 命令字初始化类型（一个命令字源文件可以有多个初始化过程，建议保持1个）

class cmdbar
{
private:
    struct CmdOption
    {
        CmdOption(): _type(0)
                   , _required(false)
                   , _good(false)
                   , _key("")
                   , _desc("")
                   , _default("")
                   , _StrPtr(NULL)
                   , _IntPtr(NULL)
        {}

        int          _type;
        bool         _required;
        bool         _good;

        const char*  _key;
        const char*  _desc;
        const char*  _default;

        std::string  *_StrPtr;
        int          *_IntPtr;
    };

public:
    // 注册接口宏
    #define registCmd(name, routine, args...) doRegistCmd(__FILE__, __LINE__, name, routine, ##args)
    #define registOpt(name, key, args...)     doRegistOpt(__FILE__, __LINE__, name, #key, &key, ##args)
    
    // 命令入口原型
    typedef int (*Routine)();

public:
    template<typename T>
    static T& getRef()
    {
        static T t;
        return t;
    }

    template<typename T>
    static T getVal(const std::string& key)
    {
        T val;
        _copyVal(key, val);
        return val;
    }

    template<typename T>
    static T getVal(const char* key)
    {
        assert(key != NULL);
        return getVal<T>(std::string(key));
    }

private:
    static void _copyVal(const std::string& key, std::string& val)
    {
        val = "";
        std::map<std::string, const char*>::iterator itOpt = me()->_CmdOptionPtrMap.find(key);
        if (itOpt != me()->_CmdOptionPtrMap.end() && itOpt->second != NULL)
        {
            val = std::string(itOpt->second);
        }
    }

    #define _COPYVAL4NUM(TYPE, FUNC) static void _copyVal(const std::string& key, TYPE& val) \
    { \
        val = 0; \
        std::map<std::string, const char*>::iterator itOpt = me()->_CmdOptionPtrMap.find(key); \
        if (itOpt != me()->_CmdOptionPtrMap.end() && itOpt->second != NULL) \
        { \
            val = FUNC(itOpt->second, NULL, 0); \
        } \
    }

    _COPYVAL4NUM(int32_t, strtol)
    _COPYVAL4NUM(int64_t, strtoll)
    _COPYVAL4NUM(uint32_t, strtoul)
    _COPYVAL4NUM(uint64_t, strtoull)

public:
    cmdbar(): _good(true){}

    // 启动命令行
    static int launch(int argc, char* argv[])
    {
        if (!me()->_good)
        {
            return -1;
        }

        if (argc <= 0 || argv == NULL)
        {
            me()->printError("Error: argc or argv invalid, argc[%d], argv[%p]", argc, argv);
            return -1;
        }

        me()->_CmdName = argv[0];
        
        if (argc < 2)
        {
            me()->printHelp();
            return 1;
        }

        std::string name = argv[1];

        // 打印命令行帮助提示
        if (name.compare("--help") == 0)
        {
            me()->printHelp();
            return 0;
        }

        // 查找命令
        std::map<std::string, Routine>::iterator cmd = me()->_CmdRoutineMap.find(name);
        if (cmd == me()->_CmdRoutineMap.end())
        {
            me()->printError("Error: unknown command '%s' for '%s'\n", name.c_str(), me()->_CmdName.c_str());
            return 2;
        }

        // 打印命令帮助提示
        if (2 < argc)
        {
            std::string help = argv[2];
            if (help.compare("--help") == 0)
            {
                me()->printHelp4Cmd(name);
                return 0;
            }
        }

        // 参数填充及检测是否没填`必填参数`
        std::map< std::string, std::vector<CmdOption> >::iterator opts = me()->_CmdOptionListMap.find(name);
        if (opts != me()->_CmdOptionListMap.end())
        {
            // 填充参数
            int idx = 2;
            for (size_t i = 0; i < opts->second.size(); ++i)
            {
                if (idx < argc)
                {
                    CmdOption &opt = opts->second[i];
                    opt._good = true;
                    if (opt._type == 1) *(opt._StrPtr) = argv[idx];
                    if (opt._type == 2) *(opt._IntPtr) = strtol(argv[idx], NULL, 0);

                    me()->_CmdOptionPtrMap[opt._key] = argv[idx];

                    idx++;
                }
            }

            // 检测是否没填`必填参数`
            for (size_t i = 0; i < opts->second.size(); ++i)
            {
                CmdOption &opt = opts->second[i];
                if (opt._required && !opt._good)
                {
                    me()->printError("Error: '%s' is required for '%s' command\n", opt._key, name.c_str());
                    return 1;
                }
            }
        }

        // 执行命令
        Routine routine = cmd->second;
        return routine();
    }

    // 注册命令
    static int doRegistCmd(const char* file, int line, const char* name, Routine routine, const char* desc = "")
    {
        if (name == NULL || routine == NULL)
        {
            me()->printPanic("Panic：command name or routine is nullptr, name:[%p], routine:[%p], file:[%s], line:[%d]\n"
                             , name, routine, file, line);
            return -1;
        }

        std::map<std::string, Routine>::iterator cmd = me()->_CmdRoutineMap.find(name);
        if (cmd != me()->_CmdRoutineMap.end())
        {
            me()->printPanic("Panic: command name duplicated, name:[%s]", name);
            return -2;
        }

        me()->_CmdRoutineMap.insert(std::pair<std::string, Routine>(name, routine));
        me()->_CmdDescriptionMap.insert(std::pair<std::string, std::string>(name, desc));
        return 0;
    }

    static int doRegistOpt(const char* file, int line, const char* name, const char* key, std::string *ptr
                           , const char* desc = "", const char* defval = "", bool required = false)
    {
        if (name == NULL || key == NULL || ptr == NULL)
        {
            me()->printPanic("Panic：command name or option key/ptr is nullptr, name:[%p], key:[%p], ptr:[%p], file:[%s], line:[%d]\n"
                             , name, key, ptr, file, line);
            return -1;
        }

        CmdOption opt;
        opt._type = 1;
        opt._required = required;
        opt._good = false;
        opt._key  = key;
        opt._desc = desc;
        opt._default = defval;
        opt._StrPtr = ptr;

        std::vector<CmdOption> &opts = me()->_CmdOptionListMap[name];
        opts.push_back(opt);
        return 0;
    }

    static int doRegistOpt(const char* file, int line, const char* name, const char* key, int *ptr
                           , const char* desc = "", const char* defval = "", bool required = false)
    {
        if (name == NULL || key == NULL || ptr == NULL)
        {
            me()->printPanic("Panic：command name or option key/ptr is nullptr, name:[%p], key:[%p], ptr:[%p], file:[%s], line:[%d]\n"
                             , name, key, ptr, file, line);
            return -1;
        }

        CmdOption opt;
        opt._type = 2;
        opt._required = required;
        opt._good = false;
        opt._key  = key;
        opt._desc = desc;
        opt._default = defval;
        opt._IntPtr = ptr;

        std::vector<CmdOption> &opts = me()->_CmdOptionListMap[name];
        opts.push_back(opt);
        return 0;
    }

private:
    void printHelp()
    {
        printf("Usage:\n");
        printf("  %s < [ <command> <options> | <options> ] >\n", _CmdName.c_str());
        printf("\n");
        
        printf("Available Commands:\n");
        for (std::map<std::string, Routine>::iterator it = _CmdRoutineMap.begin();
             it != _CmdRoutineMap.end(); ++it)
        {
            printf("  %s\t\t%s\n", it->first.c_str(), _CmdDescriptionMap[it->first].c_str());
        }
        printf("\n");

        printf("Echo Command's Usage:\n");
        for (std::map<std::string, Routine>::iterator it = _CmdRoutineMap.begin();
             it != _CmdRoutineMap.end(); ++it)
        {
            printf("  %s %s\t", _CmdName.c_str(), it->first.c_str());
            std::vector<CmdOption> &opts = me()->_CmdOptionListMap[it->first];
            for (size_t i = 0; i < opts.size(); ++i)
            {
                CmdOption &opt = opts[i];
                if (opt._required)
                {
                    printf(" <%s>", opt._key);
                }
                else
                {
                    printf(" [%s]", opt._key);
                }
            }
            printf("\n");
        }
        printf("\n");
        
        printf("Run '%s <command> --help' for more information about a command\n", _CmdName.c_str());
    }

    void printHelp4Cmd(const std::string& name)
    {
        printf("Usage:\n");
        printf("  %s %s", _CmdName.c_str(), name.c_str());
        std::vector<CmdOption> &opts = me()->_CmdOptionListMap[name];
        for (size_t i = 0; i < opts.size(); ++i)
        {
            CmdOption &opt = opts[i];
            if (opt._required)
            {
                printf(" <%s>", opt._key);
            }
            else
            {
                printf(" [%s]", opt._key);
            }
        }
        printf("\n\n");

        printf("Available Options:\n");
        for (size_t i = 0; i < opts.size(); ++i)
        {
            CmdOption &opt = opts[i];
            printf(" %s\t%s (%s, default '%s')\n", opt._key, opt._desc, (opt._required?"required":"optional"), opt._default);
        }
    }

    void printError(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);

        printf("\n");
        printf("Run '%s --help' for usage.\n", _CmdName.c_str());
    }

    void printWarn(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }

    void printPanic(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);

        _good = false;
    }

private:
    static cmdbar* me()
    {
        static cmdbar bar;
        return &bar;
    }

private:
    bool                                                           _good;
    std::string                                                 _CmdName;
    std::map<std::string, Routine>                        _CmdRoutineMap;
    std::map<std::string, std::string>                _CmdDescriptionMap;
    std::map< std::string, std::vector<CmdOption> >    _CmdOptionListMap;
    std::map<std::string, const char*>                  _CmdOptionPtrMap;
};

#endif