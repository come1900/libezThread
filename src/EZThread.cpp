/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * EZThread.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: EZThread.cpp 5884 2012-05-16 09:09:44Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include "EZTimer.h"
#include "EZThread.h"
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

void ThreadBody(void *pdat)
{
	CEZThread *pThread = (CEZThread *)pdat;

/************************************************************************
	线程通用回调函数：
	1、获得线程ID；
	2、执行特定对象的线程回调函数；
	3、当特定对象的线程回调函数执行完毕，把该线程从线程管理队列中删除；
	4、释放同步信号量；
	5、把线程句柄置为空；
************************************************************************/
	pThread->m_ID = ThreadGetID();
	g_ThreadManager.AddThread(pThread);
	//ez_trace("ThreadBody Enter name = %s, id = %d, handle = 0x%x, stack = 0x%0x\n", pThread->m_pStrThreadName->c_str(), pThread->m_ID, (int)pThread->m_hThread, &pThread);
	pThread->ThreadProc();
	//ez_trace("ThreadBody Leave name = %s, id = %d, handle = 0x%x\n", pThread->m_pStrThreadName->c_str(), pThread->m_ID, (int)pThread->m_hThread);
	pThread->ClearMessage();
	if (pThread->m_bWaitThreadExit)
	{
		pThread->m_desCEZSemaphore.Post();
	}
	pThread->m_cCEZSemaphore.Post();
	g_ThreadManager.RemoveThread(pThread);

	ThreadExit();
}

CEZThread::CEZThread(const std::string &strName, int nPriority, int nMsgQueSize /* = 0 */, unsigned int dwStackSize /* = 0 */) : m_cCEZSemaphore(1), m_desCEZSemaphore(0)
{
/************************************************************************
	线程对象初始化：
	1、信号量初始化；
	2、优先级变量；
	3、堆栈大小变量；
	4、线程名；
	5、线程句柄；
	6、消息队列对象指针；
************************************************************************/
	m_nPriority = nPriority;
	m_dwStackSize = dwStackSize;

	m_pStrThreadName = new (std::nothrow) std::string();
	assert(m_pStrThreadName);
	if(strName.length() > 0)
	{
		*m_pStrThreadName = strName;
	}
	else
	{
		*m_pStrThreadName = defDefaultThreadName;
	}
	m_hThread = NULL;
	m_ID = -1;
	m_bLoop = EZTHREAD_BOOL_FALSE;
	if(nMsgQueSize)
	{
		m_pMsgQue = new (std::nothrow) CEZMsgQue(nMsgQueSize);
	}
	else
	{
		m_pMsgQue = NULL;
	}
	m_expectedTime = 0;
	m_bWaitThreadExit = EZTHREAD_BOOL_FALSE;
}

CEZThread::~CEZThread()
{
	if (m_hThread)
	{
		DestroyThread();
	}
	
	if(m_pMsgQue)
	{
		delete m_pMsgQue;
	}

	if (m_pStrThreadName)
	{
		delete m_pStrThreadName;
	}
}

int CEZThread::SetThreadName(const std::string &strName)
{
	if(strName.length() > 0)
	{
		*m_pStrThreadName = strName;
	}
	else
	{
		*m_pStrThreadName = defDefaultThreadName;
	}

	return 0;
}

EZTHREAD_BOOL CEZThread::CreateThread()
{
/************************************************************************
	创建线程：
	1、设置信号量和线程回调函数中的循环标志；
	2、调用创建线程API；
	3、如果线程创建成功，把该线程对象加入到线程管理队列，并返回TURE，
	   否则返回FALSE；
************************************************************************/

	m_cCEZSemaphore.Wait();

	m_bLoop = EZTHREAD_BOOL_TRUE;
	
	return ThreadCreate(&m_hThread, ThreadBody, this, m_nPriority, m_dwStackSize, m_pStrThreadName->c_str()) == 0 ? EZTHREAD_BOOL_TRUE : EZTHREAD_BOOL_FALSE;
}

