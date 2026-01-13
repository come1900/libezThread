// DevUsbStorageKey.cpp: implementation of the CDevUsbStorageKey class.
//
//////////////////////////////////////////////////////////////////////

#include "DevUsbStorageKey.h"

#ifndef _DEBUG_THIS
    #define _DEBUG_THIS
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

PATTERN_SINGLETON_IMPLEMENT(CDevUsbStorageKey);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CDevUsbStorageKey::CDevUsbStorageKey() :CEZThread("CDevUsbStorageKey", TP_DEFAULT), m_SigBuffer(2/*SIGNAL_NODE_NEW*/)
{
    m_iUser = 0;
    trace("CDevUsbStorageKey Enter--------\n");
}

CDevUsbStorageKey::~CDevUsbStorageKey()
{
    trace("CDevUsbStorageKey Leave--------\n");

}

BOOL CDevUsbStorageKey::Start(CEZObject * pObj, DevUsbStorageKeySignalProc_t pProc)
{
    CEZLock __lock(m_MutexSigBuffer);

    BOOL bRet = FALSE;

    if(m_SigBuffer.Attach(pObj, pProc) < 0)
    {
        __fline;
        printf("attach error\n");
        return bRet;
    }

    if (m_iUser==0)
    {
        CreateThread();

        bRet = TRUE;
    }
    else
    {

        bRet = TRUE;

        DBG(
            __fline;
            printf("no need, CreateThread, have %d users.\n", m_iUser);
        );

    }
    m_iUser++;

    __fline;
    printf("m_iUser:%d\n", m_iUser);

    return bRet;

}
BOOL CDevUsbStorageKey::Stop(CEZObject * pObj, DevUsbStorageKeySignalProc_t pProc)
{
    printf("CDevUsbStorageKey::Stop\n");
    BOOL bRet = FALSE;

    CEZLock __lock(m_MutexSigBuffer);

    // 这不科学， 但是为了避免detach失败 而不停止线程
    // 使用者自行注意start和stop成对调用
    if(m_iUser>0)
    {
        m_iUser--;
    }


    if(m_SigBuffer.Detach(pObj, pProc) == 0)
    {
        // good
        // m_iUser--;
        bRet = TRUE;
    }
    else
    {
        // error
        __trip;
        printf("Detach failed.\n");
    }

    if (m_iUser==0)
    {

        bRet = DestroyThread(TRUE);

        if (FALSE == bRet)
        {
            __trip;
            printf("DestroyThread failed.\n");
        }
        else
        {
            DBG(
                printf("DestroyThread OK.\n");
            );
        }
    }
    else
    {
        DBG(
            __fline;
            printf("no need, DestroyThread, have %d users.\n", m_iUser);
        );
    }

    return bRet;
}

void CDevUsbStorageKey::ThreadProc()
{
    int ii=0;
    while (m_bLoop)
    {
        DBG(
            __fline;
            printf("CDevUsbStorageKey::ThreadProc.\n");
        );

        // 发送消息 (unsigned int msg, PARAM wpa = 0, PARAM lpa = 0, unsigned int priority = 0);
        //g_Consumers.SendMessage(100, ii, ii-1, 0);

        // 回调
        m_SigBuffer(/*HDISKREADER_CMD_FINISHED*/1, NULL, ii, time(NULL));

        ii++;

        //usleep(100);
        sleep(2);
    }

    //delete cfgnetcommon;
}

