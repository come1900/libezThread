// FunUsbStorageKey.h: interface for the CFunUsbStorageKey class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUNUSBSTORAGEKEY_H__B67EB36D_A812_4A1B_BB73_D3526D803C5C__INCLUDED_)
#define AFX_FUNUSBSTORAGEKEY_H__B67EB36D_A812_4A1B_BB73_D3526D803C5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFunUsbStorageKey : public CEZObject
{
    std::string m_strName;

public:
	//CFunUsbStorageKey();
	//virtual ~CFunUsbStorageKey();

    void OnDownload(int iCMD, int *pPacket, int iCh, time_t time);

    void Start();
    void Stop();

    CFunUsbStorageKey(std::string name="CFunUsbStorageKey");
    virtual ~CFunUsbStorageKey();

};
#endif // !defined(AFX_FUNUSBSTORAGEKEY_H__B67EB36D_A812_4A1B_BB73_D3526D803C5C__INCLUDED_)
