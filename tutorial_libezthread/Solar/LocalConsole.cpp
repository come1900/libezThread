/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * LocalConsole.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: LocalConsole.cpp 5884 2013-12-26 03:28:44Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-12-26 03:28:44 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include <string.h>

#include "LocalConsole.h"

#ifndef _DEBUG_THIS
//  #define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define NO_UGM_MODEL 1 //没有用户管理
#define PROMPT_CONSOLE  "Sunflower"

#define CMD_PUB_HELP  "help"
//#define CMD_PUB_START "start"
//#define CMD_PUB_RESTART "restart"
//#define CMD_PUB_STATUS "status"
//#define CMD_PUB_STOP  "stop"
//#define CMD_PUB_DUMP 		"d"
//#define CMD_PUB_SET 		"s"

#ifndef trace
#define trace printf
#endif

DBG_ATT CLocalConsole::m_sDbgTbl[N_DBG_CMD] = {
            //Basic Function
            { LVL_SPRTR,"------",	NULL,				"Basic Function --------------------",},
            { LVL_USR, CMD_PUB_HELP,		&CLocalConsole::Helpme,		"Dump the help message!",},
            { LVL_USR, "quit",	    &CLocalConsole::iDev_Logout,		"logout",},
            { LVL_USR, "reboot",	    &CLocalConsole::iDev_SysReboot,		"reboot",},

            //End

            { LVL_NONE,"none",		NULL,					"The end!",},
        };

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

void CLocalConsole::iDev_Logout()
{
    m_iState = DBG_OFF;
    //m_cUgmUser.Logout(LoginAs_Net);
    trace("iDev_Logout.\n");
}

void CLocalConsole::iDev_SysReboot()
{
    trace("iDev_SysReboot ...\n");

    //g_Sunflower.Reboot();
}


