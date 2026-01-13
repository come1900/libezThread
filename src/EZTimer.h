/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * EZTimer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: EZTimer.h 5884 2012-05-16 09:08:50Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _EZTIMER_H_
#define _EZTIMER_H_

//#include "api_system.h"
#include "typedef_ezthread.h"
#include "api_ezthread.h"

#include "EZObject.h"
#include "EZThread.h"

#include "EZMutex.h"
#include "EZLock.h"
#ifdef EZLIBS_NAMESPACE
namespace ezlibs {
#endif

typedef void (CEZObject:: * EZ_TIMER_PROC)(EZTHREAD_PARAM wParam);
#define TIMER_NAME_LEN 32

class TimerValue
{
public:
    unsigned int lo;
    unsigned int hi;
    void operator+= (unsigned int a);
};
int compare(TimerValue& a, TimerValue& b);
EZTHREAD_BOOL operator> (TimerValue a, TimerValue b);

class CEZTimerManager;
class CEZTimer : public CEZObject
{
    friend class CEZTimerManager;
public:
    CEZTimer(const char * pName = "Nonamed");
    virtual ~CEZTimer();
    //dwDueTime:延时时间；dwPriod：间隔时间（震荡频率）
    void Start(CEZObject * pObj
               , EZ_TIMER_PROC pTimerFun
               , unsigned int dwDueTime // 持续时间
               , unsigned int dwPriod
               , EZTHREAD_PARAM param = 0
               , unsigned int timeout = 0);
    void Stop(EZTHREAD_BOOL bCallNow = EZTHREAD_BOOL_FALSE);
    void run();
    const char * GetName();
    void SetName(const char *);
    EZTHREAD_BOOL IsStarted();
    EZTHREAD_BOOL IsCalled();

private:
    TimerValue m_CallTime;
    unsigned int m_Priod;
    unsigned int m_timeout;
    CEZObject *m_pObj;
    EZ_TIMER_PROC m_pTimerFun;
    EZTHREAD_PARAM m_param;
    char  m_Name[TIMER_NAME_LEN+1];
    EZTHREAD_BOOL  m_Started;
    EZTHREAD_BOOL  m_Called;
    static CEZMutex m_CMutex;
    CEZTimer* m_pPrev;		//上一个定时器
    CEZTimer* m_pNext;		//下一个定时器
    CEZThreadlet m_threadlet;
};

class CEZTimerManager : public CEZThread
{
    friend class CEZTimer;
public:
    PATTERN_SINGLETON_DECLARE(CEZTimerManager);
    CEZTimerManager();
    ~CEZTimerManager();
    void Start();
    EZTHREAD_BOOL AddTimer(CEZTimer * pTimer);
    EZTHREAD_BOOL RemoveTimer(CEZTimer * pTimer);
    void DumpTimers();
    void DumpTimers(std::string &strInfo);
    void ThreadProc();

protected:
private:
    TimerValue m_CurTime;	//毫秒计数
    CEZTimer* m_pHead;
};

#define g_TimerManager (*CEZTimerManager::instance())
#ifdef EZLIBS_NAMESPACE
}// namespace ezlibs
#endif

#endif //EZTimer
