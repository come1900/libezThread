/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * EZPoolAllocator.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: pool_allocator.cpp 5884 2012-05-16 09:10:18Z WuJunjie $
 *
 *  Explain:
	1, 参考"The C++ Programming Laguage (Special Edition)", by Bjarne Stroustrup 

	2, 只支持list容器, 因为list容器每次只申请或释放一个一种规格的节点. 对于deque, 当元素规格小于
	4K时, 其默认的申请策略就相当于内存池, 大于等于4K时, 会申请两种类型的规格, 一种是元素指针, 一次
	申请的个数采用采用番倍策略, 另一种是元素, 这时才可以考虑内存池.  对vector使用池管理实在没什么意
	义, 其内存管理策略也已经考虑了动态增长的情况, 如果试图实现内存池, 将付出沉重代价.

	3, 基于'Chunk'实现, Chunk大小默认值是1024个元素的大小, 可以通过接口setchunksize来设置, 
	最好在初始化的时候只调用一次.

	4, 暂不支持内存释放, 所以申请的内存只会增加, 不会减少.

	5, 线程安全, 即多个线程可以共享一个同类型的内存池.

	6, 此Allocator可以单独的使用, 以能达到内存池管理的目的, 比如用于固定大小数据包内存的管理.
 *
 *  Update:
 *     2011-10-13 22:00:11   Create
 */


#include "EZPoolAllocator.h"
#ifdef EZLIBS_NAMESPACE
namespace ezlibs {
#endif

CEZPool::CEZPool(unsigned int sz)
:esize(sz < sizeof(Link) ? sizeof(Link) : sz),
ecount(ecount_default)
{
	head = 0;
	chunks = 0;
}

//free all chunks
CEZPool::~CEZPool()
{
	Chunk* n = chunks;
	while(n)
	{
		Chunk *p = n;
		n = n->next;
		delete p; //the data memory of 'chunk' will also be freed
	}
}

//allocate new 'chunk' and its space, organize it as a linked list of element of size 'esize'
void CEZPool::grow()
{
	//ez_trace("CEZPool::grow %d bytes\n", esize * ecount + sizeof(Chunk));

	void *buff = new char[esize * ecount + sizeof(Chunk)];
	Chunk *n = new (buff) Chunk;
	n->mem = reinterpret_cast<char*>(n + 1);
	n->next = chunks;
	chunks = n;

	char *start = n->mem;
	char *last = &start[(ecount - 1) * esize];
	for (char *p = start; p < last; p += esize)
	{
		reinterpret_cast<Link*>(p)->next = reinterpret_cast<Link*>(p + esize);
	}
	reinterpret_cast<Link*>(last)->next = 0;
	head = reinterpret_cast<Link*>(start);
}

//if you want to change the defaut element count in a 'chunk',
//you the best call the function when initializing and only once
void CEZPool::setec(unsigned int n)
{
	mutex.Lock();
	ecount = n;
	mutex.Unlock();
}

unsigned int CEZPool::getec()
{
	return ecount;
}
#ifdef EZLIBS_NAMESPACE
}// namespace ezlibs
#endif
