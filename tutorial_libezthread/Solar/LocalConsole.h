/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * LocalConsole.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: LocalConsole.h 5884 2013-12-26 03:23:28Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-12-26 03:23:28 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __FUNCONSOLE_H__
#define __FUNCONSOLE_H__

#include "EZThread.h"

	typedef const int           int_def;
int_def SYS_MAX_PASSWD_LEN = 6;
int_def SYS_MAX_USERNAME_LEN = 6;
int_def SYS_MAX_GROUPNAME_LEN = 6;


#define  ASCII_CR		0x0d
#define  ASCII_LF		0x0a
#define  ASCII_BS		0x08
#define  ASCII_BELL		0x07
#define  ASCII_TAB		0x09
#define  ASCII_XON		0x11
#define  ASCII_XOFF		0x13
#define  ASCII_ESC		0x1B
#define  ASCII_DEL		0x7F
#define	 ASCII_BACK		0x08

#define SEP_LINE "-----------------------------------------------------"

enum _con_stat
{
    DBG_OFF = 0,	//控制台关闭状态
    DBG_USRNAME,	//用户名输入状态
    DBG_PASSWD,		//密码输入状态
    DBG_LINE,		//命令行状态
};
enum _lvl_t
{
    LVL_NONE = 0,
    LVL_SPRTR,	//分割器
    LVL_USR,
    LVL_SYS,
};


#define N_DBG_CMD 64
#define ARGV_NUM 20

class CLocalConsole;
typedef void (CLocalConsole::*DBG_FXN)();
struct DBG_ATT
{
	int			level;		//等级
	char		*cmdStr;
	DBG_FXN		proc;
	char		*hlpStr;
};
//class CNetWorkService;
class CPacket;
class CDevRecordstream;

class CLocalConsole : public CEZThread
{
private:
	/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
	// 工具
	/// Dump the help message
	void Helpme();
	/// logout
	void iDev_Logout();
	void iDev_SysReboot();

	/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
	// 功能



	/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
	// 工具内部使用
public:
	PATTERN_SINGLETON_DECLARE(CLocalConsole);
	CLocalConsole();
	~CLocalConsole();
	void ThreadProc();
	EZTHREAD_BOOL Start();
	EZTHREAD_BOOL Stop();

private:
	void OnData(void *pDat, int iLength);
	void OnPtzData(void *pDat, int iLength);//add by guoq  for test 485
	char * GetArg(int pos);
	char * GetCmd();
	void StrParse();

	EZTHREAD_BOOL m_bShowContinue;
	//当前的调试功能
	DBG_FXN m_pCurProc;
	int		m_iState;
	int		m_iWordPosition;
	char	m_szData[512];
	char	m_szUserName[SYS_MAX_USERNAME_LEN+1];
	//命令行参数数量
	int		m_iParams;
	char	m_dbParamTab[16][32];
	static	DBG_ATT	m_sDbgTbl[N_DBG_CMD];

	int m_iArgc;
	char *m_pArgv[ARGV_NUM+2];

	//CUgmUser m_cUgmUser;
//	CTimer m_Timer;
//	CTimer m_cTestTimer;
//#if defined(FUNC_FTP_UPLOAD)
//	IFtp* iftp;
//	CONFIG_FTP_PROTO_SET ftpconfig;
//#endif

};

#define g_LocalConsole (*CLocalConsole::instance())

#endif// __FUNCONSOLE_H__
