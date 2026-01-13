/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * MyObserver.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: MyObserver.cpp 5884 2013-02-17 01:52:58Z WuJunjie $
 *
 *  Explain:
 *     -
 *          用自定义的信号、槽实现观察者模式；同时示例了消息发送和接收
 *          CProducers -- 主动对象
 *          CObserver  -- 观察对象
 *          CConsumers -- 消息收取对象
 *     -
 *
 *  Update:
 *     2013-02-17 01:52:58  Create
 *     2013-03-13 9:13:20   加入观察者对象
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "CommonInclude.h"
#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

#include <string>

#define g_Producers (*CProducers::instance())
class CProducers : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CProducers);
    typedef TSignal4<int, int *, int, time_t> ProducersSignal_t;
    // cmd, packet, ch, user
    typedef ProducersSignal_t::SigProc ProducersSignalProc_t;

    BOOL Start();
    BOOL Stop();

    BOOL Start(CEZObject * pObj, ProducersSignalProc_t pProc);
    BOOL Stop(CEZObject * pObj, ProducersSignalProc_t pProc);

    void ThreadProc();

private:
    CProducers();
    virtual ~CProducers();

    //ProducersSignalProc_t m_SigBuffer;
    ProducersSignal_t m_SigBuffer;

    CEZMutex m_MutexSigBuffer;

    //计数
    int				m_iUser;
};

#define g_Consumers (*CConsumers::instance())
class CConsumers : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CConsumers);

    BOOL Start();
    BOOL Stop();
    void ThreadProc();
private:
    CConsumers();
    virtual ~CConsumers();
};

class CObserver : public CEZObject
{
    std::string m_strName;

public:
    void OnDownload(int iCMD, int *pPacket, int iCh, time_t time)
    {
        __fline;
        printf(" %s CALLBACK - OnDownload: %d, 0x%0x, %d, %ld\n", m_strName.c_str(), iCMD, (unsigned int)pPacket, iCh, time);
    }

    void Start()
    {
        g_Producers.Start(this, (CProducers::ProducersSignalProc_t)&CObserver::OnDownload);
    }

    CObserver(std::string name="CObserver")
    {
        m_strName = name;
    }
    ;
    virtual ~CObserver()
    {}
    ;

};


int main(int argc, char* argv[])
{
    int iRet = 0;

    g_TimerManager.Start();
    g_ThreadManager.RegisterMainThread(ThreadGetID());	// 注册主线程

    // 先启动
    g_Consumers.Start();

    CObserver __withCallback("tom");
    __withCallback.Start();

    CObserver __withCallback_bob("bob");
    __withCallback_bob.Start();

    CEZSemaphore sem;
    sem.Wait();

    printf("%s end.\n", argv[0]);

    return iRet;
}

PATTERN_SINGLETON_IMPLEMENT(CProducers);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CProducers::CProducers() :CEZThread("CProducers", TP_DEFAULT), m_SigBuffer(2/*SIGNAL_NODE_NEW*/)
{
    m_iUser = 0;
    trace("CProducers Enter--------\n");
}

CProducers::~CProducers()
{
    trace("CProducers Leave--------\n");

}

BOOL CProducers::Start(CEZObject * pObj, ProducersSignalProc_t pProc)
{
    CEZLock __lock(m_MutexSigBuffer);

    BOOL bRet = FALSE;

    if(m_SigBuffer.Attach(pObj, pProc) < 0)
    {
        __fline;
        printf("attach error\n");
        return bRet;
    }

    if (m_iUser==0)
    {
        CreateThread();

        bRet = TRUE;
    }
    else
    {
        __trip;

        bRet = TRUE;
    }
    m_iUser++;

    __fline;
    printf("m_iUser:%d\n", m_iUser);

    return bRet;

}
BOOL CProducers::Stop(CEZObject * pObj, ProducersSignalProc_t pProc)
{
    printf("CDevHDiskReader::Stop\n");

    BOOL bRet = DestroyThread(TRUE);
    CEZLock __lock(m_MutexSigBuffer);
    m_iUser--;

    if(m_SigBuffer.Detach(pObj, pProc) == 0)
    {
        // good
    }
    else
    {
        // error
        __trip;
    }

    return bRet;
}

void CProducers::ThreadProc()
{
    int ii=0;
    while (m_bLoop)
    {
        //__trip;

        // 发送消息 (unsigned int msg, PARAM wpa = 0, PARAM lpa = 0, unsigned int priority = 0);
        g_Consumers.SendMessage(100, ii, ii-1, 0);

        // 回调
        m_SigBuffer(/*HDISKREADER_CMD_FINISHED*/1, NULL, ii, time(NULL));

        ii++;

        usleep(100);
    }

    //delete cfgnetcommon;
}



PATTERN_SINGLETON_IMPLEMENT(CConsumers);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CConsumers::CConsumers() :CEZThread("CConsumers", TP_DEFAULT, 100)
{
    trace("CConsumers Enter--------\n");
}

CConsumers::~CConsumers()
{
    trace("CConsumers Leave--------\n");

}

BOOL CConsumers::Start()
{
    if (m_bLoop)
    {
        return TRUE;
    }
    int ret = CreateThread();

    return ret;
}

BOOL CConsumers::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = FALSE;
        DestroyThread();
    }
    return TRUE;
}
void CConsumers::ThreadProc()
{
    MSG_NODE msg;

    static time_t __ttLastTimeProc = time(NULL);

    while (m_bLoop)
    {
        if (FALSE == RecvMessage(&msg, TRUE))
        {
            __trip ;
            sleep(1);
            continue;
        }

        //g_ThreadManager.DumpThreads();

        if (__ttLastTimeProc!=time(NULL))
        {
            __fline;
            printf("RecvMessage msg.msg:%d, round -> msg.wpa:%d, msg.lpa:%d, msg.time:%d\n", msg.msg, msg.wpa, msg.lpa, msg.time);

            __ttLastTimeProc = time(NULL);
        }
    }
}
