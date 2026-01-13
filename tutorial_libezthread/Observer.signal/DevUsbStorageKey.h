// DevUsbStorageKey.h: interface for the CDevUsbStorageKey class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEVUSBSTORAGEKEY_H__7AA351FC_02F1_4C02_959C_74FBE94D83F7__INCLUDED_)
#define AFX_DEVUSBSTORAGEKEY_H__7AA351FC_02F1_4C02_959C_74FBE94D83F7__INCLUDED_

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

    BOOL Start();
    BOOL Stop();

    BOOL Start(CEZObject * pObj, DevUsbStorageKeySignalProc_t pProc);
    BOOL Stop(CEZObject * pObj, DevUsbStorageKeySignalProc_t pProc);

    void ThreadProc();

private:
    CDevUsbStorageKey();
    virtual ~CDevUsbStorageKey();

    //DevUsbStorageKeySignalProc_t m_SigBuffer;
    DevUsbStorageKeySignal_t m_SigBuffer;

    CEZMutex m_MutexSigBuffer;

    //¼ÆÊý
    int				m_iUser;
};
#endif // !defined(AFX_DEVUSBSTORAGEKEY_H__7AA351FC_02F1_4C02_959C_74FBE94D83F7__INCLUDED_)



