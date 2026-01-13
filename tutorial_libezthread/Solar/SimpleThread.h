/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * SimpleThread.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: SimpleThread.h 5884 2013-12-26 02:44:48Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-12-26 02:44:48 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_StreamPusherTykd_H)
#define _StreamPusherTykd_H

#include "EZThread.h"

#include <string>

#define g_SimpleThread (*CSimpleThread::instance())

class CSimpleThread : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CSimpleThread);
	
	EZTHREAD_BOOL Start();
	EZTHREAD_BOOL Stop();
	void ThreadProc();
private:
	CSimpleThread();
	virtual ~CSimpleThread();

 };

#endif // !defined(_StreamPusherTykd_H)

