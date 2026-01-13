/** \file EZLock.cpp
 **	\date  2005-08-22
**/

#include "EZMutex.h"
#include "EZLock.h"

#ifdef EZLIBS_NAMESPACE
namespace ezlibs {
#endif


CEZLock::CEZLock(const CEZMutex& m) : m_mutex(m)
{
	m_mutex.Lock();
}


CEZLock::~CEZLock()
{
	m_mutex.Unlock();
}

#ifdef EZLIBS_NAMESPACE
}// namespace ezlibs
#endif

