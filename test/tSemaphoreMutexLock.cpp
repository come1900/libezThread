/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * tCEZSemaphoreCMutexCLock.cpp - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: tCEZSemaphoreCMutexCLock.cpp 5884 2013-02-06 03:03:39Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-02-06 03:03:39  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "CommonInclude.h"
#include <sys/time.h>
#include <time.h>
#include <string>
#include "EZThread.h"
#include "EZTimer.h"

#include "EZSemaphore.h"
#include "EZMutex.h"
#include "EZLock.h"

class SemCLock
{
public:
    SemCLock(CEZSemaphore& sem) : m_sem(sem)
    {
        m_sem.Wait();
    }
    ~SemCLock()
    {
        m_sem.Post();
    }

private:
    CEZSemaphore& m_sem;
};

static	int val = 0;

void lock(CEZMutex& m, int i)
{
	CEZLock l(m);
	val += i;
}


void lock(CEZSemaphore& s, int i)
{
    SemCLock l(s);
    val += i;
}

#define LOCK_TIMES_FOR_TEST 1000000000

int main(int argc, char* argv[])
{
    CEZSemaphore sema(1);

    time_t ttStart;
    ttStart = time(NULL);
    for (int i = 0; i < LOCK_TIMES_FOR_TEST; i++)
    {
        lock(sema, i)
            ;
    }
    printf("%ld sec with lock CEZSemaphore\n", time(NULL)-ttStart); // 34 sec


	CEZMutex mutex;
    ttStart = time(NULL);
    for (int i = 0; i < LOCK_TIMES_FOR_TEST; i++)
    {
       lock(mutex, i);
    }
    printf("%ld sec with lock mutex\n", time(NULL)-ttStart); // 34 sec

    return 0;
}
