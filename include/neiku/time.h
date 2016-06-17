// vim:ts=4:sw=4:expandtab

/*
 * file:   neiku/time.h
 * desc:   时间工具
 * author: YIF
 * date:   2016/6/17
 */

#ifndef __TIME_H__
#define __TIME_H__

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

namespace neiku
{
namespace util
{

// 获取毫秒时间戳
uint64_t timems();

// 获取微秒时间戳
uint64_t timeus();

};
};

#endif
