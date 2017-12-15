#ifndef _TIMERTHREAD_H
#define _TIMERTHREAD_H

//#include <Windows.h>

#include <map>
#include <string>
#include <functional>

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
	//普通函数Timer创建
	int CreateTimer(unsigned int interval, void(*pfn)(void*,DWORD,DWORD), void* lpParam = 0);
	//类函数Timer创建
	template<class O>
	int CreateTimer(unsigned int interval, O* pO, void(O::*pfn)(void*,DWORD,DWORD), void* lpParam = 0)
	{
		if(!pO || !pfn) return FALSE;
		m_funcCall = std::bind(pfn, pO, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		return CreateTimer(interval,(void(*)(void*,DWORD,DWORD))NULL,lpParam);
	}
	void KillTimer(BOOL bWaitEnd = FALSE);
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
	static VOID CALLBACK WakeAPCProc(
		_In_ ULONG_PTR dwParam
		);
private:
	HANDLE               m_hTimer;
	int                    m_nInterval;
	HANDLE               m_hEvent;
	void*                m_lpParam;

	PTIMERAPCROUTINE   m_callbackFunc;
	HANDLE                m_hThread;
	DWORD                m_dwThread;
	volatile long                m_lRunning;

	std::function<void(void*,DWORD,DWORD)> m_funcCall;
};

#endif //_TIMERTHREAD_H
