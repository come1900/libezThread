/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * EZMsgQue.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: EZMsgQue.h 5884 2012-05-16 09:14:47Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _EZEZMsgQue_H_
#define _EZEZMsgQue_H_

#if defined(WIN32)
	#pragma warning (disable : 4786)
#endif
#include <list>
#include "typedef_ezthread.h"
#include "api_ezthread.h"
//#include "EZEZMutex.h"
#include "EZMutex.h"

#include "EZSemaphore.h"

#include "EZPoolAllocator.h"

#ifdef EZLIBS_NAMESPACE
namespace ezlibs {
#endif

#define MSG_PRIORITY	10
#define MSG_On_UserData 1


typedef struct tagMSG_NODE
{
    unsigned int		msg;
    EZTHREAD_PARAM		wpa;
    EZTHREAD_PARAM		lpa;
    void                *pData;
    unsigned int		time;
}
MSG_NODE;

typedef std::list<MSG_NODE, pool_allocator<MSG_NODE> > MSGQUEUE;

class CEZMsgQue
{
public:
    CEZMsgQue(int size = 1024);
    virtual ~CEZMsgQue();

    EZTHREAD_BOOL SendMessage (unsigned int msg, EZTHREAD_PARAM wpa = 0, EZTHREAD_PARAM lpa = 0, unsigned int priority = 0);
    EZTHREAD_BOOL SendMessage (void *p_user_data);
    EZTHREAD_BOOL RecvMessage (MSG_NODE *pMsg, EZTHREAD_BOOL wait = EZTHREAD_BOOL_TRUE);
    void QuitMessage ();
    void ClearMessage();
    int GetMessageCount();
    int GetMessageSize();
    void SetMessageSize(int size);

protected:
private:
    MSGQUEUE m_Queue;
    EZTHREAD_BOOL m_bMsgFlg;
    CEZMutex m_MutexMsg;
    CEZSemaphore m_CEZSemaphore;
    int m_nMsg;
    int m_nMaxMsg;
};
#ifdef EZLIBS_NAMESPACE
}// namespace ezlibs
#endif

#endif// _EZEZMsgQue_H_
