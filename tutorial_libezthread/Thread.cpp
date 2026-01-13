/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Thread.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Thread.cpp 5884 2012-05-16 09:17:46Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "CommonInclude.h"
#include "EZThread.h"
#include "EZTimer.h"
#include <string>
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


int main(int argc, char* argv[])
{
	int iRet = 0;

	g_TimerManager.Start();
	
	g_ThreadManager.RegisterMainThread(ThreadGetID());	// ×¢²áÖ÷Ïß³Ì

	printf("%s end.\n", argv[0]);
	
	g_Producers.Start();
	
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
		//__trip;
		
		g_ThreadManager.DumpThreads();
		
		sleep(1);
	}
}
