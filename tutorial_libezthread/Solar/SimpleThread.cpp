/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * SimpleThread.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: SimpleThread.cpp 5884 2013-12-26 02:44:42Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-12-26 02:44:42 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <unistd.h> // sleep()

#include "SimpleThread.h"

#ifndef _DEBUG_THIS
    //#define _DEBUG_THIS
#endif

#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
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

#define g_SimpleThread (*CSimpleThread::instance())

PATTERN_SINGLETON_IMPLEMENT(CSimpleThread);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CSimpleThread::CSimpleThread() :CEZThread("CSimpleThread", THREAD_PRIORITY_DEFAULT)
{
	printf("CSimpleThread Enter--------\n");
}

CSimpleThread::~CSimpleThread()
{
	printf("CSimpleThread Leave--------\n");
}

EZTHREAD_BOOL CSimpleThread::Start()
{
	if (m_bLoop)
	{
		return EZTHREAD_BOOL_TRUE;
	}
	int ret = CreateThread();
	
	return ret;
}

EZTHREAD_BOOL CSimpleThread::Stop()
{
	if(m_bLoop)
	{
		m_bLoop = EZTHREAD_BOOL_FALSE;
		DestroyThread();
	}
	return EZTHREAD_BOOL_TRUE;
}
void CSimpleThread::ThreadProc()
{
	while (m_bLoop)
	{
		__fline;
		printf("CSimpleThread::ThreadProc()\n");
		//g_ThreadManager.DumpThreads();
		
		sleep(1);
	}
}
