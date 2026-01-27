  /*
**	********************************************************************************
**                                     Packet
**                          Digital Video Recoder xp
**
**   (c) Copyright 1992-2004, ZheJiang Dahua Information Technology Stock CO.LTD.
**                            All Rights Reserved
**
**	File		: Packet.cpp
**	Description	: 实现数据缓冲的伙伴模式管理，将连续的内存块划分成大小相同的页面， 
				  对外提供提供包含连续n个页面缓冲的包，包首地址按页面大小对齐。 并提供
				  了C程序调用的接口。
**
**	Modify		: 2005/3/5		WHF		Create the file
                  2005/6/11     WHF     缓冲修改为4M
**	********************************************************************************
*/

////////////////////////////////////////////////////////////////////////////////////
// CPacket
////////////////////////////////////////////////////////////////////////////////////

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "Packet.h"
// #include "AppConfig.h"

#define PKT_KILOS_DEFAULT (4096)
#define PKT_PAGE_SIZE (1024)

#define FRAME_MAX_NUM (8)

//! 帧定位信息
typedef struct
{
	BYTE FrameType;					/*!< 帧类型 */
	BYTE FrameFlag;					/*!< 帧头尾的标识 */
	WORD FramePos;					/*!< 帧的起始位置 */
	DWORD FrameLength;				/*!< 帧的长度，可能跨块 */
	WORD DataLength;				/*!< 帧在本块中的长度 */
	BYTE Reserve[2];				/*!< 保留 */
}FRAMEINFO;

//! 包头的信息定义
typedef struct
{
	FRAMEINFO FrameInfo[FRAME_MAX_NUM];  /*!< 所有帧信息 */
	BYTE			PacketInfo;								 /*!< 打包信息 */
	BYTE			Reserve[7];								 /*!< 保留 */
}PKT_HEAD_INFO;

#if 0
void printFrameInfo(CPacket *pPkt)
{
	int i = 0;
	char *value = (char *)pPkt->GetHeader();
	for (i = 0; i < FRAME_MAX_NUM; i++)
	{
		trace("%08x:", i);
		for (int j = 0; j < sizeof(FRAMEINFO); j++)
		{
			trace("%02x ", *value);
			value++;
		}
		trace("\n");
	}
	trace("%08x:", i);
	for (i = 0; i < 8; i++)
	{
		trace("%02x ", *value);
		value++;
	}
	trace("\n");

	trace("********************************************************\n");
}
#endif

