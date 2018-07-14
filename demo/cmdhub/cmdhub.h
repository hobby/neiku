// vim:ts=4:sw=4:expandtab
#ifndef __NK_CMDHUB_H__
#define __NK_CMDHUB_H__

#include <unistd.h>
#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <cstddef>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

// cmdhub辅助类型
#define CMDHUB_CMD_TYPE static int                               // 命令执行入口类型
#define CMDHUB_INIT_TYPE static __attribute__((constructor)) int // 命令初始化入口类型

// cmdhub日志工具
#define CMDHUB_ERROR(fmt, args...) printf("[%s:%d] %s: "fmt"\n", __FILE__, __LINE__, cmdhub::logLabel(1), ##args) // 错误日志
#define CMDHUB_INFO(fmt, args...)  printf("[%s:%d] %s: "fmt"\n", __FILE__, __LINE__, cmdhub::logLabel(2), ##args) // 提示日志
#define CMDHUB_DEBUG(fmt, args...) printf("[%s:%d] %s: "fmt"\n", __FILE__, __LINE__, cmdhub::logLabel(3), ##args) // 调试日志

// 结构体乱序赋值（C++）出现：不支持（网上很多说明，只有c支持，c++要c++14可能会支持，但其实跟编译器有关系）
// https://www.systutorials.com/241133/g-sorry-unimplemented-non-trivial-designated-initializers-not-supported/
// 关键：乱序目标是知道值赋给了哪个字段，只要字段顺序与定义顺序一致，是可以的（g++ 4.8.5）

class cmdhub
{
public:

    typedef int (*Routine)();

    struct Cmd /* Command */
    {
        // 不能有构造函数，否则乱序赋值方式编不过

        Routine      exec;      // 命令入口
        const char*  desc;      // 命令描述
    };

    struct Opt /* Option */
    {
        const char*  name;      // 选项名字
        const char*  desc;      // 选项描述
        bool         optional;  // 是否可选（默认必填字段）

        bool         _good;     // 选项是否ok（只对必填字段有用）
    };

public:
    // 设置全局初始化入口（解析命令行参数之后，执行命令之前）
    #define ready(callback) doReady(callback, #callback, __FILE__, __LINE__)

    static cmdhub& doReady(Routine callback, const char* name, const char* file, int line)
    {
        cmdhub* _this = me();
        _this->_readyCallbackPtr = callback;
        _this->_readyCallbackName = name;
        _this->_readyCallbackFile = file;
        _this->_readyCallbackLine = line;
        return *_this;
    }

public:
    // 启动命令行
    #define launch(argc, argv)  doLaunch(argc, argv)

    static int doLaunch(int argc, char* argv[])
    {
        cmdhub* _this = me();

        if (!_this->_good)
        {
            return -1;
        }

        if (argc <= 0 || argv == NULL)
        {
            _this->printError("Error: argc or argv invalid, argc[%d], argv[%p]", argc, argv);
            return -1;
        }

        _this->_CmdName = argv[0];
        
        if (argc < 2)
        {
            _this->printHelp();
            return 1;
        }

        std::string name = argv[1];

        // 打印命令行帮助提示
        if (name.compare("--help") == 0)
        {
            _this->printHelp();
            return 0;
        }

        // 查找命令
        std::map<std::string, Routine>::iterator cmd = _this->_CmdRoutineMap.find(name);
        if (cmd == _this->_CmdRoutineMap.end())
        {
            _this->printError("Error: unknown command '%s' for '%s'\n", name.c_str(), _this->_CmdName.c_str());
            return 2;
        }

        // 打印命令帮助提示
        if (2 < argc)
        {
            std::string help = argv[2];
            if (help.compare("--help") == 0)
            {
                _this->printHelp4Cmd(name);
                return 0;
            }
        }

        // 参数填充及检测是否没填`必填参数`
        std::map< std::string, std::vector<Opt> >::iterator opts = _this->_CmdOptionListMap.find(name);
        if (opts != _this->_CmdOptionListMap.end())
        {
            // 填充参数
            int idx = 2;
            for (size_t i = 0; i < opts->second.size(); ++i)
            {
                if (idx < argc)
                {
                    Opt &opt = opts->second[i];
                    opt._good = true;
                    _this->_CmdOptionPtrMap[opt.name] = argv[idx];
                    idx++;
                }
            }

            // 检测是否没填`必填参数`
            for (size_t i = 0; i < opts->second.size(); ++i)
            {
                Opt &opt = opts->second[i];
                if (!opt.optional && !opt._good)
                {
                    _this->printError("Error: '%s' is required for '%s' command\n", opt.name, name.c_str());
                    return 1;
                }
            }
        }

        // 执行全局初始化（业务自定义）
        if (_this->_readyCallbackPtr != NULL)
        {
            int ret = _this->_readyCallbackPtr();
            if (ret != 0)
            {
                _this->printPanic("Panic: ready faild for command, ret:[%d], callback:[%s], code:[%s:%d]\n"
                                  , ret, _this->_readyCallbackName.c_str()
                                  , _this->_readyCallbackFile.c_str(), _this->_readyCallbackLine);
                return -1;
            }
        }

        // 执行命令
        Routine routine = cmd->second;
        return routine();
    }

public:
    // 注册命令、选项，支持链式操作
    // 例如：cmdhub::addCmd(Cmd{}).addOpt(Opt{}).addOpt(Opt{});
    #define addCmd(CMD, args...)  doAddCmd(__FILE__, __LINE__, CMD, ##args)

