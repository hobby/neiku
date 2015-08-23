
/*
 * file:   yaf/proc.h
 * author: yifee
 * site:   http://haama.net
 * date:   2014/11/16 12:20:00
 * desc:
 * CProc v1.0
 * (1) 提供/proc子系统文件的访问，
 *     * /proc/net/dev
 * 
 * usage:
 * -----------------proc_test.cpp--------------------
 * #include <yaf/proc.h>
 * int main(int argc, char** argv)
 * {
 *     printf("current tcp established connection:[%u]\n"
 *            , PROC->net.snmp.tcp.CurrEstab);
 *
 *     return 0;
 * }
 * -----------------proc_test.cpp--------------------
 * compile:
 * $g++ -o proc_test proc_test.cpp -I${YAF_PROJ_PATH}/include -L${YAF_PROJ_PATH}/lib -lyaf_util
 */

#ifndef __PROC_H_
#define __PROC_H_

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <cctype>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <neiku/singleton.h>
#include <neiku/string.h>
#define PROC SINGLETON(yaf::util::proc_t)

namespace yaf
{
namespace util
{

// File => /proc/net/dev
class CDev
{
    public:
        typedef class CInterface
        {
            public:
                typedef class CReceive
                {
                    public:
                        uint64_t bytes;
                        uint64_t packets;
                        uint64_t errs;
                        uint64_t drop;
                        uint64_t fifo;
                        uint64_t frame;
                        uint64_t compressed;
                        uint64_t multicast;
                
                    public:
                        CReceive(): bytes(0)
                                  , packets(0)
                                  , errs(0)
                                  , drop(0)
                                  , fifo(0)
                                  , frame(0)
                                  , compressed(0)
                                  , multicast(0)
                        {};
                } receive_t;
                
                typedef class CTransimt
                {
                    public:
                        uint64_t bytes;
                        uint64_t packets;
                        uint64_t errs;
                        uint64_t drop;
                        uint64_t fifo;
                        uint64_t colls;
                        uint64_t carrier;
                        uint64_t compressed;
                
                    public:
                        CTransimt(): bytes(0)
                                   , packets(0)
                                   , errs(0)
                                   , drop(0)
                                   , fifo(0)
                                   , colls(0)
                                   , carrier(0)
                                   , compressed(0)
                        {};
                } transime_t;
            
            public:
                CReceive  receive;
                CTransimt transimt;
        } interface_t;
    
    public:
        // 功能：获取指定网卡信息，以CInterface对像保存，数据来自/proc/net/dev
        // (1) /proc/net/dev格式
        // -----------------------------------------------------*
        // Iteer|       Receive       |    Transimt
        // face | Receive相关字段(8个)  Transimt相关字段(8个)
        //  eth0: ...
        //  eth1: ...
        // -----------------------------------------------------
        // (2) 特别注意
        //    a、/proc/net/dev是时刻变化着的
        //    b、文件权限:444，用户:root，用户组:root，打开时只能使用读模式
        interface_t interface(const char* name)
        {
            interface_t oInterface;

            std::fstream oFile("/proc/net/dev", std::fstream::in);
            if (oFile.is_open())
            {
                std::string sLine;
                while (std::getline(oFile, sLine))
                {
                    std::vector<std::string> vLine;
                    STRING->Explode(vLine, sLine, ':');
                    
                    STRING->Trim(vLine[0]);
                    if (vLine[0].compare(name) == 0)
                    {
                        STRING->Trim(vLine[1]);
                        std::vector<uint64_t> vData;
                        STRING->Explode(vData, vLine[1], ' ');
                        
                        oInterface.receive.bytes       = vData[0];
                        oInterface.receive.packets     = vData[1];
                        oInterface.receive.errs        = vData[2];
                        oInterface.receive.drop        = vData[3];
                        oInterface.receive.fifo        = vData[4];
                        oInterface.receive.frame       = vData[5];
                        oInterface.receive.compressed  = vData[6];
                        oInterface.receive.multicast   = vData[7];
                        
                        oInterface.transimt.bytes      = vData[8];
                        oInterface.transimt.packets    = vData[9];
                        oInterface.transimt.errs       = vData[10];
                        oInterface.transimt.drop       = vData[11];
                        oInterface.transimt.fifo       = vData[12];
                        oInterface.transimt.colls      = vData[13];
                        oInterface.transimt.carrier    = vData[14];
                        oInterface.transimt.compressed = vData[15];
                        break;
                    }
                }
                oFile.close();
            }
            
            return oInterface;
        };
};

// File => /proc/net
class CNet
{
    public:
        typedef CDev dev_t;
        
    public:
        dev_t dev;
};

// File => /proc
typedef class CProc
{
    public:
        typedef CNet net_t;

    public:
        net_t net;
} proc_t;

};
};
#endif
