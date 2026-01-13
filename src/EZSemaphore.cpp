/**
 **	\file EZSemaphore.cpp
 **	\date  2007-04-13
**/

#include "EZSemaphore.h"


#ifdef EZLIBS_NAMESPACE
namespace ezlibs {
#endif


// ---------------------------------------------------------------
#ifdef _WIN32

CEZSemaphore::CEZSemaphore(ez_libs_value_t start_val)
{
	m_handle = ::CreateSemaphore((LPSECURITY_ATTRIBUTES)NULL, start_val, 1, (LPCTSTR)NULL);
}


CEZSemaphore::~CEZSemaphore()
{
	::CloseHandle(m_handle);
}


int CEZSemaphore::Post()
{
	return (::ReleaseSemaphore(m_handle, 1, (LPLONG)NULL) != 0) ? 0 : -1;
}


int CEZSemaphore::Wait()
{
	return (WaitForSingleObject(m_handle, INFINITE) == WAIT_OBJECT_0) ? 0 : -1;
}


int CEZSemaphore::TryWait()
{
	return -1; // %! not implemented
}


int CEZSemaphore::GetValue(int& i)
{
	return 0; // %! not implemented
}

// ---------------------------------------------------------------
#else

CEZSemaphore::CEZSemaphore(ez_libs_value_t start_val)
{
	sem_init(&m_sem, 0, start_val);
}


CEZSemaphore::~CEZSemaphore()
{
	sem_destroy(&m_sem);
}


int CEZSemaphore::Post()
{
	return sem_post(&m_sem);
}


int CEZSemaphore::Wait()
{
	return sem_wait(&m_sem);
}


int CEZSemaphore::TryWait()
{
	return sem_trywait(&m_sem);
}


int CEZSemaphore::GetValue(int& i)
{
	return sem_getvalue(&m_sem, &i);
}

#endif


#ifdef EZLIBS_NAMESPACE
} // namespace EZLIBS_NAMESPACE {
#endif

