/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DevUsbStorageKey.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: DevUsbStorageKey.h 5884 2012-05-16 09:14:47Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _EZDevUsbStorageKey_H_
#define _EZDevUsbStorageKey_H_

#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

#define g_DevUsbStorageKey (*CDevUsbStorageKey::instance())
class CDevUsbStorageKey : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CDevUsbStorageKey);
    typedef TSignal4<int, int *, int, time_t> DevUsbStorageKeySignal_t;
    // cmd, packet, ch, user
    typedef DevUsbStorageKeySignal_t::SigProc DevUsbStorageKeySignalProc_t;

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();

    EZTHREAD_BOOL Start(CEZObject * pObj, DevUsbStorageKeySignalProc_t pProc);
    EZTHREAD_BOOL Stop(CEZObject * pObj, DevUsbStorageKeySignalProc_t pProc);

    void ThreadProc();

private:
    CDevUsbStorageKey();
    virtual ~CDevUsbStorageKey();

    //DevUsbStorageKeySignalProc_t m_SigBuffer;
    DevUsbStorageKeySignal_t m_SigBuffer;

    CEZMutex m_MutexSigBuffer;

    //计数
    int				m_iUser;
};

#endif// _EZDevUsbStorageKey_H_
