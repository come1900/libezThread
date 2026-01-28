/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FunUsbStorageKey.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: FunUsbStorageKey.h 5884 2012-05-16 09:14:47Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _EZFunUsbStorageKey_H_
#define _EZFunUsbStorageKey_H_

#include <string>
#include "EZObject.h"

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

#endif// _EZFunUsbStorageKey_H_
