/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * tt.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: tt.cpp 5884 2012-05-16 09:17:46Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2011-10-13 22:00:11   Create
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
	
	BOOL Start();
	BOOL Stop();
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
CProducers::CProducers() :CEZThread("CProducers", TP_DEFAULT)
{
	trace("CProducers Enter--------\n");

}

CProducers::~CProducers()
{
	trace("CProducers Leave--------\n");

}

BOOL CProducers::Start()
{
	if (m_bLoop)
	{
		return TRUE;
	}
	int ret = CreateThread();
	
	return ret;
}

BOOL CProducers::Stop()
{
	if(m_bLoop)
	{
		m_bLoop = FALSE;
		DestroyThread();
	}
	return TRUE;
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