/*!
	\b Description		:	查找包的I帧\n
	\b Argument			:	CPacket *pkt, DWORD *pos
	\param	pkt			:	欲查找包的指针
	\param  pos			:	找到的I帧在包中的位置(第一个)
	\param  packtype	:	dsp通知的打包类型 zhongyj add 2006-11-07
	\return	找到I帧的个数，0为未找到; -1:指针出错			

	\b Revisions		:	
		- 2005-10-24		yuansy		Create
		- 2006-04-20		yuansy		修改返回值
		- 2006-12-21		yuansy		修改包长度引起的查找帧的变化
*/
#if 0
int LocateIFrame(CPacket *pkt, DWORD *pos, BYTE *packtype)
{
	//视新增加了打包时查找I帧

	/*      帧定位信息
	说明: 帧定位信息一共16字节, 每4个字节为一个记录,一共4个记录
	每个记录组成如下:

	MSB								     LSB
	---------------------------------------------------------------------------------
	|31  30 | 29                      18 | 17                                     0 |
	---------------------------------------------------------------------------------
	|Flag   |     Location               |           Length                        

	Flag  		    00	                    01 						10
	Location       无效                I帧在本块中的定位            无效
	Length         无效                I帧长度                      剩余I帧的计数

	注释: 当本块中I帧数目大于4时, 前3个定位信息指示前3个I帧的定位, 
	最后一个定位信息指示无法详细定位的I帧的数目.
	*/

	/*
	不定长的流模式,每次最多传输8K字节, 也可以少于8K字节
	每次最多传输8帧(视频帧和音频帧), 帧尾部或者中间数据不包含帧头的不算.
	每满8帧必须发起传输(即使不满8K数据)
	每帧都有帧定位信息(这种流模式即可认为是帧模式)
	每个帧定位信息8个字节
	每8K数据有8个帧定位信息

	8位									8位									16位					32位							16位				16位
	帧类型标志					头尾标志							起始位置						长度							段长度			　　保留
	0xFF  无数据				0  非头尾数据					帧在本块中起始位置					帧真正的长度, 可能跨块			帧在本块中的长度
	0     P帧					1  包含帧头
	1     I 帧					2  包含帧尾
	2     B帧					3  包含帧头和帧尾
	3     音频帧				
	4     其他帧

	紧接着一个字节表示标志位。
	在主码流的数据下，１表示提示应用程序打包，０表示不需要打包。
	如果是抓图的情况下， 0表示数据未完，1表示图片数据的开始，２表示数据的结束, 3表示在仅包含在一个数据块中，也就是说在开始的时候同时结束。需要说明的是，两张图片应该放在不同的数据包里，即使前一张图片只占了数据包的很小一部分。

	*/

	DWORD	iframepos = 0xffffffff;
	int		iframenum = 0;

	if ((!pkt) || (!pos) ||(!packtype))
	{
		tracepoint();
		return -1;
	}
#if (!defined(DVR_HB)) && (!defined(WIN32)) && !defined(DVR_NB)
	//printFrameInfo(pkt);
	PKT_HEAD_INFO	*pkthead = (PKT_HEAD_INFO *)(pkt->GetHeader()); //注意由于下面内存已对齐
	
	for (int i = 0; i < FRAME_MAX_NUM; i++)
	{
		if ((pkthead->FrameInfo[i].FrameType == 1) 
		&& ((pkthead->FrameInfo[i].FrameFlag == 1) || (pkthead->FrameInfo[i].FrameFlag == 3)))
		{
			if (iframenum == 0)
			{
				iframepos = pkthead->FrameInfo[i].FramePos;
			}
			iframenum++;
		}
	}
	
	*packtype = pkthead->PacketInfo;
#else
	iframepos = 0;
	iframenum++;
	*packtype = 1;
#endif
	if (iframepos != 0xffffffff)
	{
		*pos = iframepos;
	}

	return iframenum;
}

