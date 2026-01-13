/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * api_ezthread.c - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: api_ezthread.c 5884 2014-08-26 04:50:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-08-26 04:50:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if defined(_WIN32)

#elif defined(MACOSX) || defined(IOS)
	#include <sys/time.h>
	#include <string.h>
	#include <stdlib.h>
#else //linux
	#include <string.h>
	#include <stdlib.h>
	#include <time.h> /*clock_gettime*/
	#include <unistd.h>/*usleep*/
#endif //

#if defined(MACOSX) || defined(IOS)
unsigned int SystemGetUSCount(void)
{
	struct timeval		tv;
	struct timezone	tz;
	int			ret;

	gettimeofday(&tv, &tz);
	ret = tv.tv_sec * 1000 * 1000 + tv.tv_usec;

	return ret;
} /* end SystemGetUSCount */
#endif //if defined(MACOSX) || defined(IOS)


/************************************************************************
 *  times 在ios中程序崩溃 linux中ok -- wujj 2014-3-24 14:22:48
 ************************************************************************/
unsigned int SystemGetMSCount(void)
{
	unsigned int tCount;

#if defined(_WIN32)
	tCount = GetTickCount();
#elif defined(MACOSX) || defined(IOS)
	tCount = SystemGetUSCount()*1000;
#elif defined(LINUX)
    static struct timespec ts = {0, 0};
    struct timespec ts_tmp;
    int iret;

    // iret = clock_gettime(CLOCK_MONOTONIC, &ts_tmp);
    iret = clock_gettime(CLOCK_MONOTONIC_COARSE, &ts_tmp);// Linux 2.6 or higher
    if (iret == 0)
    {
    	ts.tv_sec = ts_tmp.tv_sec;
    	ts.tv_nsec = ts_tmp.tv_nsec;
    }

    return (unsigned long long)(ts.tv_sec * 1000) + (ts.tv_nsec / 1000 / 1000);
#else
	tCount = 0;
#endif

	return tCount;
} /* end SystemGetMSCount */

//milliseconds since process start
unsigned long long process_counter_ms()
{
	static unsigned int lastTime = 0;
	static unsigned int curTime = 0;
	static unsigned long long adjTime = 0; //两次调用的差值
	static unsigned long long MyMsCount = 0; //自行计数

	if (lastTime==0)
	{
		lastTime = SystemGetMSCount();
	}
	curTime = SystemGetMSCount();

	if (curTime>=lastTime)
	{
		adjTime = curTime-lastTime;
	}
	else
	{
		adjTime = (0xffffffff-lastTime)+curTime;
	}

	MyMsCount += adjTime ;

	lastTime = curTime;

    return (MyMsCount);
}

unsigned long long process_counter_sec()
{
	return process_counter_ms()/1000;
}

/************************************************************************
 *
 ************************************************************************/
void SystemSleep(unsigned int ms)
{
	//usleep(ms * 1000);
	//return;

#if defined(_WIN32)
	Sleep(ms);
#elif defined(MACOSX) || defined(IOS)
	usleep(ms * 1000);
#else //linux
	usleep(ms * 1000);
#endif
} /* end SystemSleep */
