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

// 结构体乱序赋值（C++）出现：不支持（网上很多说明，只有c支持，c++要c++14可能会支持，但其实跟编译器有关系）
// https://www.systutorials.com/241133/g-sorry-unimplemented-non-trivial-designated-initializers-not-supported/
// 关键：乱序目标是知道值赋给了哪个字段，只要字段顺序与定义顺序一致，是可以的（g++ 4.8.5）

class cmdbar
{
public:
    cmdbar(): _good(true){}

    typedef int (*Routine)();

    struct Cmd
    {
        // 不能有构造函数，否则乱序赋值方式编不过

        Routine      exec;      // 命令入口
        const char*  desc;      // 命令描述
    };

    struct CmdOption
    {
        const char*  name;      // 选项名字
        const char*  desc;      // 选项描述
        bool         optional;  // 是否可选（默认必填字段）

        bool         _good;     // 选项是否ok（只对必填字段有用）
    };

public:
    // 启动命令行
    static int launch(int argc, char* argv[])
    {
        cmdbar* _this = me();

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
        std::map< std::string, std::vector<CmdOption> >::iterator opts = _this->_CmdOptionListMap.find(name);
        if (opts != _this->_CmdOptionListMap.end())
        {
            // 填充参数
            int idx = 2;
            for (size_t i = 0; i < opts->second.size(); ++i)
            {
                if (idx < argc)
                {
                    CmdOption &opt = opts->second[i];
                    opt._good = true;
                    _this->_CmdOptionPtrMap[opt.name] = argv[idx];
                    idx++;
                }
            }

            // 检测是否没填`必填参数`
            for (size_t i = 0; i < opts->second.size(); ++i)
            {
                CmdOption &opt = opts->second[i];
                if (!opt.optional && !opt._good)
                {
                    _this->printError("Error: '%s' is required for '%s' command\n", opt.name, name.c_str());
                    return 1;
                }
            }
        }

        // 执行命令
        Routine routine = cmd->second;
        return routine();
    }

public:
    // 注册命令、选项，支持链式操作
    // 例如：cmdbar::addCmd(Cmd{}).addOpt(CmdOption{}).addOpt(CmdOption{});
    #define addCmd(CMD, args...)  doAddCmd(__FILE__, __LINE__, CMD, ##args)

    static cmdbar& doAddCmd(const char* file, int line, cmdbar::Cmd cmd)
    {
        cmdbar* _this = me();

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

    cmdbar& addOpt(cmdbar::CmdOption opt)
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

        std::vector<CmdOption> &opts = _CmdOptionListMap[_lastAddedCmdName];
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

private:
    static void _copyVal(const std::string& name, std::string& value)
    {
        cmdbar* _this = me();
        value = "";
        std::map<std::string, const char*>::iterator itOpt = _this->_CmdOptionPtrMap.find(name);
        if (itOpt != _this->_CmdOptionPtrMap.end() && itOpt->second != NULL)
        {
            value = std::string(itOpt->second);
        }
    }

    #define _COPYVAL4NUM(TYPE, FUNC) static void _copyVal(const std::string& name, TYPE& value) \
    { \
        cmdbar* _this = me(); \
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
            std::vector<CmdOption> &opts = _CmdOptionListMap[it->first];
            for (size_t i = 0; i < opts.size(); ++i)
            {
                CmdOption &opt = opts[i];
                if (opt.optional)
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
        printf("Usage:\n");
        printf("  %s %s", _CmdName.c_str(), name.c_str());
        std::vector<CmdOption> &opts = _CmdOptionListMap[name];
        for (size_t i = 0; i < opts.size(); ++i)
        {
            CmdOption &opt = opts[i];
            if (opt.optional)
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
            CmdOption &opt = opts[i];
            printf(" %s\t%s (%s)\n", opt.name, opt.desc, (!opt.optional?"required":"optional"));
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
    std::map< std::string, std::vector<CmdOption> >    _CmdOptionListMap;
    std::map<std::string, const char*>                  _CmdOptionPtrMap;

    std::string                                        _lastAddedCmdName;
};

#endif