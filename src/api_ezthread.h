/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * api_ezthread.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: api_ezthread.h 5884 2014-08-26 04:50:52Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-08-26 04:50:52  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __api_ezthread_H__
#define __api_ezthread_H__


#ifdef __cplusplus
extern "C"
{
#endif

	/// \defgroup SystemAPI API System
	/// 系统时间设置，关机与重启，系统资源状况，升级等API。
	/// @{

	/// 得到系统的毫秒计数。两次调用之间的间隔时间就是得到的计数的差值。
	///
	/// \param 无
	/// \return 当前计数。
	/// \note 一般情况下，49.7天后得到计数会溢出，使用到的地方要做溢出后处理！
	unsigned int SystemGetMSCount(void);
	unsigned long long process_counter_ms();
	unsigned long long process_counter_sec();

	/// 挂起当前线程，时间到后再唤醒。
	///
	/// \param [in] dwMilliSeconds 挂起的时间。
	void SystemSleep(unsigned int dwMilliSeconds);

	/// @} end of group

#ifdef __cplusplus
}
#endif

#endif //__api_ezthread_H__

