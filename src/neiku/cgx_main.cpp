// vim:ts=4:sw=4:expandtab

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <string>

#include "neiku/cgx_main.h"

#define FASTCGI_SOCKET_FD 0

static int cgx_parse(int argc, char* argv[], std::map<const char*, std::string>& opts)
{
    // cmdline config
    const struct option longopts[] = {
        {"bind", required_argument, NULL, 0}
      , {"backlog", required_argument, NULL, 0}
      , {"fork", required_argument, NULL, 0}
      , {0, 0, 0, 0}
    };

    // disable stderr in getopt_long
    opterr = 0;

    // do parse
    while (true)
    {
        int idx = 0;
        int opt = getopt_long(argc, argv, ":", longopts, &idx);
        if (opt == -1)
        {
            break;
        }
        else if (opt == 0)
        {
            opts[longopts[idx].name] = optarg ? optarg : "";
        }
    }
    
    return 0;
}

static int cgx_bind(std::string& filename, int backlog)
{
    // new socket
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
    {
        fprintf(stderr, "socket fail, msg:[%s]\n", strerror(errno));
        return -1;
    }

    // bind socket on filename
    int ret = 0;
    do
    {
        struct sockaddr_un sockaddr;
        memset(&sockaddr, 0, sizeof(sockaddr));

        sockaddr.sun_family = AF_UNIX;
        strcpy(sockaddr.sun_path, filename.c_str());

        socklen_t socklen = SUN_LEN(&sockaddr);

        ret = bind(fd, (struct sockaddr*)(&sockaddr), socklen);
        if (ret != 0)
        {
            fprintf(stderr, "bind fail, filename:[%s], msg:[%s]\n", filename.c_str(), strerror(errno));   
            break;
        }

        ret = listen(fd, backlog);
        if (ret != 0)
        {
            fprintf(stderr, "listen fail, filename:[%s], msg:[%s]\n", filename.c_str(), strerror(errno));   
            break;
        }

        ret = 0;
    } while (0);
    if (ret != 0)
    {
        close(fd);
        return -1;
    }

    // set fastcgi-socketfd with fd
    // only stdin used, stdout/stderr nouse
    // fd == FASTCGI_SOCKET_FD is ok (man dup2)
    ret = dup2(fd, FASTCGI_SOCKET_FD);
    if (ret < 0)
    {
        fprintf(stderr, "set fastcgi sockfd fail, filename:[%s], msg:[%s]"
                        ", fd:[%d], fastcgi-sockfd:[%d]\n"
                        , filename.c_str(), strerror(errno)
                        , fd, FASTCGI_SOCKET_FD);
        return -1; 
    }

    // close un-use fd
    close(fd);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    return 0;
}

int cgx_fork(int number)
{
    int father = 1;

    for (int i = 0; i < number; ++i)
    {
        int pid = fork();
        if (pid == 0)
        {
            father = 0;
            daemon(1, 1);
            break;
        }
        else if (pid == -1)
        {
            fprintf(stderr, "fork fail, number:[%d], msg:[%s]\n"
                            , number, strerror(errno));
            return -1;
        }
    }

    if (father)
    {
        exit(0);
    }

    return 0;
}

void neiku::cgx_main(int argc, char* argv[])
{
    int ret = 0;

    // 解析命令行
    std::map<const char*, std::string> opts;
    ret = cgx_parse(argc, argv, opts);
    if (ret != 0)
    {
        exit(ret);
    }

    // FIXME: 先查询，让cgi模式不用多申请内存
    // 根据bind选项决定运行模式
    if (opts.find("bind") == opts.end())
    {
        // CGI
        return;
    }
    // FastCGI

    // 初始化backlog
    // 优先从命令行获取，否则从文件系统获取，默认5
    int backlog = strtol(opts["backlog"].c_str(), NULL, 0);
    if (backlog <= 0)
    {
        backlog = 5;
    }

    // 初始化监听socket
    ret = cgx_bind(opts["bind"], backlog);
    if (ret != 0)
    {
        exit(ret);
    }

    // 初始化守护进程
    int number = strtol(opts["fork"].c_str(), NULL, 0);
    if (number <= 0)
    {
        number = 1;
    }
    ret = cgx_fork(number);
    if (ret != 0)
    {
        exit(ret);
    }

    return;
}