int LocateIFrame(CPacket *pPkt, DHTIME *pDHTIME)
{
	if (!pDHTIME)
	{
		return -1;
	}

	DWORD dwIFramePos = 0;
	BYTE type = 0;
	if (LocateIFrame(pPkt, &dwIFramePos, &type))
	{
		BYTE *pFrameInfo = pPkt->GetBuffer() + dwIFramePos;
		BYTE *pIFameTime = pFrameInfo + 8;//这儿暂写死
		memcpy((void *)pDHTIME, pIFameTime, sizeof(DHTIME));
		return 0;
	}

	return 1;
}
#endif // iframe
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
CPacketManager g_PacketManager;
int log2i(DWORD value)
{
	int ret = -1;
	while(value)
	{
		value /= 2;
		ret++;
	}
	return ret;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

CPacket::CPacket()
{
}

CPacket::~CPacket()
{
}

void CPacket::Init()
{
	m_RefCount = 1;
	m_Length = 0;
	m_Size = 0;
	m_pBuffer = NULL;
	m_iLastUId = -1;
}

DWORD CPacket::PutBuffer(void * pdat, DWORD dwLength)
{
	DWORD left;

	CEZLock guard(m_Mutex);

	left = m_Size - m_Length;
	if (left < dwLength) {		//包容量不足
		dwLength = left;
	}
	memcpy(m_pBuffer, pdat, dwLength);

	m_Length += dwLength;

	return dwLength;
}

BYTE * CPacket::GetBuffer()
{
	return m_pBuffer;
}


DWORD CPacket::SetLength(DWORD dwLength)
{
	
//	assert(dwLength <= m_Size);
	
	m_Length = dwLength;
	if(m_Length > m_Size){
		m_Length = m_Size;
	}
	return m_Length;
}

DWORD CPacket::GetLength()
{
	return m_Length;
}

DWORD CPacket::GetSize()
{
	return m_Size;
}

DWORD CPacket::GetLeft()
{
	CEZLock guard(m_Mutex);
	return (m_Size - m_Length);
}

BYTE* CPacket::GetHeader()
{
	return m_Header;
}

void CPacket::ClearHeader()
{
	memset(m_Header, 0, PKT_HDR_SIZE);
}

DWORD CPacket::Release(int iId)
{
	m_Mutex.Lock();

	assert(m_RefCount > 0);

	if(m_RefCount == 0){
		trace("CPacket::Release() reffrence count error\n");
		m_Mutex.Unlock();
		return 0;
	}

	--m_RefCount;

	DWORD count = m_RefCount;
	m_Mutex.Unlock();//必须在回收包之前解锁，否则引起严重错误

	if (count == 0) {
		g_PacketManager.PutPacket(this);
	}
	return count;
}

DWORD CPacket::AddRef(int iId)
{
	CEZLock guard(m_Mutex);

	m_RefCount++;
	return m_RefCount;
}

DWORD CPacket::GetRef()
{
	CEZLock guard(m_Mutex);
	return m_RefCount;
}

void CPacket::GetSysTime(SYSTEM_TIME *time)
{
	CEZLock guard(m_Mutex);
	if(NULL == time)
	{
		tracepoint();
		return;
	}
	memcpy(time,&m_SysTime,sizeof(SYSTEM_TIME));
}

void	CPacket::SetSysTime(SYSTEM_TIME *time)
{
	CEZLock guard(m_Mutex);
	if(NULL == time)
	{
		tracepoint();
		return;
	}
	memcpy(&m_SysTime,time,sizeof(SYSTEM_TIME));
//	trace("%d-%d-%d %d:%d:%d\n",m_SysTime.year,m_SysTime.month,m_SysTime.day,m_SysTime.hour,m_SysTime.minute,m_SysTime.second);
}

////////////////////////////////////////////////////////////////////////////////////
// CPacketManager
////////////////////////////////////////////////////////////////////////////////////
CPacketManager::CPacketManager()
{
	trace("CPacketManager::CPacketManager()>>>>>>>>>\n");
	int i;
	// SYSTEM_CAPS syscaps;
	int size;
	PBN *pNode;

	m_pFreeList = NULL;
	 m_nTypes = 0;
	// SystemGetCaps(&syscaps);
	/*if (syscaps.MemoryLeft > 4096)//剩余内存大于4M
	{//DVR应用程序将把剩余的内存减去4M，作为缓冲使用，最大32M
		m_nPages = MIN(syscaps.MemoryLeft - 4096, 32 * 1024);
	}
	else
	{
		trace("CPacketManager::CPacketManager() not enough memory!!!!\n");
		return ;
	}*/
	// m_nPages = (int)AppConfig::instance()->getNumber("Global.Memory.PacketBufSize", PKT_KILOS_DEFAULT) * 1024 / PKT_PAGE_SIZE;
	m_nPages = 32 * 1024 / PKT_PAGE_SIZE;

	m_pOriginBuffer = new BYTE[(m_nPages + 1) * PKT_PAGE_SIZE];
	if (!m_pOriginBuffer)
	{
		trace("CPacketManager::CPacketManager() new buffer failed!!!!\n");
		return ;
	}
	m_pBuffer = (BYTE *)(((unsigned long)m_pOriginBuffer + PKT_PAGE_SIZE) & ~(PKT_PAGE_SIZE - 1));//按页面大小对齐
	
	 
	for(m_nTypes = 0, size = m_nPages; ; m_nTypes++)
	{
		if(m_nPages < (1 << m_nTypes))
		{
			break;
		}

		// 取偶数，为了节点合并方便
		size = ((size + 1 ) & 0xfffffffe);

		m_PBAs[m_nTypes].pArray = new PBN[size];
		if(!m_PBAs[m_nTypes].pArray)
		{
			trace("CPacketManager::CPacketManager() new nodes failed!!!!\n");
		}

		// 所有节点标志清空
		for(i = 0; i < size; i++)
		{
			m_PBAs[m_nTypes].pArray[i].pNext = NULL;
			m_PBAs[m_nTypes].pArray[i].nIndex = i;
		}

		// 出始化空闲节点链表
		if (m_nPages & BITMSK(m_nTypes))
		{
			pNode = &m_PBAs[m_nTypes].pArray[m_nPages / (1 << m_nTypes) - 1];
			m_PBAs[m_nTypes].pHeader = pNode;
			pNode->pNext = pNode;
			pNode->pPrev = pNode;
		}
		else
		{
			m_PBAs[m_nTypes].pHeader = NULL;
		}
		m_PBAs[m_nTypes].nCount = size;
		size /= 2;
	}
	
	Dump();
}

CPacketManager::~CPacketManager()
{
	if(m_pOriginBuffer)
	{
		delete []m_pOriginBuffer;
	}

	for(; m_nTypes; m_nTypes--)
	{
		delete []m_PBAs[m_nTypes-1].pArray;
	}
}

CPacket* CPacketManager::AllocPacket()
{
	CPacket * p = m_pFreeList;
	
	if(p) 
	{
		m_pFreeList = p->m_pNext;
	}
	else 
	{
		CPacket *q = new CPacket[NALL];
		
		// fprintf(stderr, "==>CPacketManager: allocate buffer:0x%08X size:%ld\n", (int)q, NALL * sizeof(CPacket));
		/* 链接自由对象 */
		for(p = m_pFreeList = &q[NALL -1]; q<p; p--) 
		{
			p->m_pNext = p - 1;
		}
		
		(p + 1)->m_pNext = NULL; // 最后一个自由对象的后续为空
	}
	
	return p;
}

void CPacketManager::FreePacket(CPacket* p)
{
	assert(p);

	p->m_pNext = m_pFreeList;
	m_pFreeList = p;
}

CPacket * CPacketManager::GetPacket(DWORD dwBytes /* = 0 */)
{
	CPacket* pPacket;
	int index = 0;
	int type;
	int pages;
	int i;

	CEZLock guard(m_Mutex);

	pPacket = AllocPacket();
	
	if(!pPacket)
	{
		trace("CPacketManager::GetPacket packet error!!!!!!\n");
		return NULL;
	}

	pPacket->Init();

	// 没有缓冲，只有包头的包
	if(dwBytes == 0)
	{
		pPacket->m_Size = 0;
		pPacket->m_pBuffer = NULL;
		return pPacket;
	}

	// 根据缓冲大小计算出节点级别
	pages = (dwBytes + PKT_PAGE_SIZE - 1) / PKT_PAGE_SIZE;
	type = log2i(pages);
	if((1u << type) != (unsigned int)pages)
	{
		type++;
	}
	pages = 1 << type;//强制页面数为2幂次，也就是屏蔽包申请时大包小包互补策略。 WHF 2007-2-28

	// 查找最小的空闲节点
	for(i = type; i < m_nTypes; i++)
	{
		if(m_PBAs[i].pHeader)
		{
			break;
		}
	}
	if(i >= m_nTypes)
	{
		trace("CPacketManager::GetPacket none free node!!!!!!\n");
		FreePacket(pPacket);
		return NULL;
	}

	// 取出最小的空闲节点
	index = m_PBAs[i].pHeader->nIndex;
	RemoveFree(m_PBAs[i].pHeader, &m_PBAs[i].pArray[index]);

	// 填充包数据成员
	pPacket->m_Size = pages * PKT_PAGE_SIZE;
	pPacket->m_pBuffer = m_pBuffer + (index << i) * PKT_PAGE_SIZE;

	//切分大规格的节点
	for(i--; i >= type; i--)
	{
		index *= 2;
		InsertFree(m_PBAs[i].pHeader, &m_PBAs[i].pArray[index + 1]);
	}

	// 分析出多余的空间包含的更小规格空闲节点， 比如申请11个页面时， 实际取到的是16个页面， 还多出5 = 4 + 1个页面
	if((unsigned int)pages != (1u << type))
	{
		for(; i >= 0; i--)
		{
			index = index * 2;
			if(!(pages & BITMSK(i)))
			{
				InsertFree(m_PBAs[i].pHeader, &m_PBAs[i].pArray[index + 1]);
			}
			else
			{
				index += 1;
				pages -= (1u << i);
				if(!pages)
				{
					InsertFree(m_PBAs[i].pHeader, &m_PBAs[i].pArray[index]);
					break;
				}
			}
		}
	}

	return pPacket;
}

void CPacketManager::PutPacket(CPacket *pPacket)
{
	CEZLock guard(m_Mutex);
	int index;
	int theother;
	int type;
	int pages;
	int i;
	int merged = 0; // 0-没有合并 1-正在合并 2-合并完成

	if(!pPacket)
	{
		return;
	}
	if(!pPacket->m_Size || !pPacket->m_pBuffer)
	{
		FreePacket(pPacket);
		return;
	}

	// 取出包数据成员
	pages = pPacket->m_Size / PKT_PAGE_SIZE;
	index = (pPacket->m_pBuffer - m_pBuffer) / PKT_PAGE_SIZE;

	type = log2i(pages);
	if((1u << type) != (unsigned int)pages)
	{
		type++;
		index += pages; // index为要释放的包内存块后紧跟的0级节点序号
	}
	else
	{
		i = type;
		index /= pages; // index为当前节点的序号
		goto post_merge;
	}

	// 分析出包缓冲包含的所有2幂次节点，如 11 = 8 + 2 + 1，从小到大一一进行合并
	for(i = 0; i < type; i++)
	{
		if(index & 0x1)
		{
			if(merged == 0)
			{
				if(m_PBAs[i].pArray[index].pNext)
				{
					merged = 1;
					RemoveFree(m_PBAs[i].pHeader, &m_PBAs[i].pArray[index]);
				}
				else
				{
					merged = 2;
					InsertFree(m_PBAs[i].pHeader, &m_PBAs[i].pArray[index - 1]);
				}
			}
			else if(merged == 2)
			{
				InsertFree(m_PBAs[i].pHeader, &m_PBAs[i].pArray[index - 1]);
			}
		}
		else
		{
			if(merged == 1)
			{
				if(m_PBAs[i].pArray[index + 1].pNext)
				{
					RemoveFree(m_PBAs[i].pHeader, &m_PBAs[i].pArray[index + 1]);
				}
				else
				{
					merged = 2;
					InsertFree(m_PBAs[i].pHeader, &m_PBAs[i].pArray[index]);
				}
			}
		}
		index /= 2;
	}

	//将相邻小节点合并成大节点, 如1->2->4->8, 直到不能再合并为止
	if(merged == 1)
	{
post_merge:
		for(; i < m_nTypes - 1; i++)
		{
			theother = (index % 2) ? (index - 1) : (index + 1);
			if(m_PBAs[i].pArray[theother].pNext)
			{
				RemoveFree(m_PBAs[i].pHeader, &m_PBAs[i].pArray[theother]);
			}
			else
			{
				break;
			}
			index /= 2;
		}

		InsertFree(m_PBAs[i].pHeader, &m_PBAs[i].pArray[index]);
	}

	// 回收包壳对象
	FreePacket(pPacket);
}

// 删除空闲节点
inline void CPacketManager::RemoveFree(PBN * & pHeader, PBN * pThis)
{
	if(pThis == pHeader)
	{
		pHeader = pThis->pNext;
		if(pThis == pHeader) // 只有一个节点
		{
			pThis->pNext = NULL;
			pHeader = NULL;
			return;
		}
	}

	pThis->pPrev->pNext = pThis->pNext;
	pThis->pNext->pPrev = pThis->pPrev;
	pThis->pNext = NULL;
}

// 将新的空闲节点加入链表
inline void CPacketManager::InsertFree(PBN * & pHeader, PBN * pThis)
{
	if(pHeader)
	{
		pHeader->pPrev->pNext = pThis;
		pThis->pPrev = pHeader->pPrev;
		pHeader->pPrev = pThis;
		pThis->pNext = pHeader;
		pHeader = pThis;
	}
	else
	{
		pHeader = pThis;
		pThis->pPrev = pThis;
		pThis->pNext = pThis;
	}
}

//返回缓冲的大小,以KB为单位
DWORD CPacketManager::GetBufferSize()
{
	CEZLock guard(m_Mutex);

	return m_nPages * PKT_PAGE_SIZE / 1024;
}

void CPacketManager::Dump()
{
	int type;
	CEZLock guard(m_Mutex);
	PBN* current;
	int freepages = 0;
#if defined(NVS_VENUS) || defined(NVS_E)
	char tempstr[128];
#endif
	
	//打印利用率
	for(type = 0; type < m_nTypes; type++)
	{
		if(m_PBAs[type].pHeader)
		{
			current = m_PBAs[type].pHeader;
			while (1)
			{
				freepages += 1<<type;
				if(current == m_PBAs[type].pHeader->pPrev)
				{
					break;
				}
				current = current->pNext;
			}
		}
	}
	trace("Packet usage : %dK / %dK, %d%%\n", m_nPages - freepages, m_nPages, 100-100*freepages/m_nPages);

#if 1
	//
	// show user
	//
	/*PPACKET pPACKET;
	CPacket* pPacket;

	for(pPACKET = m_Queue.begin(); pPACKET != m_Queue.end(); pPACKET++)
	{
		pPacket = *pPACKET;

		if (pPacket->m_iLastUId >= 0)
		{
			trace(" %d", pPacket->m_iLastUId);
		} // if
	}// for*/
#ifdef todo_joy
	SYSTEM_TIME stNow;
	SystemGetCurrentTime(&stNow);
	trace("\n----------------------[%02d-%02d %02d:%02d:%02d]\n",
			stNow.month, stNow.day,
			stNow.hour, stNow.minute, stNow.second);
#endif //todo_joy

#endif

}
	
void CPacketManager::DumpNodes()
{
	int type;
	CEZLock guard(m_Mutex);
	PBN* current;
	//打印所有节点的状态
#if 0
	for(type = 0; type < m_nTypes; type++)
	{
		trace("%4d : ", (1u <<type));
		for(int i = 0; i < (1 << type) - 1; i++)
		{
			trace(" ");
		}
		for(i = 0; i < m_PBAs[type].nCount; i++)
		{
			trace("%x", m_PBAs[type].pArray[i].pNext ? 1 : 0);
			for(int j = 0; (j < (2 << type) - 1) && (i != m_PBAs[type].nCount - 1); j++)
			{
				trace(" ");
			}
		}
		trace("\n");
	}
#endif
	//打印空闲链表
	trace("______________________________\n");
	for(type = 0; type < m_nTypes; type++)
	{//顺序打印
		trace("%4d : ", (1u <<type));
		if(m_PBAs[type].pHeader)
		{
			current = m_PBAs[type].pHeader;
			while (1)
			{
				trace("%d ", current->nIndex);
				if(current == m_PBAs[type].pHeader->pPrev)
				{
					break;
				}
				current = current->pNext;
			}
		}
		trace("\n");
	}

	trace("\n");
}
#define _DEBUG_THIS 1
void CPacketManager::Test()
{
#ifdef _DEBUG_THIS
	int i;
	int count = 1024;
	unsigned long size = 0;
	unsigned long page = 0;
	unsigned long len;
	CPacket * pPacket[1024];
	// SYSTEM_TIME systime;

	// SystemGetCurrentTime(&systime);
	memset(pPacket, 0 , sizeof(CPacket *) * count);
	DumpNodes();
	// srand(systime.second);
	srand(time(NULL));
	for(i=0; i<count; i++)
	{
		len = ((rand() % 65536) + 1);
		//len = (1 << log2i(len));
		pPacket[i] = g_PacketManager.GetPacket(len);
		if(!pPacket[i]){
			break;
		}
		size += len;
		page += (len + PKT_PAGE_SIZE - 1) / PKT_PAGE_SIZE;
	}
	trace("CPacketManager::Test() rand allocate usage: in byte %ld%%, in page %ld%%.\n"
	, 100*size/PKT_PAGE_SIZE/m_nPages, 100*page/m_nPages);
	DumpNodes();
	for(i=0; i<count; i++)
	{
		if(pPacket[i]){
			pPacket[i]->Release();
		}
	}
	DumpNodes();
#endif
}

////////////////////////////////////////////////////////////////////////////////////
/////////////////  提供给C调用的函数。
int MemoryAlloc (MEMORY_BLOCK * pBlock, DWORD dwBytes)
{
	assert(pBlock != NULL);

	CPacket *pPacket = g_PacketManager.GetPacket(dwBytes);
	if (pPacket)
	{
		pBlock->handle = (HANDLE)pPacket;
		pBlock->length = pPacket->GetLength();
		pBlock->size = pPacket->GetSize();
		pBlock->ptr = pPacket->GetBuffer();
		pBlock->header = pPacket->GetHeader();
		return 0;
	}
	return -1;
}

int MemorySetLength(HANDLE hMemory, DWORD dwBytes)
{
	assert(hMemory != NULL);
	
	CPacket * pPakcet = (CPacket *)hMemory;
	pPakcet->SetLength(dwBytes);
	return 0;
}

int MemoryAddRef(HANDLE hMemory)
{
	assert(hMemory != NULL);

	CPacket * pPakcet = (CPacket *)hMemory;
	pPakcet->AddRef();
	return 0;
}

int MemoryRelease(HANDLE hMemory)
{
	assert(hMemory != NULL);

	CPacket * pPakcet = (CPacket *)hMemory;
	pPakcet->Release();
	return 0;
}



//
// End of "$Id: Packet.cpp 8836 2007-05-10 09:20:04Z ni_xiaofei $"
//
