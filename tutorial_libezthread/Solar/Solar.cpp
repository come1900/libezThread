/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Solar.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Solar.cpp 5884 2012-05-16 09:17:46Z WuJunjie $
 *
 *  Explain:
 *     -
 *      一般主调类
 *     -
 *
 *  Update:
 *     2012-05-16 09:17:46   WuJunjie  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <iostream>
//#include <ez_fs.h>
#include "Logs.h"

#include "EZThread.h"
#include "EZTimer.h"

#include "Solar.h"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) (void)&x;

// config fname
const char * const ConfigFileSolar_1= "smartPorts.cfg";
const char * const ConfigFileSolar_2 = "smartPorts.cfg.duplicate";

//const char * const copy_right = "@(#) Copyright (c) 2013 ezlibs.com.  All rights reserved";
const char * const copy_right = "Copyright (C) 2013 ezlibs.com, All Rights Reserved.";
const char * const version_string = "v 0.9.0 - ";//2013-06-25 11:16:53
//const char * const version_string = "v 1.0.0 - ";
const char * const desc="P_2013-6-25.SmartPorts.1010";

const char * const INSTANCE_PID_FILE = "/tmp/SmartPorts.pid";

int already_running(const char *filename);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
PATTERN_SINGLETON_IMPLEMENT(CSolar);

CSolar::CSolar():m_TimerMaintain("CSolar-Maintain")
{
    m_strVer = version_string;
    m_strVer += __DATE__;
    m_strVer += " ";
    m_strVer +=__TIME__;

    m_iRunPeriod = 0;
    m_ttStartTime = time(NULL);
}

CSolar::~CSolar()
{
    //g_Config.saveFile();
}

void CSolar::ShowVersion()
{
    printf("---------------------------\n\n");
    printf("%s\n", copy_right);
    printf("%s\n", desc);
    printf("$ Solar: main.c,v 2.6 %s %s Exp $\n\n", __DATE__, __TIME__);
    printf("---------------------------\n");
}

void CSolar::Initialize(int argc, char * argv[])
{
    ShowVersion();

    g_ThreadManager.RegisterMainThread(ThreadGetID());
    g_TimerManager.Start();

    std::cout <<  "Starting(" << desc << ") ..." << std::endl;

    //g_Config.initialize(ConfigFileSolar_1, ConfigFileSolar_2);
    InitializeLogs();

    LOG4CPLUS_INFO(LOG_SOLAR, "Starting(" << desc << ") ...");

    // comment it when development
#if 0

    LOG4CPLUS_INFO(LOG_SOLAR, "is already_running ...");
    if (already_running(INSTANCE_PID_FILE) != 0)
    {
        // important msg, write to console and log files.
        LOG4CPLUS_ERROR(LOG_SOLAR, "Allowed to run only one instance, pls check");
        LOG4CPLUS_INFO(LOG_SOLAR, "Exit ...");

        printf("Allowed to run only one instance, pls check\n");
        printf("Exit ...\n");

        exit(-1);
    }
#endif

    //begin -- start server
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    //end -- start server
    //g_Maintenance.Start(/*bUseCfgAtStartForce*/false, /*bMaintenanceAuto*/true);


    // 启动完毕， 保存一次配置文件
    //g_Config.saveFile();
    LOG4CPLUS_INFO(LOG_SOLAR, argv[0] << " Initialize finished");
    printf("%s Initialize end, running ...\n", argv[0]);

}


int CSolar::InitializeLogs()
{
    //	初始化日志系统


    return 0;
}

void CSolar::Done()
{
    while (1)
    {
        CEZSemaphore sem;
        sem.Wait();
    }
}

void CSolar::Term()
{
    LOG4CPLUS_INFO(LOG_SOLAR, "CSolar::Term");
    LOG4CPLUS_INFO(LOG_SOLAR, "StopSmartWebs ...");

#ifdef USE_SMARTWEBS

    g_WebsSmartPorts.Stop();
#endif//USE_SMARTWEBS

    //m_TimerMaintain.Stop();

    return;
}

int CSolar::Reboot()
{
    return 0;//ez_sys_reboot();
}
int CSolar::Shutdown()
{
    return 0;
}

int CSolar::Restart()
{
    return 0;
}

int CSolar::Restore()
{
    DBG(__fline;printf("g_Config.SetDefaultConfig(CFG_IDX_ALL);\n"););
    // 全部恢复

    return 0;
}
void CSolar::haveException(int ExcNo, char *pMsg)
{
    LOG4CPLUS_INFO(LOG_SOLAR, "CSolar::haveException:" << ExcNo << " Msg:" << pMsg);
    
    std::cout <<  "CSolar::haveException:" << ExcNo << " Msg:" << pMsg << std::endl;

    return;
}

/* set advisory lock on file */
int lockfile(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK;  /* write lock */
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;  //lock the whole file

    return(fcntl(fd, F_SETLK, &fl));
}

int already_running(const char *filename)
{
    int fd;
    char buf[16];

    fd = open(filename, O_RDWR | O_CREAT, (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
    if (fd < 0)
    {
        printf("can't open %s: %m\n", filename);
        exit(-1);
    }

    /* 先获取文件锁 */
    if (lockfile(fd) == -1)
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            //printf("file: %s already locked\n", filename);
            close(fd);
            return -1;
        }
        printf("can't lock %s: %m\n", filename);
        exit(-1);
    }
    /* 写入运行实例的pid */
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);

    return 0;
}

void CSolar::TimerProcMaintain()
{
    // 为了计数准确，不依赖系统时间，自己维护
    m_iRunPeriod ++;
}



const std::string & CSolar::GetVersion()
{
    return m_strVer;
}
const char *CSolar::GetCopyRights()
{
    return copy_right;
}

unsigned int CSolar::GetRunPeriod()
{
    return m_iRunPeriod;
}

time_t CSolar::GetStartTime()
{
    return m_ttStartTime;
}
