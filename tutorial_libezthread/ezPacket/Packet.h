/*
**	********************************************************************************
**                                     Packet
**                          Digital Video Recoder xp
**
**   (c) Copyright 1992-2004, ZheJiang Dahua Information Technology Stock CO.LTD.
**                            All Rights Reserved
**
**	File		: Packet.h
**	Description	: 
**	Modify		: 2005/3/5		WHF		Create the file
**	********************************************************************************
*/

#ifndef __PACKET_H__
#define __PACKET_H__

#if defined(WIN32)
	#pragma warning (disable : 4786)
#endif
#include <deque>

#include <assert.h>
// #include "DVR.H"
#include "EZMutex.h"
#include "EZLock.h"

#define PKT_HDR_SIZE 104

#include "EZPoolAllocator.h"

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD, *PDWORD, *LPDWORD;
typedef void *					HANDLE;
typedef struct tagSYSTEM_TIME{
    int  year;
	int  month;
	int  day;
	int  wday;
	int  hour;
	int  minute;
	int  second;
	int  isdst;
}SYSTEM_TIME;
#define trace printf
#define BITMSK(bit)				(int)(1 << (bit))
#define tracepoint() 			do {trace("tracepoint: %s,%d\n",__FILE__,__LINE__); } while(0)
/************************************************************************
Memory Management
************************************************************************/
typedef struct tagMEMORY_BLOCK
{
	HANDLE		handle;
	BYTE*		ptr;
	BYTE*		header;		//32 bytes
	DWORD		length;
	DWORD		size;
	struct tagMEMORY_BLOCK *next;
	struct tagMEMORY_BLOCK *prev;
	DWORD		flag1;
	DWORD		flag2;
}MEMORY_BLOCK;


class CPacketManager;
////////////////////////////////////////////////////////////////////////////////////
// CPacket
////////////////////////////////////////////////////////////////////////////////////
class CPacket {
	// friend int LocateIFrame(CPacket *pPkt, DWORD *uPos, BYTE *packtype);
	// friend int LocateIFrame(CPacket *pPkt, DHTIME *pDHTIME);
public:
	DWORD	PutBuffer(void * pdat, DWORD dwLength);
	BYTE *	GetBuffer();
	DWORD   SetLength(DWORD dwLength);		//设置包长度	
	DWORD	GetLength();					//取数据长度
	DWORD	GetSize();						//获取容量
	DWORD	GetLeft();						//取剩余容量
	BYTE *	GetHeader();					//清空包头数据
	void	ClearHeader();					//清空包头数据
	DWORD	Release(int iId=-1);
	DWORD	AddRef(int iId=-1);
	DWORD	GetRef();
	void	GetSysTime(SYSTEM_TIME *time);
	void	SetSysTime(SYSTEM_TIME *time);

	int		m_iLastUId;

private:
	BYTE *	m_pBuffer;		//缓冲指针
	DWORD	m_Size;			//大小
	DWORD	m_Length;		//数据长度
	DWORD	m_RefCount;		//引用计数
	CEZMutex	m_Mutex;		//互斥量
	SYSTEM_TIME  m_SysTime;  //保存包的当前时间
	BYTE	m_Header[PKT_HDR_SIZE];
	CPacket *m_pNext;

private:
	CPacket();
	~CPacket();
	void Init();

	friend class CPacketManager;
};

class CPacketManager {
	
	typedef struct _PBN{//packet buffer node
		_PBN *	pPrev;
		_PBN *	pNext;
		int		nIndex;
	}PBN;

	typedef struct _PBA{//packet buffer array
		PBN *	pArray;
		PBN *	pHeader;
		DWORD	nCount;
	}PBA;

public:
	CPacketManager();
	~CPacketManager();
	
public:
	CPacket*	GetPacket(DWORD dwBytes = 0);
	void		PutPacket(CPacket *pPacket);
	DWORD		GetBufferSize();
	void		Dump();
	void		DumpNodes();
	void		Test();

private:
	CPacket*	AllocPacket();
	void		FreePacket(CPacket *p);
	void		InsertFree(PBN * & pHeader, PBN * pThis);
	void		RemoveFree(PBN * & pHeader, PBN * pThis);
	
private:
	BYTE *		m_pOriginBuffer;
	BYTE *		m_pBuffer;
	CEZMutex		m_Mutex;
	PBA 		m_PBAs[32];//管理每种缓冲的节点数组
	int			m_nTypes;
	int			m_nPages;
	enum		{ NALL = 1024 };
	CPacket*	m_pFreeList;	
};

extern CPacketManager g_PacketManager;
#endif// __PACKET_H__
//
// End of "$Id: Packet.h 8082 2007-04-04 02:42:40Z yuan_shiyong $"
//
