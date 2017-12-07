#ifndef _TIMERTHREAD_H
#define _TIMERTHREAD_H

//#include <Windows.h>

#include <map>
#include <string>
/**
CreateWaitableTimer Document:
http://msdn.microsoft.com/en-us/library/windows/desktop/ms682492(v=vs.85).aspx
Modifyer:

**/

class CTimerThread
{
public:
	CTimerThread();
	~CTimerThread();
public:
	int CreateTimer(unsigned int interval, void* lpParam);
	void KillTimer();
private:
	static DWORD WINAPI TimerThread(LPVOID lpParam);
	static VOID WINAPI Callback(
		//The value passed to the function using the lpArgToCompletionRoutine parameter of the SetWaitableTimer function.
		__in_opt LPVOID lpArgToCompletionRoutine,
		//The low-order portion of the UTC-based time at which the timer was signaled. This value corresponds to the dwLowDateTime
		//member of the FILETIME structure. For more information about UTC-based time, see System Time.
		__in     DWORD dwTimerLowValue,
		//The high-order portion of the UTC-based time at which the timer was signaled. This value corresponds to the dwHighDateTime
		//member of the FILETIME structure.
		__in     DWORD dwTimerHighValue
		);

private:
	HANDLE               m_hTimer;
	int                    m_nInterval;
	HANDLE               m_hEvent;
	void*                m_lpParam;

	PTIMERAPCROUTINE   m_callbackFunc;
	HANDLE                m_hThread;
	DWORD                m_dwThread;
	bool                m_bRunning;
};

#endif //_TIMERTHREAD_H
