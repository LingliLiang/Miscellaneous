#include "TimerThread.h"

CTimerThread::CTimerThread() 
	:m_hThread(NULL), m_hEvent(NULL),
	m_lRunning(0), m_nInterval(10),m_callbackFunc(&Callback)
{}


CTimerThread:: ~CTimerThread()
{
	KillTimer(TRUE);
}


void CTimerThread::KillTimer(BOOL bWaitEnd)
{
	if(::InterlockedCompareExchange(&m_lRunning, -1, 1) == 1)
	{
		if (m_hTimer != NULL) {
			CancelWaitableTimer(m_hTimer);
			CloseHandle(m_hTimer);
			m_hTimer = NULL;
		}
		if(m_hThread)
		{
			QueueUserAPC(WakeAPCProc, m_hThread, (ULONG_PTR)this); //»½ÐÑÏß³Ì
			if(bWaitEnd) ::WaitForSingleObject(m_hThread, INFINITE);
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
	}
}


int CTimerThread::CreateTimer(unsigned int interval, void(*pfn)(void*,DWORD,DWORD), void* lpParam)
{
	if (m_lRunning != 0) return TRUE;
	m_nInterval = interval;
	m_lpParam = lpParam;
	::InterlockedExchange(&m_lRunning, 1);
	if(pfn) m_funcCall = std::bind(pfn, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	m_hThread = CreateThread(
		NULL,
		0,
		TimerThread,
		this,
		0,
		&m_dwThread
		);
	if (m_hThread == NULL) {
		::InterlockedExchange(&m_lRunning, 0);
		return FALSE;
	}
	return TRUE;
}


DWORD CTimerThread::TimerThread(LPVOID lpParam)
{
	CTimerThread* pThis = (CTimerThread*)lpParam;
	int bRet = 0;
	::CoInitialize(NULL);
	do {
		pThis->m_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
		if (pThis->m_hTimer == NULL) break;

		LARGE_INTEGER liDueTime;
		liDueTime.QuadPart = 0;
		bRet = SetWaitableTimer(
			pThis->m_hTimer,
			&liDueTime,
			pThis->m_nInterval,
			pThis->m_callbackFunc,
			pThis,
			FALSE);
	} while (0);
	if(bRet){
		while(pThis->m_lRunning == 1){
			SleepEx(30000, TRUE);
		}
	}
	::InterlockedExchange(&pThis->m_lRunning, 0);
	//TRACE("CTimerThread::TimerThread(LPVOID lpParam) END!\n");
	::CoUninitialize();
	return bRet;
}


VOID  CTimerThread::Callback(
	__in_opt LPVOID lpArgToCompletionRoutine,
	__in     DWORD dwTimerLowValue,
	__in     DWORD dwTimerHighValue
	)
{
	//TRACE("lpArgToCompletionRoutine %x dwTimerLowValue %d dwTimerHighValue %d\n",lpArgToCompletionRoutine,dwTimerLowValue,dwTimerHighValue);
	CTimerThread* pThis = (CTimerThread*)lpArgToCompletionRoutine;
	if(pThis && pThis->m_funcCall)
	{
		(pThis->m_funcCall)(pThis->m_lpParam, dwTimerLowValue, dwTimerHighValue);
	}
}


VOID CALLBACK CTimerThread::WakeAPCProc(_In_ ULONG_PTR dwParam)
{
	//TRACE("CTimerThread::WakeAPCProc !\n");
}