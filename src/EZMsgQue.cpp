/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * EZMsgQue.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: MsgQue.cpp 5884 2012-05-16 09:14:51Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2011-10-13 22:00:11   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "EZMsgQue.h"
#include "EZLock.h"
#ifdef EZLIBS_NAMESPACE
namespace ezlibs {
#endif

CEZMsgQue::CEZMsgQue(int size /* = 1024 */)
{
    m_nMaxMsg = size;
    m_nMsg = 0;
    m_bMsgFlg = EZTHREAD_BOOL_TRUE;
}

CEZMsgQue::~CEZMsgQue()
{
}

EZTHREAD_BOOL CEZMsgQue::SendMessage(unsigned int msg, EZTHREAD_PARAM wpa /* = 0 */, EZTHREAD_PARAM lpa /* = 0 */, unsigned int priority /* = 0 */)
{
    MSG_NODE l_MSG;
    MSGQUEUE::iterator pi;

    m_MutexMsg.Lock();
    if(m_nMsg >= m_nMaxMsg)
    {
        m_MutexMsg.Unlock();
        return EZTHREAD_BOOL_FALSE;
    }
    if(priority >= MSG_PRIORITY)
    {
        m_MutexMsg.Unlock();
        return EZTHREAD_BOOL_FALSE;
    }
    if (!m_bMsgFlg)
    {
        m_MutexMsg.Unlock();
        return EZTHREAD_BOOL_FALSE;
    }

    //鼠标消息特殊处理, 合并鼠标移动消息
#ifdef SUPPORT_MOUSE_MSG
    if(msg == XM_MOUSEMOVE && !m_Queue.empty())
    {
        for (pi = m_Queue.begin(); pi != m_Queue.end(); pi++)
        {
            if((*pi).msg >= XM_LBUTTONDOWN && (*pi).msg <= XM_MBUTTONDBLCLK)
            {
                break;
            }
            if((*pi).msg == XM_MOUSEMOVE)
            {
                (*pi).wpa = wpa;
                (*pi).lpa = lpa;
                (*pi).time = SystemGetMSCount();
                m_MutexMsg.Unlock();
                return EZTHREAD_BOOL_TRUE;
            }
        }
    }
#endif
    /************************************************************************
    	发送消息:
    	1、按照优先级把该消息插入队列m_Queue；
    //	2、在该队列中查找该消息节点，直到找不到该节点才退出循环并从该函数返回；
    ************************************************************************/
    l_MSG.msg = msg;
    l_MSG.wpa = wpa;
    l_MSG.lpa = lpa;
    l_MSG.pData = NULL;
    l_MSG.time = SystemGetMSCount();
    //	m_aMQIterator[priority] = m_Queue.insert(m_aMQIterator[priority], l_MSG);
    //printf("<<<<<<<<<<l_MSG.msg==%x l_MSG.wpa==%x l_MSG.lpa==%x\n",	l_MSG.msg, l_MSG.wpa, l_MSG.lpa);
    m_Queue.push_front(l_MSG);
    m_nMsg++;
    m_MutexMsg.Unlock();

    m_CEZSemaphore.Post();
    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CEZMsgQue::SendMessage(void *p_user_data)
{
    MSG_NODE l_MSG;
    MSGQUEUE::iterator pi;

    m_MutexMsg.Lock();
    if(m_nMsg >= m_nMaxMsg)
    {
        m_MutexMsg.Unlock();
        return EZTHREAD_BOOL_FALSE;
    }
//    if(priority >= MSG_PRIORITY)
//    {
//        m_MutexMsg.Unlock();
//        return EZTHREAD_BOOL_FALSE;
//    }
    if (!m_bMsgFlg)
    {
        m_MutexMsg.Unlock();
        return EZTHREAD_BOOL_FALSE;
    }

    /************************************************************************
    	发送消息:
    	1、按照优先级把该消息插入队列m_Queue；
    //	2、在该队列中查找该消息节点，直到找不到该节点才退出循环并从该函数返回；
    ************************************************************************/
    l_MSG.msg = MSG_On_UserData;
    l_MSG.wpa = 0;
    l_MSG.lpa = 0;
    l_MSG.pData = p_user_data;
    l_MSG.time = SystemGetMSCount();
    //	m_aMQIterator[priority] = m_Queue.insert(m_aMQIterator[priority], l_MSG);
    //printf("<<<<<<<<<<l_MSG.msg==%x l_MSG.wpa==%x l_MSG.lpa==%x\n",	l_MSG.msg, l_MSG.wpa, l_MSG.lpa);
    m_Queue.push_front(l_MSG);
    m_nMsg++;
    m_MutexMsg.Unlock();

    m_CEZSemaphore.Post();
    return EZTHREAD_BOOL_TRUE;
}


EZTHREAD_BOOL CEZMsgQue::RecvMessage(MSG_NODE *pMsg, EZTHREAD_BOOL wait /* = EZTHREAD_BOOL_TRUE */)
{
    /************************************************************************
    	接收消息，如果等待则一直等到有消息时返回，否则直接返回。
    	1、从消息忙队列m_Queue取元素，如果取成功，则直接返回；
    	2、否则循环从消息忙m_Queue中取元素，直到取成功才退出循环；
    ************************************************************************/

    if(wait)
    {
        m_CEZSemaphore.Wait();
    }

    //CEZLock __lock(m_MutexMsg);
    CEZLock __lock(m_MutexMsg);
    //	ez_trace("RecvMessage %d\n", m_nMsg);

    if (m_Queue.empty())
    {
        //		ez_tracepoint();
        return EZTHREAD_BOOL_FALSE;
    }
    if(!wait)
    {
        m_CEZSemaphore.Wait();
    }
    assert(m_nMsg);
    *pMsg = m_Queue.back();
    m_Queue.pop_back();
    //	printf("pMsg==%x  l_MSG.msg==%x l_MSG.wpa==%x l_MSG.lpa==%x\n",	pMsg, l_MSG.msg, l_MSG.wpa, l_MSG.lpa);
    m_nMsg--;

    return EZTHREAD_BOOL_TRUE;
}

void CEZMsgQue::QuitMessage ()
{
    CEZLock __lock(m_MutexMsg);

    m_bMsgFlg = EZTHREAD_BOOL_FALSE;
}

void CEZMsgQue::ClearMessage()
{
    CEZLock __lock(m_MutexMsg);

    int n = m_nMsg;
    for(int i = 0; i < n; i++)
    {
        m_CEZSemaphore.Wait();
        m_Queue.pop_back();
        m_nMsg--;
    }
}

int CEZMsgQue::GetMessageCount()
{
    CEZLock __lock(m_MutexMsg);

    return m_nMsg;
}

int CEZMsgQue::GetMessageSize()
{
    CEZLock __lock(m_MutexMsg);

    return m_nMaxMsg;
}

void CEZMsgQue::SetMessageSize(int size)
{
    CEZLock __lock(m_MutexMsg);

    m_nMaxMsg = size;
}
#ifdef EZLIBS_NAMESPACE
}// namespace ezlibs
#endif

