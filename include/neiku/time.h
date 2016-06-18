// vim:ts=4:sw=4:expandtab

/*
 * file:   neiku/time.h
 * desc:   时间工具
 * author: YIF
 * date:   2016/6/17
 */

#ifndef __TIME_H__
#define __TIME_H__

#include <time.h>
#include <string>

namespace neiku
{
namespace util
{

// x86_64: time_t => long unsigned int (uint64_t)

// 获取毫秒时间戳
time_t timems();

// 获取微秒时间戳
time_t timeus();

// 秒时间戳转时间串
std::string timestr(time_t sec, const char* fmt);

// 时间串转秒时间戳
time_t timestamp(const char* str, const char* fmt);

};
};

#endif
