
#include "neiku/log.h"
#include "neiku/msq.h"

void signal_user1(int i)
{
    cout << "catch signal user1" << endl;

    return ;
}


int main(int argc, char* argv[])
{
    cout << "EINTR = " << EINTR << ", " << "EINVAL = " << EINVAL << endl;
    signal(SIGUSR1, signal_user1);

    if(argc < 2)
    {
        cout << "usage: " << argv[0] << " key send|recv [msg]" << endl;
        return -1;
    }

    int iRet = 0;
    key_t msgkey = strtoul(argv[1], NULL, 0);
    std::string msgact;
    if(2 < argc)
    {
        msgact = argv[2];
    }
    std::string msg;
    if(3 < argc)
    {
        msg = argv[3];
    }

    CMsq oMsq;
    iRet = oMsq.Init(msgkey);
    if (iRet != 0)
    {
        cout << "msq init fail, ret = " << iRet
             << ", msg = " << oMsq.GetLastMsg() << endl;
        return -1;
    }

    if(msgact.compare("send") == 0)
    {
        iRet = oMsq.SendMsg(msg);
        if (iRet != 0)
        {
            cout << "msq send fail, ret = " << iRet
                 << ", msg = " << oMsq.GetLastMsg() << endl;
            return -1;
        }
        cout << "msq send succ, msg = " << oMsq.GetLastMsg() << endl;

        return 0;
    }

    else if(msgact.compare("recv") == 0)
    {
        std::string sMsg;
        iRet = oMsq.RecvMsg(sMsg);
        if (iRet != 0)
        {
            cout << "msq recv fail, ret = " << iRet
                 << ", msg = " << oMsq.GetLastMsg() << endl;
            return -1;
        }
        cout << "msq recv succ, msg = " << oMsq.GetLastMsg() << endl;
        cout << "recv msg => " << sMsg << endl;

        return 0;
    }

    else
    {
        cout << "unknow msg action, action = " << msgact << endl;
        return 0;
    }

    return 0;
}
