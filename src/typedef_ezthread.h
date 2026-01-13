/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * typedef_ezthread.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: typedef_ezthread.h 5884 2012-05-16 09:16:43Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __TYPEDEF_EZTHREAD_H__
#define __TYPEDEF_EZTHREAD_H__

#include <stdio.h>

typedef unsigned int			EZTHREAD_PARAM;
typedef void *					EZTHREAD__HANDLE;
typedef int						EZTHREAD_BOOL;

#define EZTHREAD_BOOL_TRUE					1
#define EZTHREAD_BOOL_FALSE					0

#define ez_trace printf
#define ez_tracepoint() 			do {ez_trace("ez_tracepoint: %s,%d\n",__FILE__,__LINE__); } while(0)

enum xm_msg_t {
	XM_MSG_SYSTEM = 0x000000,
		XM_QUIT,

	XM_SYSTEM_END
};

// 线程优先级
enum __thread_priority {
    THREAD_PRIORITY_COM     =   1 ,
    THREAD_PRIORITY_TIMER   =   2 ,
    THREAD_PRIORITY_ALARM   =   3 ,
    THREAD_PRIORITY_CAPTURE =  20 ,
    THREAD_PRIORITY_PTZ     =  26 ,
    THREAD_PRIORITY_SMTP    =  30 ,
    THREAD_PRIORITYWATCH_DOG  =  31 ,
    THREAD_PRIORITY_RTP     =  32 ,
    THREAD_PRIORITY_BACKUP  =  35 ,
    THREAD_PRIORITY_SVR     =  37 ,
    THREAD_PRIORITY_NET     =  39 ,
    THREAD_PRIORITY_TEL     =  55 ,
    THREAD_PRIORITY_MOUSE   =  57 ,
    THREAD_PRIORITY_FTP     =  58 ,
    THREAD_PRIORITY_WEB     =  58 ,
    THREAD_PRIORITY_DEFAULT =  64 ,
    THREAD_PRIORITY_JWORKER       ,// too Many
    THREAD_PRIORITY_PRE     = 127 ,
};

#endif //__TYPEDEF_EZTHREAD_H__
