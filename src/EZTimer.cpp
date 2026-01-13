/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * EZTimer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Timer.cpp 5884 2012-05-16 09:09:34Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2011-10-13 22:00:11   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "EZTimer.h"
#include <string.h>
#ifdef _DEBUG
	#define DEB(x) 
	#define DBG(x) 
#else
	#define DEB(x) 
	#define DBG(x)
#endif

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) (void)&x;
#ifdef EZLIBS_NAMESPACE
namespace ezlibs {
#endif

CEZMutex CEZTimer::m_CMutex(MUTEX_RECURSIVE);

void TimerValue::operator+= (unsigned int a)
{
	lo += a;
	if(lo < a) //进位
	{
		hi++;
	}
}

int compare(TimerValue& a, TimerValue& b)
{
	if(a.hi < b.hi)
	{
		return -1;
	}
	else if(a.hi > b.hi)
	{
		return 1;
	}
	else
	{
		if(a.lo < b.lo)
		{
			return -1;
		}
		else if(a.lo > b.lo)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

EZTHREAD_BOOL operator> (TimerValue a, TimerValue b)
{
	return (compare(a, b) > 0);
}

EZTHREAD_BOOL operator< (TimerValue a, TimerValue b)
{
	return (compare(a, b) < 0);
}

CEZTimer::CEZTimer(const char * pName)
{
	SetName(pName);
	m_Started = EZTHREAD_BOOL_FALSE;
	m_Called = EZTHREAD_BOOL_FALSE;
}

CEZTimer::~CEZTimer()
{
	CEZLock __lock(m_CMutex);

	if(m_Started)
	{
		g_TimerManager.RemoveTimer(this);
	}
}

void CEZTimer::Start(CEZObject * pObj, EZ_TIMER_PROC pTimerFun, unsigned int dwDueTime, unsigned int dwPriod, EZTHREAD_PARAM param /* = 0 */, unsigned int timeout /* = 0 */)
{
	CEZLock __lock(m_CMutex);
	m_pObj = pObj;
	m_pTimerFun = pTimerFun;
	m_CallTime = g_TimerManager.m_CurTime;
	m_CallTime += dwDueTime;
	m_Priod = dwPriod;
	m_timeout = timeout;
	m_param = param;
	m_Called = EZTHREAD_BOOL_FALSE;
	if(m_Started)
	{
		g_TimerManager.RemoveTimer(this);
	}
	g_TimerManager.AddTimer(this);
	m_Started = EZTHREAD_BOOL_TRUE;
}

void CEZTimer::Stop(EZTHREAD_BOOL bCallNow /* = EZTHREAD_BOOL_FALSE */)
{
	CEZLock __lock(m_CMutex);
	if(!m_Started)
	{
		return;
	}

	//结束时立即调用一次回调函数，在需要提前结束非周期定时器时有用
	if(bCallNow && m_Priod == 0)
	{
		(m_pObj->*m_pTimerFun)(m_param); 
	}
	g_TimerManager.RemoveTimer(this);
	m_Started = EZTHREAD_BOOL_FALSE;
}

void CEZTimer::run()
{
	g_TimerManager.RemoveTimer(this);
	if(m_Priod)
	{
		m_CallTime += m_Priod;
		g_TimerManager.AddTimer(this);
	}
	else
	{
		m_Started = EZTHREAD_BOOL_FALSE;
		m_Called = EZTHREAD_BOOL_TRUE;	
	}

	m_threadlet.run(m_Name, m_pObj, m_pTimerFun, m_param, m_timeout);
}

const char * CEZTimer::GetName()
{
	return m_Name;
}

void CEZTimer::SetName(const char * pszName)
{
	//int len = MIN(strlen(pszName), 31);
	//memcpy(m_Name, pszName, len);
	
	strncpy(m_Name, pszName, TIMER_NAME_LEN);
	//m_Name[len]='\0';
}

EZTHREAD_BOOL CEZTimer::IsStarted()
{
	return m_Started;
}

EZTHREAD_BOOL CEZTimer::IsCalled()
{
	return m_Called;
}
////////////////////////////////////////////////////////////////////////////////////
// CEZTimerManager
////////////////////////////////////////////////////////////////////////////////////

PATTERN_SINGLETON_IMPLEMENT(CEZTimerManager);

CEZTimerManager::CEZTimerManager():CEZThread("TimerManager", THREAD_PRIORITY_TIMER)
{
	m_pHead = NULL;
	
	m_CurTime.lo = SystemGetMSCount();
	m_CurTime.hi = 0;
DBG(
	ez_trace("CEZTimerManager::CTimerManager()>>>>>>>>>\n");
);
}

CEZTimerManager::~CEZTimerManager()
{
	DestroyThread();
}

void CEZTimerManager::Start()
{
	CreateThread();
}

EZTHREAD_BOOL CEZTimerManager::AddTimer(CEZTimer * pTimer)
{
	CEZTimer *pp,*pn;

	pp = NULL;
	pn = m_pHead;

	while(pn && (pTimer->m_CallTime > pn->m_CallTime))
	{
		pp = pn;
		pn = pn->m_pNext;
	}

	if(pp)
	{
		pp->m_pNext = pTimer;
	}
	else
	{
		m_pHead = pTimer;
	}
	pTimer->m_pPrev = pp;

	if(pn)
	{
		pn->m_pPrev = pTimer;
	}
	pTimer->m_pNext = pn;
	
	return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CEZTimerManager::RemoveTimer(CEZTimer * pTimer)
{
	if(pTimer->m_pPrev == NULL)
	{
		m_pHead = pTimer->m_pNext;
		if(m_pHead)
		{
			m_pHead->m_pPrev = NULL;
		}

		return EZTHREAD_BOOL_TRUE;
	}

	pTimer->m_pPrev->m_pNext = pTimer->m_pNext;
	if(pTimer->m_pNext != NULL)
		pTimer->m_pNext->m_pPrev = pTimer->m_pPrev;

	return EZTHREAD_BOOL_TRUE;
}

void CEZTimerManager::DumpTimers()
{
	CEZTimer * p = m_pHead;
	int i=0; // num

	//CEZGuard guard(CEZTimer::m_CMutex);
		CEZLock __lock(CEZTimer::m_CMutex);
	ez_trace("Timers: ( %u Milli-Seconds Elapsed )\n", m_CurTime.lo);
	ez_trace("%33s%10s%10s%10s\n", "Name", "Priod", "TimeOut", "State");
	ez_trace("----------------------------------------------------------------\n");
	while (p)
	{
		i++;
		ez_trace("%33s%10u%10u%10s\n", p->GetName(), p->m_Priod, p->m_timeout, p->m_threadlet.isRunning() ? "Running" : "Idel");
		p = p->m_pNext;
	}
	ez_trace("----------------------------------------------------------------\n");
	ez_trace("\tTotal:%d\n", i);
}
void CEZTimerManager::DumpTimers(std::string &strThreadInfo)
{
	CEZTimer * p = m_pHead;
	int i=0; // num
	char tmpBuf[1024];
	strThreadInfo = "";

	//CEZGuard guard(CEZTimer::m_CMutex);
		CEZLock __lock(CEZTimer::m_CMutex);
	sprintf(tmpBuf, "Timers: ( %u Milli-Seconds Elapsed )\n", m_CurTime.lo);
	strThreadInfo += tmpBuf;
	sprintf(tmpBuf, "%33s%10s%10s%10s\n", "Name", "Priod", "TimeOut", "State");
	strThreadInfo += tmpBuf;
	sprintf(tmpBuf, "----------------------------------------------------------------\n");
	strThreadInfo += tmpBuf;
	while (p)
	{
		i++;
		sprintf(tmpBuf, "%33s%10u%10u%10s\n", p->GetName(), p->m_Priod, p->m_timeout, p->m_threadlet.isRunning() ? "Running" : "Idel");
		strThreadInfo += tmpBuf;
		p = p->m_pNext;
	}
	sprintf(tmpBuf, "----------------------------------------------------------------\n");
	strThreadInfo += tmpBuf;
	sprintf(tmpBuf, "\tTotal:%d\n", i);
	strThreadInfo += tmpBuf;
}

void CEZTimerManager::ThreadProc()
{
	do 
	{
		SystemSleep(1);

		//CEZGuard guard(CEZTimer::m_CMutex);
CEZLock __lock(CEZTimer::m_CMutex);
		unsigned int OldTime = m_CurTime.lo;
		m_CurTime.lo = SystemGetMSCount();

		// 计时没有改变，可能是因为计时器精度不高
		if(m_CurTime.lo == OldTime)
		{
			continue;
		}

		if(m_CurTime.lo < OldTime)
		{
			// ez_trace("CEZTimerManager::Check() MSCount Overflowed, %u < %u .\n", m_CurTime.lo, OldTime);

			if(m_CurTime.lo < 10000) //判定为溢出
			{
				m_CurTime.hi++;
			}
		}

		while(m_pHead && m_pHead->m_CallTime < m_CurTime)
		{
			m_pHead->run();
		}
	} while(m_bLoop);
}
#ifdef EZLIBS_NAMESPACE
}// namespace ezlibs
#endif
