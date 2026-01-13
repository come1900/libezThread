/** \file EZMutex.cpp
 **	\date  2004-10-30
**/
#include <assert.h>

#include "typedef_ezthread.h"

#include "EZMutex.h"

#ifdef EZLIBS_NAMESPACE
namespace ezlibs
{
#endif

#if defined(MACOSX) || defined(IOS) || defined(LINUX_MIPS)
#define HAVE_PTHREAD_MUTEX_RECURSIVE 1
#else
#define HAVE_PTHREAD_MUTEX_RECURSIVE_NP 1
#endif
/* 
   One of PTHREAD_MUTEX_RECURSIVE_NP and PTHREAD_MUTEX_RECURSIVE seem to be 
   present in different versions.  For example, Mac OS X 10.4 had 
   PTHREAD_MUTEX_RECURSIVE_NP but Mac OS X 10.5 does not; instead it has
   PTHREAD_MUTEX_RECURSIVE 
*/
#if defined(HAVE_PTHREAD_MUTEX_RECURSIVE_NP)
#define PTHREAD_MUTEX_RECURSIVE_VALUE PTHREAD_MUTEX_RECURSIVE_NP
#elif defined(HAVE_PTHREAD_MUTEX_RECURSIVE)
#define PTHREAD_MUTEX_RECURSIVE_VALUE PTHREAD_MUTEX_RECURSIVE
#else
#error 'Unable to determine pthrad mutex recursive value'
#endif /* pthread mutex recursive value */


CEZMutex::CEZMutex(int nType)
{
#ifdef _WIN32
    m_mutex = ::CreateMutex(NULL, EZTHREAD_BOOL_FALSE, NULL);
#else
    int ret ;
    if (nType==MUTEX_RECURSIVE)
    {
		pthread_mutexattr_t attr;
        ret = pthread_mutexattr_init(&attr);
        assert(ret==0);
        //ret = pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE_NP);
        // fix for macos 2014-3-18 11:25:43
        ret = pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE_VALUE);
        assert(ret==0);

        ret = pthread_mutex_init(&m_mutex, &attr)
              ;
        assert(ret==0);
    }
    else
    {
        ret = pthread_mutex_init(&m_mutex, NULL);
        assert(ret==0);
    }

#endif
}


CEZMutex::~CEZMutex()
{
#ifdef _WIN32
    ::CloseHandle(m_mutex);
#else

    pthread_mutex_destroy(&m_mutex);
#endif
}


void CEZMutex::Lock() const
{
#ifdef _WIN32
    DWORD d = WaitForSingleObject(m_mutex, INFINITE);
    /// \todo check 'd' for result
#else

    pthread_mutex_lock(&m_mutex);
#endif
}


void CEZMutex::Unlock() const
{
#ifdef _WIN32
    ::ReleaseMutex(m_mutex);
#else

    pthread_mutex_unlock(&m_mutex);
#endif
}


#ifdef EZLIBS_NAMESPACE
}
#endif

