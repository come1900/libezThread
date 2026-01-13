/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ThreadMsg.cpp - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: ThreadMsg.cpp 5884 2012-05-16 09:17:46Z WuJunjie $
 *
 *  Explain:
 *     -
 *      生产者和消费者的例子
 *        g_Producers  ----- 生产者 
 *        CTimerProducers -- 定时器生产者
 *        g_Consumers  ----- 消费者
 *     -
 *
 *  Update:
 *     2012-05-16 09:17:46   WuJunjie  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "CommonInclude.h"

#include "EZThread.h"
#include "EZTimer.h"

#define MSG_TYPE_PRODUCER 100
#define MSG_TYPE_TIMER 101

#define g_Producers (*CProducers::instance())

class CProducers : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CProducers);

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
    void ThreadProc();
private:
    CProducers();
    virtual ~CProducers();
};

#define g_Consumers (*CConsumers::instance())
class CConsumers : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CConsumers);

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
    void ThreadProc();
private:
    CConsumers();
    virtual ~CConsumers();
};

class CTimerProducers : public CEZObject
{
public:
    void TimerProc()
    {
        if (g_Consumers.IsRunning())
        {
            g_Consumers.SendMessage(MSG_TYPE_TIMER);
        }
        else
        {
            __fline;
            printf("g_Consumers Is not Running, skip now.\n");
        }
    }
    void Start()
    {
        CEZTimer *_pTimerProducers;
        _pTimerProducers = new CEZTimer("_pTimerProducers");
        _pTimerProducers->Start(this
                                , (EZ_TIMER_PROC)&CTimerProducers::TimerProc
                                , 0
                                , 3 * 1000 /*period*/
                                , 0
                                , 10*1000
                               );
    }

    CTimerProducers()
    {
        ;
    }
    virtual ~CTimerProducers()
    {
        ;
    }
};
//
int main(int argc, char* argv[])
{
    int iRet = 0;

    // 定时器开启
    g_TimerManager.Start();
    // 注册主线程
    g_ThreadManager.RegisterMainThread(ThreadGetID());

    printf("%s end.\n", argv[0]);

    // 启动消费者线程
    g_Consumers.Start();
    // 启动生产者线程
    g_Producers.Start();

    CTimerProducers TimerProducers;
    TimerProducers.Start();

    // waite forever, for nobody send to me.
    CEZSemaphore sem;
    sem.Wait();

    return iRet;
}


PATTERN_SINGLETON_IMPLEMENT(CProducers);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CProducers::CProducers() :CEZThread("CProducers", THREAD_PRIORITY_DEFAULT)
{
    printf("CProducers Enter--------\n");
}

CProducers::~CProducers()
{
    printf("CProducers Leave--------\n");
}

EZTHREAD_BOOL CProducers::Start()
{
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }
    int ret = CreateThread();

    return ret;
}

EZTHREAD_BOOL CProducers::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }
    return EZTHREAD_BOOL_TRUE;
}

void CProducers::ThreadProc()
{
    while (m_bLoop)
    {
        // send per second
        if (g_Consumers.IsRunning())
        {
            g_Consumers.SendMessage(MSG_TYPE_PRODUCER);
        }
        else
        {
            __fline;
            printf("g_Consumers Is not Running, skip now.\n");
        }

        //sleep(1);
    }
}

PATTERN_SINGLETON_IMPLEMENT(CConsumers);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CConsumers::CConsumers() :CEZThread("CConsumers", THREAD_PRIORITY_DEFAULT, 100)
{
    printf("CConsumers Enter--------\n");

}

CConsumers::~CConsumers()
{
    printf("CConsumers Leave--------\n");

}

EZTHREAD_BOOL CConsumers::Start()
{
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }
    int ret = CreateThread();

    return ret;
}

EZTHREAD_BOOL CConsumers::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }
    return EZTHREAD_BOOL_TRUE;
}
int abc()
{
__trip;
return 0;
}

void CConsumers::ThreadProc()
{
    MSG_NODE msg;

    while (m_bLoop)
    {
        //__trip;
        if (EZTHREAD_BOOL_FALSE == RecvMessage(&msg, EZTHREAD_BOOL_TRUE))
        {
            __trip ;
            sleep(1);
            continue;
        }
//abc();
        switch(msg.msg)
        {
            case MSG_TYPE_PRODUCER:
            {
                printf("\nReceive msg from PRODUCER\n");
                g_ThreadManager.DumpThreads();

                break;
            }
            case MSG_TYPE_TIMER:
            {
                printf("\nReceive msg from TIMER\n");
                g_TimerManager.DumpTimers();

                break;
            }
            default:
            {
                break;
            }
        }

        // __trip;
    }
}