    static cmdhub& doAddCmd(const char* file, int line, cmdhub::Cmd cmd)
    {
        cmdhub* _this = me();

        if (file == NULL || cmd.exec == NULL)
        {
            _this->printPanic("Panic：command file or exec is nullptr, exec:[%p], file:[%s], line:[%d]\n"
                             , cmd.exec, file, line);
            return *_this;
        }
        if (cmd.desc == NULL)
        {
            cmd.desc = "";
        }

        std::string name = _this->getFileName(file);
        std::map<std::string, Routine>::iterator itCmd = _this->_CmdRoutineMap.find(name);
        if (itCmd != _this->_CmdRoutineMap.end())
        {
            _this->printPanic("Panic: command name duplicated, name:[%s], file:[%s], line:[%d]\n"
                              , name.c_str(), file, line);
            return *_this;
        }

        _this->_lastAddedCmdName = name;
        _this->_CmdRoutineMap.insert(std::pair<std::string, Routine>(name, cmd.exec));
        _this->_CmdDescriptionMap.insert(std::pair<std::string, std::string>(name, cmd.desc));
        return *_this;
    }

    cmdhub& addOpt(cmdhub::Opt opt)
    {
        if (opt.name == NULL || opt.name[0] == 0)
        {
            printPanic("Panic：option name is nullptr or empty, option name-ptr:[%p], option name-val:[%s], command name:[%s]\n"
                             , opt.name, opt.name, _lastAddedCmdName.c_str());
            return *this;
        }

        if (opt.desc == NULL)
        {
            opt.desc = "";
        }

        std::vector<Opt> &opts = _CmdOptionListMap[_lastAddedCmdName];
        opts.push_back(opt);
        return *this;
    }

public:
    template<typename T>
    static T& getRef()
    {
        static T t;
        return t;
    }

    template<typename T>
    static T getVal(const std::string& name)
    {
        T val;
        _copyVal(name, val);
        return val;
    }

    template<typename T>
    static T getVal(const char* name)
    {
        assert(name != NULL);
        return getVal<T>(std::string(name));
    }

public:
    static const char* logLabel(int level)
    {
        assert(1 <= level && level <= 3);

        static const char* _label4terminal[] = {
            "",
            "\033[31;1mERROR\033[0m",
            "\033[32;1mINFO\033[0m",
            "\033[35;1mDEBUG\033[0m",
        };

        static const char* _label4file[] = {
            "",
            "ERROR",
            "INFO",
            "DEBUG",
        };

        static int _isatty = 0;
        static bool _checktty = false;
        if (!_checktty)
        {
            _checktty = true;
            _isatty = isatty(STDOUT_FILENO);
        }

        if (_isatty)
        {
            return _label4terminal[level];
        }
        return _label4file[level];
    }

private:
    static void _copyVal(const std::string& name, std::string& value)
    {
        cmdhub* _this = me();
        value = "";
        std::map<std::string, const char*>::iterator itOpt = _this->_CmdOptionPtrMap.find(name);
        if (itOpt != _this->_CmdOptionPtrMap.end() && itOpt->second != NULL)
        {
            value = std::string(itOpt->second);
        }
    }

    #define _COPYVAL4NUM(TYPE, FUNC) static void _copyVal(const std::string& name, TYPE& value) \
    { \
        cmdhub* _this = me(); \
        value = 0; \
        std::map<std::string, const char*>::iterator itOpt = _this->_CmdOptionPtrMap.find(name); \
        if (itOpt != _this->_CmdOptionPtrMap.end() && itOpt->second != NULL) \
        { \
            value = FUNC(itOpt->second, NULL, 0); \
        } \
    }

