/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Observer.cpp - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: Observer.cpp 5884 2013-02-17 01:52:58Z WuJunjie $
 *
 *  Explain:
 *     -
 *          Observer pattern 的示例
 *          另外有
 *     -
 *
 *  Update:
 *     2013-02-17 01:52:58  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "CommonInclude.h"
#include "EZThread.h"
#include "EZTimer.h"
#include <iostream>
#include <string>
#include <sigc++/sigc++.h>

//#define g_Producers (*CProducers::instance())
class CProducers : public CEZThread
{
public:
    typedef sigc::signal<void, int, int *, int, time_t> SIG_DEV_HDISK_READER;

private:

    //SIG_DEV_HDISK_READER m_sigBuffer;
    SIG_DEV_HDISK_READER m_sigBuffer;

    CEZMutex m_CMutex;

    //计数
    int				m_iUser;

public:
    PATTERN_SINGLETON_DECLARE(CProducers);
    // cmd, packet, ch, user
    //typedef TSignal4<int, int *, int, time_t>::SigProc SIG_DEV_HDISK_READER;

    SIG_DEV_HDISK_READER::iterator Start(const SIG_DEV_HDISK_READER::slot_type&slot_)
    {
        CEZLock __lock(m_CMutex);

        SIG_DEV_HDISK_READER::iterator __it = m_sigBuffer.connect(slot_);


        if (m_iUser==0)
        {
            CreateThread();
        }
        else
        {
            __trip;
        }
        m_iUser++;

        __fline;
        printf("m_iUser:%d\n", m_iUser);

        return __it;

    }
    EZTHREAD_BOOL Stop(SIG_DEV_HDISK_READER::iterator);

    void ThreadProc();

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

class CObserver : public sigc::trackable
{
    std::string m_strName;

public:
    void OnDownload(int iCMD, int *pPacket, int iCh, time_t time)
    {
        //static time_t __ttLastTime = time;

        //if (__ttLastTime != time)
        //{
            __fline;
            printf(" %s CALLBACK - OnDownload: %d, 0x%0x, %d, %ld\n", m_strName.c_str(), iCMD, pPacket, iCh, time);
            //__ttLastTime = time;
        //}
    }

//    void Start()
//    {
//        g_Producers.Start(sigc::mem_fun(this, &CObserver::OnDownload));
//    }

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
    CProducers __prod;


    // 先启动
    g_Consumers.Start();

    CObserver __withCallback_tom("tom");
    //__withCallback.Start();

    CObserver __withCallback_bob("bob");
    //__withCallback_bob.Start();

    CProducers::SIG_DEV_HDISK_READER::iterator __iter_bob
    = __prod.Start(sigc::mem_fun(__withCallback_bob, &CObserver::OnDownload));
    
    CProducers::SIG_DEV_HDISK_READER::iterator __iter_tom
    = __prod.Start(sigc::mem_fun(__withCallback_tom, &CObserver::OnDownload));


    CEZSemaphore sem;
    sem.Wait();

    printf("%s end.\n", argv[0]);

    return iRet;
}

//PATTERN_SINGLETON_IMPLEMENT(CProducers);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CProducers::CProducers() :CEZThread("CProducers", THREAD_PRIORITY_DEFAULT)
{
    m_iUser = 0;
    printf("CProducers Enter--------\n");
}

CProducers::~CProducers()
{
    printf("CProducers Leave--------\n");

}

EZTHREAD_BOOL CProducers::Stop(SIG_DEV_HDISK_READER::iterator it_)
{
    printf("CDevHDiskReader::Stop\n");

    EZTHREAD_BOOL bRet = DestroyThread(EZTHREAD_BOOL_TRUE);
    //CEZGuard guard(m_CMutex);
    CEZLock __lock(m_CMutex);
    m_iUser--;

    it_->disconnect();

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
        //m_sigBuffer(/*HDISKREADER_CMD_FINISHED*/1, NULL, ii, time(NULL));
        m_sigBuffer.emit(/*HDISKREADER_CMD_FINISHED*/1, NULL, ii, time(NULL));
        ii++;

        //usleep(10);
        sleep(1);
    }

    //delete cfgnetcommon;
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
void CConsumers::ThreadProc()
{
    MSG_NODE msg;

    static time_t __ttLastTimeProc = time(NULL);

    while (m_bLoop)
    {
        if (EZTHREAD_BOOL_FALSE == RecvMessage(&msg, EZTHREAD_BOOL_TRUE))
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
