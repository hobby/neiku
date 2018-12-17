
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <iostream>

using namespace std;


class CMsq
{
    public:
        enum EErrCode
        {
            MSQ_ERRCODE_MSGTOOBIG = 1,
            MSQ_ERRCODE_NEEDAGAIN = 2,
        };

    public:
        #define JDZT_MSQ_MSGMAXSIZE 1024*1024UL
        struct SMsgBuf
        {
            long mtype;
            char mtext[JDZT_MSQ_MSGMAXSIZE];
        };

    public:
        CMsq(): m_bInit(false)
              , m_tKey(0)
              , m_iMsqId(0)
              , m_iMsgFlag(0)
        {};

        // 初始化
        int Init(key_t tKey, int iMsgFlag = IPC_CREAT | 0666)
        {
            m_bInit = false;

            m_tKey = tKey;
            m_iMsgFlag = iMsgFlag;

            m_iMsqId = msgget(m_tKey, m_iMsgFlag);
            if (m_iMsqId == -1)
            {
                char *str = strerror_r(errno, m_szStrError, sizeof(m_szStrError));
                snprintf(m_szLastErrMsg, sizeof(m_szLastErrMsg)
                         , "msgget fail, key:[%#010x], ret:[%d], msg:[%s]"
                         , m_tKey, m_iMsqId, str);
                return -1;
            }

            m_bInit = true;
            m_szLastErrMsg[0] = 0;
            return 0;
        };

        // 接收消息(阻塞模式)
        int RecvMsg(std::string& sMsg)
        {
            if (m_bInit == false)
            {
                snprintf(m_szLastErrMsg, sizeof(m_szLastErrMsg), "msq not init");
                return -1;
            }

            int iRet = 0;
            do
            {
                iRet = msgrcv(m_iMsqId, (struct msgbuf*)&m_sMsgBuf, sizeof(m_sMsgBuf.mtext), 0, 0);
                if (iRet == -1)
                {
                    if(errno == EIDRM)
                    {
                        iRet = Init(m_tKey, m_iMsgFlag);
                        if(iRet != 0)
                        {
                            break;
                        }
                        continue;
                    }
                    else if(errno == EINTR)
                    {
                        continue;
                    }
                    else
                    {
                        char *str = strerror_r(errno, m_szStrError, sizeof(m_szStrError));
                        snprintf(m_szLastErrMsg, sizeof(m_szLastErrMsg)
                                 , "msgrcv fail, ret:[%d], msg:[%s]"
                                 , iRet, str);
                        break;
                    }
                }

                sMsg.assign(m_sMsgBuf.mtext, iRet);

                iRet = 0;
                m_szLastErrMsg[0] = 0;
                break;
            } while (true);
            return iRet;
        }

        // 发送消息(非阻塞模式)
        int SendMsg(const std::string& sMsg)
        {
            if (m_bInit == false)
            {
                snprintf(m_szLastErrMsg, sizeof(m_szLastErrMsg), "msq not init");
                return -1;
            }

            if(JDZT_MSQ_MSGMAXSIZE < sMsg.size())
            {
                snprintf(m_szLastErrMsg, sizeof(m_szLastErrMsg)
                         , "msg too big, size:[%lu], maxsize:[%lu]"
                         , sMsg.size(), JDZT_MSQ_MSGMAXSIZE);
                return MSQ_ERRCODE_MSGTOOBIG;
            }
            
            m_sMsgBuf.mtype = getpid();
            memcpy(m_sMsgBuf.mtext, sMsg.c_str(), sMsg.size());

            int iRet = 0;
            do
            {
                iRet = msgsnd(m_iMsqId, (struct msgbuf*)&m_sMsgBuf, sMsg.size(), IPC_NOWAIT);
                if (iRet == -1)
                {
                    if(errno == EIDRM)
                    {
                        iRet = Init(m_tKey, m_iMsgFlag);
                        if(iRet != 0)
                        {
                            break;
                        }
                        continue;
                    }
                    else if(errno == EINTR)
                    {
                        continue;
                    }
                    else
                    {
                        char *str = strerror_r(errno, m_szStrError, sizeof(m_szStrError));
                        snprintf(m_szLastErrMsg, sizeof(m_szLastErrMsg)
                                 , "msgsnd fail, ret:[%d], msg:[%s]"
                                 , iRet, str);
                        break;
                    }
                }

                iRet = 0;
                m_szLastErrMsg[0] = 0;
                break;
            } while (true);
            return iRet;
        }

        const char* GetLastMsg()
        {
            return m_szLastErrMsg;
        }

    private:
        bool    m_bInit;
        key_t   m_tKey;
        int     m_iMsqId;
        int     m_iMsgFlag;
        SMsgBuf m_sMsgBuf;
        char    m_szLastErrMsg[256];
        char    m_szStrError[256];
};

