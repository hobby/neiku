// vim:ts=4:sw=4:expandtab

/*
 * file:   neiku/cgx_main.h
 * desc:   CGX入口函数, 根据命令行切换CGI/FastCGI运行模式
 * author: YIF
 * date:   2016/10/03 22:26:00
 * version:
 *         2016/10/03 建立模型，通过--bind选项识别运行模式(有:fastcgi; 无:cgi)
 */

#ifndef __NEIKU_CGX_MAIN_H__
#define __NEIKU_CGX_MAIN_H__ 1

namespace neiku
{

// CGX入口函数: 根据命令行切换CGI/FastCGI运行模式
// 命令行：
// --bind=<usock-filename>
// 返回：
// 成功时有返回，失败时直接exit退出程序
void cgx_main(int argc, char* argv[]);

};

#endif
