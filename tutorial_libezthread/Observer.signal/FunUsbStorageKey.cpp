// FunUsbStorageKey.cpp: implementation of the CFunUsbStorageKey class.
//
//////////////////////////////////////////////////////////////////////

#include "../../Device/DevUsbStorageKey.h"

#include "FunUsbStorageKey.h"

#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
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

CFunUsbStorageKey::    CFunUsbStorageKey(std::string name)
{
    m_strName = name;
}

CFunUsbStorageKey::~CFunUsbStorageKey()
{
}

void CFunUsbStorageKey::OnDownload(int iCMD, int *pPacket, int iCh, time_t time)
{
    __fline;
    printf(" %s CALLBACK - OnDownload: %d, 0x%0x, %d, %ld\n", m_strName.c_str(), iCMD, (unsigned int)pPacket, iCh, time);
}

void CFunUsbStorageKey::Start()
{
    g_DevUsbStorageKey.Start(this, (CDevUsbStorageKey::DevUsbStorageKeySignalProc_t)&CFunUsbStorageKey::OnDownload);
}

void CFunUsbStorageKey::Stop()
{
    g_DevUsbStorageKey.Stop(this, (CDevUsbStorageKey::DevUsbStorageKeySignalProc_t)&CFunUsbStorageKey::OnDownload);
}