EZTHREAD_BOOL CEZThread::DestroyThread(EZTHREAD_BOOL bWaited)
{
/************************************************************************
	结束线程：
	1、直接把用于控制主循环的变量置为FALSE，让它退出主循环；
************************************************************************/
	if (!IsStoped())
	{
		m_bLoop = EZTHREAD_BOOL_FALSE;
		SendMessage(XM_QUIT);//如果发送出错，说明队列已满，此时m_bLoop可以保证线程循环退出

		// 不是自己关自己的时候才采用阻塞式
		if (m_ID != ThreadGetID())
		{
			m_bWaitThreadExit = bWaited;
			if (m_bWaitThreadExit)
			{
				m_desCEZSemaphore.Wait();
			}
		}
	}
	return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CEZThread::IsStoped()
{
/************************************************************************
	判断线程回调函数是否已经停止：
	1、如果已经停止，则返回TRUE，否则返回FALSE；
************************************************************************/
	return !m_bLoop;
	
}

EZTHREAD_BOOL CEZThread::IsRunning()
{
	return m_bLoop;
}

int CEZThread::GetThreadID()
{
	return m_ID;
}

int CEZThread::ShareSocket(int nSocket)
{
	return ThreadShareSocket(m_hThread, nSocket);
}

EZTHREAD_BOOL CEZThread::SendMessage(unsigned int msg, EZTHREAD_PARAM wpa /* = 0 */, EZTHREAD_PARAM lpa /* = 0 */, unsigned int priority /* = 0 */)
{
/************************************************************************
	发送消息：
	1、判断本线程是否有消息队列；
	2、如果有，则调用消息队列对象的SendMessage函数发送消息，
	   否则直接返回FALSE；
************************************************************************/
	if(m_pMsgQue)
	{
		return m_pMsgQue->SendMessage(msg, wpa, lpa, priority);
	}
	return EZTHREAD_BOOL_FALSE;
}

EZTHREAD_BOOL CEZThread::SendMessage (void *p_user_data)
{
	if(m_pMsgQue)
	{
		return m_pMsgQue->SendMessage(p_user_data);
	}
	return EZTHREAD_BOOL_FALSE;
}


EZTHREAD_BOOL CEZThread::RecvMessage(MSG_NODE *pMsg, EZTHREAD_BOOL wait /* = EZTHREAD_BOOL_TRUE */)
{
/************************************************************************
	接收消息：
	1、判断本线程是否有消息队列；
	2、如果有，则调用消息队列对象的RecvMessage函数接收消息，
	   否则直接返回FALSE；
************************************************************************/
	if(m_pMsgQue)
	{
		return m_pMsgQue->RecvMessage(pMsg, wait);
	}
	return EZTHREAD_BOOL_FALSE;
}

void CEZThread::QuitMessage()
{
/************************************************************************
	停止消息：
	1、判断本线程是否有消息队列；
	2、如果有，则调用消息队列对象的QuitMessage函数停止消息，
	   否则直接返回FALSE；
************************************************************************/
	if(m_pMsgQue)
	{
		m_pMsgQue->QuitMessage();
	}
}

void CEZThread::ClearMessage()
{
	if(m_pMsgQue)
	{
		m_pMsgQue->ClearMessage();
	}
}

unsigned int CEZThread::GetMessageCount()
{
	if(m_pMsgQue)
	{
		return m_pMsgQue->GetMessageCount();
	}
	return 0;
}

void CEZThread::SetTimeout(unsigned int milliSeconds)
{
	if(milliSeconds == 0) // 清空预期时间
	{
		m_expectedTime = 0;
	}
	else
	{
		m_expectedTime = SystemGetMSCount() + milliSeconds;

		if(m_expectedTime < milliSeconds) // 计数溢出， 暂时不设置预期时时间
		{
			m_expectedTime = 0;
		}
	}
}

EZTHREAD_BOOL CEZThread::IsTimeout()
{
	return (m_expectedTime != 0 && m_expectedTime < SystemGetMSCount());
}

////////////////////////////////////////////////////////////////////////////////////
// CEZPooledThread
////////////////////////////////////////////////////////////////////////////////////
CEZPooledThread::CEZPooledThread() : CEZThread(std::string("Pooled"), THREAD_PRIORITY_DEFAULT)
{
}

CEZPooledThread::~CEZPooledThread()
{
}

void CEZPooledThread::ThreadProc()
{
	while(m_bLoop)
	{
		m_semaphore.Wait();
		(m_obj->*m_asynFunc)(m_param); 
		SetThreadName("Pooled");
		SetTimeout(0);
		CEZThreadlet::m_mutex.Lock();// Enter();
		if(m_caller)
		{
			m_caller->m_thread = NULL;
			m_caller = NULL;
		}
		CEZThreadlet::m_mutex.Unlock();
		g_ThreadManager.ReleasePooledThread(this);
	}
}

////////////////////////////////////////////////////////////////////////////////////
// CEZThreadlet
////////////////////////////////////////////////////////////////////////////////////
CEZMutex CEZThreadlet::m_mutex;

CEZThreadlet::CEZThreadlet()
{
	m_thread = NULL;
}

CEZThreadlet::~CEZThreadlet()
{
	//CEZLock __lock(m_mutex);
	CEZLock __lock(m_mutex);

	if(m_thread)
	{
		m_thread->m_caller = NULL;
	}
}

EZTHREAD_BOOL CEZThreadlet::run(const char * name, CEZObject * obj, ASYNPROC asynProc, EZTHREAD_PARAM param, unsigned int timeout)
{
	CEZLock __lock(m_mutex);

	if(m_thread) //正在运行
	{
		return EZTHREAD_BOOL_FALSE;
	}

	m_thread = g_ThreadManager.GetPooledThread();
	m_thread->SetThreadName(name);
	m_thread->SetTimeout(timeout);
	m_thread->m_obj = obj;
	m_thread->m_asynFunc = asynProc;
	m_thread->m_param = param;
	m_thread->m_caller = this;
	m_thread->m_semaphore.Post();
	return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CEZThreadlet::isRunning()
{
	return (m_thread != NULL);
}

////////////////////////////////////////////////////////////////////////////////////
// CEZThreadManager
////////////////////////////////////////////////////////////////////////////////////

PATTERN_SINGLETON_IMPLEMENT(CEZThreadManager);

CEZThreadManager::CEZThreadManager() : m_CMutex(MUTEX_RECURSIVE)
{
DBG(
	ez_trace("CEZThreadManager::CThreadManager()>>>>>>>>>\n");
);
	m_pHead = NULL;
	m_headPooled = NULL;
}

CEZThreadManager::~CEZThreadManager()
{
}

void CEZThreadManager::RegisterMainThread(int id)
{
	m_mainThreadID = id;
}

EZTHREAD_BOOL CEZThreadManager::AddThread(CEZThread *pThread)
{
	CEZLock __lock(m_CMutex);

	pThread->m_pPrev = NULL;
	pThread->m_pNext = m_pHead;

	if(m_pHead)
	{
		m_pHead->m_pPrev = pThread;
	}

	m_pHead = pThread;

	return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CEZThreadManager::RemoveThread(CEZThread *pThread)
{
	CEZLock __lock(m_CMutex);

	if(pThread->m_pPrev == NULL)
	{
		m_pHead = pThread->m_pNext;
		if(m_pHead)
		{
			m_pHead->m_pPrev = NULL;
		}

		return EZTHREAD_BOOL_TRUE;
	}

	pThread->m_pPrev->m_pNext = pThread->m_pNext;
	if(pThread->m_pNext != NULL)
		pThread->m_pNext->m_pPrev = pThread->m_pPrev;

	return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CEZThreadManager::HasThread(CEZThread *pThread)
{
	CEZLock __lock(m_CMutex);
	CEZThread * p = m_pHead;

	while (p)
	{
		if (p == pThread)
		{
			return EZTHREAD_BOOL_TRUE;
		}
		p = p->m_pNext;
	}

	return EZTHREAD_BOOL_FALSE;
}

CEZPooledThread * CEZThreadManager::GetPooledThread()
{
	CEZLock __lock(m_CMutex);
	CEZPooledThread * p = m_headPooled;

	if(p) 
	{
		m_headPooled = p->m_nextPooled;
	}
	else 
	{
		p = new CEZPooledThread();
		p->CreateThread();
	}

	return p;
}

void CEZThreadManager::ReleasePooledThread(CEZPooledThread* pThread)
{
	CEZLock __lock(m_CMutex);
	assert(pThread);

	pThread->m_nextPooled = m_headPooled;
	m_headPooled = pThread;
}
#include <time.h>
void CEZThreadManager::DumpThreads()
{
	int i=0; // thread num
	CEZLock __lock(m_CMutex);
	CEZThread * p = m_pHead;

	//ez_trace("Threads:      Time(%ld)\n", time(NULL));
	ez_trace("Threads:\n");
	//ez_trace("               Name            PID  Prior State\n");
	ez_trace("%33s%10s%10s%10s\n", "Name", "PID", "Prior", "State");
	ez_trace("----------------------------------------------------------------\n");
	ez_trace("%33s%10d%10d%10s\n", "Main", m_mainThreadID, THREAD_PRIORITY_DEFAULT, "Normal");
	i++;
	while (p)
	{
		i++;
		ez_trace("%33s%10d%10d%10s\n", p->m_pStrThreadName->c_str(), p->m_ID, p->m_nPriority, p->IsTimeout()?"Timeout":"Normal");
		p = p->m_pNext;
	}
	ez_trace("----------------------------------------------------------------\n");
	ez_trace("\tTotal:%d\n", i);
}

void CEZThreadManager::DumpThreads(std::string &strThreadInfo, int voidPriority)
{
	int i=0; // thread num
	char tmpBuf[1024];
	CEZLock __lock(m_CMutex);
	CEZThread * p = m_pHead;
	strThreadInfo = "Threads:\n";
	//ez_trace("               Name            PID  Prior State\n");
	sprintf(tmpBuf, "%33s%10s%10s%10s\n", "Name", "PID", "Prior", "State");
	strThreadInfo += tmpBuf;
	sprintf(tmpBuf, "----------------------------------------------------------------\n");
	strThreadInfo += tmpBuf;
	sprintf(tmpBuf, "%33s%10d%10d%10s\n", "Main", m_mainThreadID, THREAD_PRIORITY_DEFAULT, "Normal");
	strThreadInfo += tmpBuf;
	i++;
        int jj=0;
	while (p)
	{
		i++;
                // do not show
                if (voidPriority>0 && p->m_nPriority==voidPriority)
                {
                    jj++;
                }
                else
                {
   		    sprintf(tmpBuf, "%33s%10d%10d%10s\n", p->m_pStrThreadName->c_str(), p->m_ID, p->m_nPriority, p->IsTimeout()?"Timeout":"Normal");
		    strThreadInfo += tmpBuf;
                }

		p = p->m_pNext;
	}
	sprintf(tmpBuf, "----------------------------------------------------------------\n");
	strThreadInfo += tmpBuf;
	sprintf(tmpBuf, "%21s:%d\n", "Total", i);
	strThreadInfo += tmpBuf;

        if (jj>0)
        {
            sprintf(tmpBuf, "%16s[%03d]:%d\n", "voidPriority", voidPriority, jj);
	    strThreadInfo += tmpBuf;
        }
}
#ifdef EZLIBS_NAMESPACE
}// namespace ezlibs
#endif
