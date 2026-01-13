/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Thread.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Thread.cpp 5884 2012-05-16 09:17:46Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//#include "CommonInclude.h"
#include <unistd.h>
#include <string>
#include "EZThread.h"
#include "EZTimer.h"

#include "EZPoolAllocator.h"

#include "Packet.h"

#define g_Producers (*CProducers::instance())

int main(int argc, char* argv[])
{
	int iRet = 0;

	g_PacketManager.Test();
	
	return iRet;
}


