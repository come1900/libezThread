/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * api_thread.c - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: api_thread.c 5884 2012-05-16 09:31:37Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2011-10-13 22:00:11   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#define	MODULE_DEBUG		0	/* 1=Enable debug, 0=Disable	*/
#define	SCHED_FIFO_SUPPORT	0	/* 1=support SCHED_FIFO		*/

#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sched.h>
#include <termios.h> 
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
//#include <sys/io.h>
#include <sys/time.h>
#include <sys/times.h>
//#include <sys/shm.h>	
#include <sys/mount.h>
//#ifndef MACOSX

#if defined(MACOSX) || defined(IOS)
// do not use the header of linux
#else
//#include <sys/sysmacros.h>
//#include <linux/types.h>
//#include <string.h>
//#include <linux/hdreg.h>
//#include <linux/fs.h>
//#include <linux/major.h>
//#include <linux/raw.h>
//#include <linux/unistd.h>
//#include <sys/poll.h>
#endif //MACOSX

#include <syslog.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <net/if.h>
#include "typedef_ezthread.h"
//#include "api_system.h"
#include "api_thread.h"

/************************************************************************
 * create thread
 *
 * ret :	= 0		success
 * 		< 0		false
 ************************************************************************/
int ThreadCreate
(
 EZTHREAD__HANDLE *	phthread,
 void		(* tsk)(void* p),
 void *		pdat,
 int		prio,
 unsigned int		stk_sz,
 const char *	name
)
{
	int			policy;
	int			priority_min;
	int			priority_max;
	const char *		failure;
	int			min_stack_size = TSK_DEF_STACK_SIZE;
	pthread_attr_t		attr;
	
	
	for (;;)
	{
		if ((prio > 0) && (prio < 30))
		{
			policy = SCHED_FIFO;
		}
		else
		{
			policy = SCHED_OTHER;
		}
	
		if ((priority_min = sched_get_priority_min(policy)) == -1)
		{
			failure = "sched_get_priority_min";
			printf("failed: %s\n", failure);
			break;
		}
	
		if ((priority_max = sched_get_priority_max(policy)) == -1)
		{
			failure = "sched_get_priority_max";
			printf("failed: %s\n", failure);
			break;
		}
	
		if (prio > priority_max)	prio = priority_max;
		if (prio < priority_min)	prio = priority_min;
		
		if (policy != SCHED_OTHER)
		{
			prio = priority_max - prio;
		}
		else
		{
			prio = 50;
		}
	
		if( stk_sz < min_stack_size )
		{
			stk_sz = min_stack_size;
		}
	
		if (pthread_attr_init(&attr) != 0)
		{
			failure = "pthread_attr_init";
			break;
		}
	
		if (pthread_create((pthread_t *)phthread, NULL,
		     	(void* (*)(void *))tsk, pdat) != 0)
		{
			printf("pthread_create failed\n");
			break;
		}
	
		if (pthread_detach((pthread_t)(*phthread)) != 0)
		{
			failure = "pthread detached";
			printf("failed: %s\n", failure);
			break;
		}
		
#if (SCHED_FIFO_SUPPORT)
		if (policy != SCHED_OTHER)
		{
			if (pthread_setschedparam((pthread_t)(*phthread),
			 policy, (struct sched_param *)(&prio)) != 0)
			{
				failure = "pthread detached";
				printf("failed: %s\n", failure);
				break;
			}
		}
#endif
	
		/* printf("+++++ thread created\n"); */
	
		return 0;
	}
	
	return -1;
} /* end ThreadCreate */

/************************************************************************
 * Destory thread
 *
 * ret :	= 0		success
 * 		< 0		false
 ************************************************************************/
int ThreadDestory(EZTHREAD__HANDLE hthread)
{
	int		ret;
	
	ret = pthread_join((pthread_t)hthread, NULL);
	if (ret != 0)
	{
		errno = ret;
		perror("pthread_join");
		return -ret;
	}
	else
	{
		return 0;
	}

} /* end ThreadDestory */

/************************************************************************
 * thread share socket
 *
 * ret :	socket
 ************************************************************************/
int ThreadShareSocket(EZTHREAD__HANDLE hthread, int nSocket)
{
 return 0;
} /* end ThreadShareSocket */

/************************************************************************
 * Destory thread
 ************************************************************************/
void ThreadExit(void)
{
	pthread_exit(NULL);
} /* end ThreadExit */

/************************************************************************
 * Get thread ID
 ************************************************************************/
#include <sys/syscall.h>

pid_t __gettid(void)
{
#if defined(_ANDROID)
  return syscall(__NR_gettid);
#else
  return syscall(SYS_gettid);
#endif // android
}
 
int ThreadGetID(void)
{
	//return getpid();
	return __gettid();
} /* end ThreadGetID */
