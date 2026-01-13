/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * EZLock.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: EZLock.h 5884 2005-08-22 09:14:22Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2005-08-22 09:14:22  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _EZLock_H
#define _EZLock_H

#ifdef EZLIBS_NAMESPACE
namespace ezlibs {
#endif

class CEZMutex;

/** ICMutex encapsulation class. 
	\ingroup threading */
class CEZLock
{
public:
	CEZLock(const CEZMutex&);
	~CEZLock();

private:
	const CEZMutex& m_mutex;
};

#ifdef EZLIBS_NAMESPACE
}
#endif
#endif // _EZLock_H

