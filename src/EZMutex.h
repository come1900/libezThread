/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * EZMutex.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: EZMutex.h 5884 2004-10-30 09:17:02Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2004-10-30 09:17:02  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/** \file EZMutex.h
 **	\date  2004-10-30
**/
#ifndef _EZMutex_H
#define _EZMutex_H

#ifndef _WIN32
#include <pthread.h>
#else
//#include "socket_include.h"
#include <windows.h>
#endif
//#include "IEZMutex.h"

#ifdef EZLIBS_NAMESPACE
namespace ezlibs {
#endif

/// 互斥量类型
enum mutex_type
{
	/// 快速互斥量，如果临界区已经被一个线程获取，其他线程或者该线程再次获取时，
	/// 调用线程直接被挂起。
	MUTEX_FAST = 0,

	/// 递归互斥量，已经获取临界区的线程可以多次获取，互斥量对象内部应有获取次数
	/// 计数。如果要完全释放临界区，一定要保证获取次数和释放次数一致。
	MUTEX_RECURSIVE,
};

/** CEZMutex container class, used by CEZLock. 
	\ingroup threading */
class CEZMutex// : public ICMutex
{
public:
	CEZMutex(int nType = MUTEX_FAST);
	virtual ~CEZMutex();

	virtual void Lock() const;
	virtual void Unlock() const;

private:
#ifdef _WIN32
	HANDLE m_mutex;
#else
	mutable pthread_mutex_t m_mutex;
#endif
};


#ifdef EZLIBS_NAMESPACE
}
#endif
#endif // _EZMutex_H

