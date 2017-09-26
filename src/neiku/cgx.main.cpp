// vim:ts=4:sw=4:expandtab

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <string>

#include "neiku/cgx.h"

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

static int cgx_mkdir(const char* filename)
{
    std::string path = filename;
    size_t len = path.size();
    for (size_t i = 1; i < len; ++i)
    {
        if (path[i] != '/')
        {
            continue;
        }

        errno = 0;
        path[i] = '\0';
        int ret = mkdir(path.c_str(), 0777);
        if (ret != 0 && errno != EEXIST)
        {
            fprintf(stderr, "mkdir fail, filename:[%s], path:[%s], errno:[%d], errmsg:[%s]"
                    , filename, path.c_str(), errno, strerror(errno));
            return -1;
        }
        path[i] = '/';
    }

    return 0;
}

static int cgx_bind(const char* filename, int backlog)
{
    // new socket
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
    {
        fprintf(stderr, "socket fail, errmsg:[%s]\n", strerror(errno));
        return -1;
    }

    // check if already inused
    struct sockaddr_un sockaddr;
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sun_family = AF_UNIX;
    strcpy(sockaddr.sun_path, filename);

    errno = 0;
    socklen_t socklen = SUN_LEN(&sockaddr);
    int ret = connect(fd, (struct sockaddr*)(&sockaddr), socklen);
    if (ret == 0 || errno == EACCES)
    {
        close(fd);
        fprintf(stderr, "socket already inused, filename:[%s], errno:[%d], errmsg:[%s]\n"
                , filename, errno, strerror(errno));
        return -1;
    }

    // cleanup old socket file
    // so that we can listen on it (create new socket file)
    errno = 0;
    ret = unlink(filename);
    if (ret != 0 && errno != ENOENT)
    {
        close(fd);
        fprintf(stderr, "cleanup old socket fail, filename:[%s], errno:[%d], errmsg:[%s]\n"
                , filename, errno, strerror(errno));
        return -1;
    }

    // auto create component directory
    ret = cgx_mkdir(filename);
    if (ret != 0)
    {
        close(fd);
        return -1;
    }

    // bind socket on filename
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sun_family = AF_UNIX;
    strcpy(sockaddr.sun_path, filename);

    socklen = SUN_LEN(&sockaddr);
    ret = bind(fd, (struct sockaddr*)(&sockaddr), socklen);
    if (ret != 0)
    {
        close(fd);
        fprintf(stderr, "bind fail, filename:[%s], errno:[%d], errmsg:[%s]\n"
                , filename, errno, strerror(errno));
        return -1;
    }

    // listen socket
    ret = listen(fd, backlog);
    if (ret != 0)
    {
        close(fd);
        fprintf(stderr, "listen fail, filename:[%s], errno:[%d], errmsg:[%s]\n"
                , filename, errno, strerror(errno));
        return -1;
    }

    // chmod perm
    // so that everone can connect me
    ret = chmod(filename, 0666);
    if (ret != 0)
    {
        close(fd);
        fprintf(stderr, "chmod fail, filename:[%s], errno:[%d], errmsg:[%s]\n"
                , filename, errno, strerror(errno));
        return -1;
    }

    // set fastcgi-socketfd with fd
    // and only stdin used
    // fd == FASTCGI_SOCKET_FD is ok (man dup2)
    ret = dup2(fd, FASTCGI_SOCKET_FD);
    if (ret < 0)
    {
        close(fd);
        fprintf(stderr, "set fastcgi sockfd fail, filename:[%s], errno:[%d], errmsg:[%s]"
                        ", fd:[%d], fastcgi-sockfd:[%d]\n"
                        , filename, errno, strerror(errno)
                        , fd, FASTCGI_SOCKET_FD);
        return -1;
    }

    // close un-used fd
    close(fd);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    return 0;
}

static int cgx_fork(int number)
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
            fprintf(stderr, "fork fail, number:[%d], errmsg:[%s]\n"
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

/* usage:
 * - for cgi
 *   ./cgi
 * - for fastcgi
 *   ./fastcgi --bind=<path to sock> \
 *             --fork=<number of fastcgi daemon> \
 *             --backlog=<backlog for listen>
 */
void neiku::CgX::main(int argc, char* argv[])
{
    int ret = 0;

    std::map<const char*, std::string> opts;
    ret = cgx_parse(argc, argv, opts);
    if (ret != 0)
    {
        exit(ret);
    }

    if (opts.find("bind") == opts.end())
    {
        return;
    }

    int backlog = strtol(opts["backlog"].c_str(), NULL, 0);
    if (backlog <= 0)
    {
        backlog = 1024;
    }

    ret = cgx_bind(opts["bind"].c_str(), backlog);
    if (ret != 0)
    {
        exit(ret);
    }

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
