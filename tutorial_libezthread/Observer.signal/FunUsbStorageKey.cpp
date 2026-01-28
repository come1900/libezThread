/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FunUsbStorageKey.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: FunUsbStorageKey.cpp 5884 2012-05-16 09:14:51Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "DevUsbStorageKey.h"
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
    printf(" %s CALLBACK - OnDownload: %d, %p, %d, %ld\n", m_strName.c_str(), iCMD, (void*)pPacket, iCh, time);
}

void CFunUsbStorageKey::Start()
{
    g_DevUsbStorageKey.Start(this, (CDevUsbStorageKey::DevUsbStorageKeySignalProc_t)&CFunUsbStorageKey::OnDownload);
}

void CFunUsbStorageKey::Stop()
{
    g_DevUsbStorageKey.Stop(this, (CDevUsbStorageKey::DevUsbStorageKeySignalProc_t)&CFunUsbStorageKey::OnDownload);
}