/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void CLocalConsole::Helpme()
{
    int i;
    //	char *pszCmd0=GetCmd();

    for(i=0; i<N_DBG_CMD; i++)
    {
        if (m_sDbgTbl[i].level == LVL_SPRTR)
        {
            trace("------------------%s\n",m_sDbgTbl[i].hlpStr);
            continue;
        }
        else if (m_sDbgTbl[i].level == LVL_NONE)
        {
            break;
        }
        //		if (m_sDbgTbl[i].level == LVL_SYS && (!pszCmd0 || pszCmd0[0]!='s'))
        //		{
        //			continue;
        //		}
        trace("%-9s         %s\n", m_sDbgTbl[i].cmdStr, m_sDbgTbl[i].hlpStr);
    }
    trace("-----------------------------------------------------\n");
    trace("`cmd -h` for details.\n");
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
PATTERN_SINGLETON_IMPLEMENT(CLocalConsole);

void CLocalConsole::ThreadProc()
{
    //	char buf[512];
    unsigned char ch = 0;

    while (m_bLoop)
    {
        fflush(stdout);

        ch = (char)getchar();
        if(ch)
        {
            DBG(
                tracepoint();
                trace("0x%0x\n", ch);
            )
            OnData(&ch, 1);
        }
    }
}

CLocalConsole::CLocalConsole() : CEZThread("LocalConsole", THREAD_PRIORITY_COM, 1024)/*, m_cUgmUser(CUgmManage::instance())*//*, m_Timer("Console")*/
{
    m_bShowContinue = EZTHREAD_BOOL_FALSE;
    m_iState = DBG_OFF;
    m_iWordPosition  = 0;

    //m_pDevRecordstream = new CDevRecordstream;
}

CLocalConsole::~CLocalConsole()
{}

EZTHREAD_BOOL CLocalConsole::Start()
{
    trace("CLocalConsole::Start()>>>>>>>>>>>>>>>>>\n");

    CreateThread();

    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CLocalConsole::Stop()
{
    DestroyThread();
    return EZTHREAD_BOOL_TRUE;
}

/// 取到数据的调用
///
/// \param [in] pCaps 数据指针
/// \param [in] iLength 数据长度
/// \retval 0  获取成功。
/// \retval -1  获取失败。
void CLocalConsole::OnData(void *pDat, int iLength)
{
    char  l_dbData[2];
    EZTHREAD_BOOL l_bRet;
    char *l_pdbData = (char *)pDat;

    while(iLength--)
    {
        l_dbData[0] = *l_pdbData++;


        if(m_bShowContinue)
        {
            if(l_dbData[0] == 'q' || l_dbData[0] == 'Q') // 'q'为退出键
            {
                m_bShowContinue = EZTHREAD_BOOL_FALSE;
                trace("\nDEBUG>");
            }
            else
            {
                (this->*m_pCurProc)();	// ?
                if(m_bShowContinue)
                {
                    trace("press 'Q' to quit, any other key to continue ...\n\n");
                }
                else
                {
                    trace("\nDEBUG>");
                }
            }
            continue;
        }
        switch (l_dbData[0])
        {
            //	break;
            case ASCII_LF:
            case ASCII_CR:
            switch(m_iState)
            {
                case DBG_OFF:
                {
                    trace("\f\n(none) login:");
                    m_iState = DBG_USRNAME;
                    m_iWordPosition = 0;
                    break;
                }

                case DBG_USRNAME:
                {
                    m_szData[m_iWordPosition++] = '\0';
                    memcpy(m_szUserName, m_szData, SYS_MAX_USERNAME_LEN);
                    m_szUserName[SYS_MAX_USERNAME_LEN] = '\0';
                    trace("\nPassword:");
                    m_iWordPosition = 0;
                    m_iState = DBG_PASSWD;

                    break;
                }
                case DBG_PASSWD:
                {
                    m_szData[m_iWordPosition++] = '\0';
                    //
                    // 调试用户作为网络用户登录，确保本地登录只有一个用户
                    //
#ifdef NO_UGM_MODEL

                    l_bRet = EZTHREAD_BOOL_TRUE;

                    if (m_szUserName[0] == '\0')
                    {
                        strcpy(m_szUserName, "ezlibs");
                    }
#else
                    l_bRet = m_cUgmUser.Login(m_szUserName, m_szData, LoginAs_Net);
#endif
                    //std::string u_name = m_szUserName;//m_cUgmUser.Login(m_szUserName, m_szData, LoginAs_Net);

                    if (EZTHREAD_BOOL_FALSE == l_bRet)
                    {
                        trace("\nUser not valid!");
                        trace("\n(none) login:");
                        m_iWordPosition = 0;
                        m_iState = DBG_USRNAME;
                    }
                    else
                    {
                        m_iWordPosition = 0;
                        m_iState = DBG_LINE;
                        trace("\n[%s@%s]# "
#ifdef NO_UGM_MODEL
                              , m_szUserName
#else
                              //, /* m_cUgmUser.GetUser().*/u_name.c_str()
#endif
                              , PROMPT_CONSOLE);
                    }
                    break;
                }
                case DBG_LINE:
                {
                    if (m_iWordPosition > 0)
                    {
                        m_szData[m_iWordPosition++] = '\0';
                        l_dbData[0] = ASCII_CR;
                        l_dbData[1] = 0;
                        trace(&l_dbData[0]);
                        l_dbData[0] = ASCII_LF;
                        l_dbData[1] = 0;
                        trace(&l_dbData[0]);
                        StrParse();
                        m_iWordPosition = 0;
                    }
                    if(m_bShowContinue)
                    {
                        trace("	press 'Q' to quit, any other key to continue ...\n\n");
                    }
                    else
                    {
                        trace("\n[%s@%s]# "
#ifdef NO_UGM_MODEL
                              , m_szUserName
#else
                              //, /* m_cUgmUser.GetUser().*/u_name.c_str()
#endif
                              , PROMPT_CONSOLE);
                    }
                    break;

                }
                break;
            }

            case ASCII_BACK:
            {
                if (m_iWordPosition)
                {
                    trace("\b \b");
                    m_iWordPosition--;
                }
                break;
            }

            default:
            {
                m_szData[m_iWordPosition++] = l_dbData[0];
                if(m_iState == DBG_PASSWD)
                {
                    l_dbData[0] = '*';
                }
                l_dbData[1] = 0;
                trace(&l_dbData[0], 1);
                if (m_iWordPosition >= 500)
                {
                    m_iWordPosition = 0;
                }
            }
        }
    }
    return;
}

/*	$FXN :
==	======================================================================
==	$DSC :	解析在命令行输入的字符串，并把解析出来的命令和参数都放在m_dbParamTab变量中
==	$ARG :
==		 :
==	$RET :
==	$MOD :
==	======================================================================
*/
void CLocalConsole::StrParse()
{
    int i = -1;
    int	j = 0;
    int k = 0;

    m_iParams = 0;
    while (m_szData[k])
    {
        if (k >= 512)
        {
            break;
        }
        if (m_szData[k] == ' ')
        {
            if (i >= 0)
                m_dbParamTab[i][j] = '\0';
            j = 0;
            k ++;
            continue;
        }
        if (j == 0)
        {
            if (++i >= 16)
                break;
            m_iParams = i;
        }
        m_dbParamTab[i][j++] = m_szData[k];
        if (j >= 32)
        {
            while (m_szData[k])
            {
                if (k >= 512)
                {
                    break;
                }
                if (m_szData[k] == ' ')
                {
                    break;
                }
                k ++;
            }
        }
        k ++;
    }
    m_dbParamTab[i][j] = '\0';

    if (strlen(m_dbParamTab[0]) < 3)
    {
        //trace("\nUnknow command:%s\n", m_dbParamTab[0]);
        trace("\nUnknow command:`%s`, '%s' show command.\n", m_dbParamTab[0], CMD_PUB_HELP);
        return;
    }

    for (i=0; i<N_DBG_CMD; i++)
    {
        //	trace("i->%d, s1->%s, s2->%s, l->%d\n", i, m_dbParamTab[0], m_sDbgTbl[i].cmdStr, strlen(m_dbParamTab[0]));
        if (m_sDbgTbl[i].level == LVL_NONE)
        {
            break;
        }
        if (strncmp(m_dbParamTab[0], m_sDbgTbl[i].cmdStr, strlen(m_dbParamTab[0])) == 0)
        {
            m_pCurProc = m_sDbgTbl[i].proc;
            ///////////////
            m_iArgc = 1;
            m_pArgv[0] = m_dbParamTab[0];
            m_pArgv[1] = GetCmd();
            if (NULL == m_pArgv[1])
            {
                trace("no cmd.\n");
            }
            else
            {
                m_iArgc++;

                for (int ii=0; ii<=20; ii++)
                {
                    m_pArgv[ii+2] = GetArg(ii);
                    if (NULL == m_pArgv[ii+2])
                    {
                        break;
                    }
                    else
                    {
                        //trace("m_pArgv[%d] = %s\n", ii+2, m_pArgv[ii+2]);
                        m_iArgc++;
                    }
                }
            }

            //			trace("m_iArgc:%d\n", m_iArgc);

            ///////////////
            (this->*m_sDbgTbl[i].proc)();

            return;
        }
    }

    //trace("\nUnknow command:`%s`, 'help' show command.\n", m_dbParamTab[0]);
    trace("\nUnknow command:`%s`, '%s' show command.\n", m_dbParamTab[0], CMD_PUB_HELP);
}

/// 取命令接口
///
/// \retval 0  获取成功。
/// \retval -1  获取失败。
char * CLocalConsole::GetCmd()
{
    char *str = m_dbParamTab[1];

    if (m_iParams < 1)
    {
        return 0;
    }
    if (str[0] != '-' && str[0] != '/')
    {
        return m_dbParamTab[1];
    }
    return &m_dbParamTab[1][1];
}

/// 取命令参数
///
/// \param [in] pos 参数位置
/// \retval *  获取成功。
/// \retval NULL  获取失败。
char * CLocalConsole::GetArg(int pos)
{
    if (m_iParams < (pos + 2))
    {
        return 0;
    }
    return m_dbParamTab[pos + 2];
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

