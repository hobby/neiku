
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <neiku/proc.h>
#include <neiku/cmdline.h>
using namespace std;
using namespace yaf::util;

#define TIME_CYCLE_DEFAULT "5"

void show_help()
{
    printf("usage: ./proc_test [--netio-info] [--net-card=<网卡名称>] [--time-cycle=<采集周期(s)>] [-h | --help]\n");
}

void show_net_io(const char* pszNetCard, uint32_t dwTimeCycle)
{
    proc_t::net_t::dev_t::interface_t interface_prev, interface_curr;
    interface_prev = PROC->net.dev.interface(pszNetCard);
    while (true)
    {
        sleep(dwTimeCycle);
        interface_curr = PROC->net.dev.interface(pszNetCard);
        
        uint64_t ddwDeltaInBytes = interface_curr.receive.bytes - interface_prev.receive.bytes;
        uint64_t ddwDeltaInPackets = interface_curr.receive.packets - interface_prev.receive.packets;
        
        uint64_t ddwDeltaOutBytes = interface_curr.transimt.bytes - interface_prev.transimt.bytes;
        uint64_t ddwDeltaOutPackets = interface_curr.transimt.packets - interface_prev.transimt.packets;
        
        printf("bits-in(KB/s):[%lu/%0.3f]"
               ", pkgs-in:[%lu]"
               ", bits-out(KB/s):[%lu/%0.3f]"
               ", pkgs-out:[%lu]\n"
               , ddwDeltaInBytes/dwTimeCycle
               , ddwDeltaInBytes/dwTimeCycle/1000.0
               , ddwDeltaInPackets/dwTimeCycle
               , ddwDeltaOutBytes/dwTimeCycle
               , ddwDeltaOutBytes/dwTimeCycle/1000.0
               , ddwDeltaOutPackets/dwTimeCycle);
        
        interface_prev = interface_curr;
    }
}

int main(int argc, char **argv)
{
    int iRet = CMDLINE->Init(argc, argv, "--netio-info,--net-card:,--time-cycle:,-h|--help");
    if (iRet != 0)
    {
        printf("init cmdline fail, ret:[%d], msg:[%s]\n"
               , iRet, CMDLINE->GetErrMsg());
        show_help();
        return iRet;
    }
    
    if (CMDLINE->HasOpt("--netio-info")) 
    {
        uint32_t dwTimeCycle = strtoul(CMDLINE->GetOpt("--time-cycle", TIME_CYCLE_DEFAULT), NULL, 0);
        if (dwTimeCycle == 0 || (dwTimeCycle & 0x80000000) != 0)
        {
            dwTimeCycle = strtoul(TIME_CYCLE_DEFAULT, NULL, 0);
        }
        
        const char* pszNetCard = CMDLINE->GetOpt("--net-card", "eth1");
        
        printf("--- proc test: show netio info, net-card:[%s], time-cycle:[%u] ---\n"
               , pszNetCard, dwTimeCycle);
        show_net_io(pszNetCard, dwTimeCycle);
    }
    else if (CMDLINE->GetOpt("--help", NULL) != NULL)
    {
        show_help();
    }
    else
    {
        show_help();
    }

    return 0;
}