    _COPYVAL4NUM(int32_t, strtol)
    _COPYVAL4NUM(int64_t, strtoll)
    _COPYVAL4NUM(uint32_t, strtoul)
    _COPYVAL4NUM(uint64_t, strtoull)

private:
    void printHelp()
    {
        printf("Usage:\n");
        printf("  %s < [ <command> <options> | <options> ] >\n", _CmdName.c_str());
        printf("\n");
        
        // 计算最长名字的长度，用于打印help填充对齐
        size_t longestCmdName = 0;
        for (std::map<std::string, Routine>::iterator it = _CmdRoutineMap.begin();
             it != _CmdRoutineMap.end(); ++it)
        {
            if (longestCmdName < it->first.size())
            {
                longestCmdName = it->first.size();
            }
        }

        printf("Available Commands:\n");
        for (std::map<std::string, Routine>::iterator it = _CmdRoutineMap.begin();
             it != _CmdRoutineMap.end(); ++it)
        {
            std::string padding;
            if (it->first.size() < longestCmdName)
            {
                padding.append(longestCmdName - it->first.size(), ' ');
            }
            printf("  %s%s  %s\n", it->first.c_str(), padding.c_str(), _CmdDescriptionMap[it->first].c_str());
        }
        printf("\n");

        printf("Echo Command's Usage:\n");
        for (std::map<std::string, Routine>::iterator it = _CmdRoutineMap.begin();
             it != _CmdRoutineMap.end(); ++it)
        {
            std::string padding;
            if (it->first.size() < longestCmdName)
            {
                padding.append(longestCmdName - it->first.size(), ' ');
            }
            printf("  %s %s%s ", _CmdName.c_str(), it->first.c_str(), padding.c_str());

            std::vector<Opt> &opts = _CmdOptionListMap[it->first];
            for (size_t i = 0; i < opts.size(); ++i)
            {
                Opt &opt = opts[i];
                if (!opt.optional)
                {
                    printf(" <%s>", opt.name);
                }
                else
                {
                    printf(" [%s]", opt.name);
                }
            }
            printf("\n");
        }
        printf("\n");
        
        printf("Run '%s <command> --help' for more information about a command\n", _CmdName.c_str());
    }

    void printHelp4Cmd(const std::string& name)
    {
        // 计算最长名字的长度，用于打印help填充对齐
        std::vector<Opt> &opts = _CmdOptionListMap[name];
        size_t longestOptName = 0;
        for (size_t i = 0; i < opts.size(); ++i)
        {
            Opt &opt = opts[i];
            size_t len = strlen(opt.name);
            if (longestOptName < len)
            {
                longestOptName = len;
            }
        }

        printf("Usage:\n");
        printf("  %s %s", _CmdName.c_str(), name.c_str());
        for (size_t i = 0; i < opts.size(); ++i)
        {
            Opt &opt = opts[i];
            if (!opt.optional)
            {
                printf(" <%s>", opt.name);
            }
            else
            {
                printf(" [%s]", opt.name);
            }
        }
        printf("\n\n");

        printf("Available Options:\n");
        for (size_t i = 0; i < opts.size(); ++i)
        {
            Opt &opt = opts[i];
            size_t len = strlen(opt.name);
            std::string padding;
            if (len < longestOptName)
            {
                padding.append(longestOptName - len, ' ');
            }
            printf(" %s%s  %s  (%s)\n", opt.name, padding.c_str(), opt.desc, (!opt.optional?"required":"optional"));
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
    cmdhub(): _good(true), _readyCallbackPtr(NULL), _readyCallbackLine(0) {}
    cmdhub(const cmdhub&);
    cmdhub& operator=(const cmdhub&);
    ~cmdhub(){}

    static cmdhub* me()
    {
        static cmdhub hub;
        return &hub;
    }

    std::string getFileName(const std::string& path)
    {
        size_t pos = path.find_last_of('/');
        std::string filename = path.substr(pos+1);

        pos = filename.find_last_of('.');
        std::string name = filename.substr(0, pos);
        return name;
    };

private:
    bool                                                           _good;
    std::string                                                 _CmdName;
    std::map<std::string, Routine>                        _CmdRoutineMap;
    std::map<std::string, std::string>                _CmdDescriptionMap;
    std::map< std::string, std::vector<Opt> >          _CmdOptionListMap;
    std::map<std::string, const char*>                  _CmdOptionPtrMap;

    std::string                                        _lastAddedCmdName;

    Routine                                            _readyCallbackPtr;
    std::string                                       _readyCallbackName;
    std::string                                       _readyCallbackFile;
    int                                               _readyCallbackLine;
};

#endif
