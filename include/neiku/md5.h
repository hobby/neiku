
/*
 * file:   yaf/md5.h
 * desc:   md5 sum tools.
 * author: yifee
 * date:   2014/9/19 23:36:00
 *
 * usage:
 *       (1) #include <yaf/md5.h>
 *       (2) cout << "md5sum(helloworl)" << yaf::util::Md5Sum("helloworld") << endl;
 *       (2) g++ xxx.cpp -lyaf_util
 */

#ifndef __MD5_H__
#define __MD5_H__

namespace yaf
{
namespace util
{

// ¶Ô×Ö·û´®Çómd5
std::string Md5Sum(const std::string& s);

};
};

#endif
