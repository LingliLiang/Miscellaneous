#include "TimerThread.h"

CTimerThread::CTimerThread() 
	:m_hThread(NULL), m_hEvent(NULL),
	m_bRunning(false), m_nInterval(10),m_callbackFunc(&Callback)
{}


CTimerThread:: ~CTimerThread()
{
	KillTimer();
}


void CTimerThread::KillTimer()
{
	if (m_hTimer != NULL) {
		CancelWaitableTimer(m_hTimer);
		CloseHandle(m_hTimer);
		m_hTimer = NULL;
	}
	m_bRunning = false;
}

int CTimerThread::CreateTimer(unsigned int interval, void* lpParam)
{
	if (m_bRunning) return TRUE;
	m_nInterval = interval;
	m_lpParam = lpParam;
	m_bRunning = true;
	m_hThread = CreateThread(
		NULL,
		0,
		TimerThread,
		this,
		0,
		&m_dwThread
		);
	if (m_hThread == NULL) {
		m_bRunning = false;
		return FALSE;
	}
	CloseHandle(m_hThread);
	return TRUE;
}


DWORD CTimerThread::TimerThread(LPVOID lpParam)
{
	CTimerThread* pThis = (CTimerThread*)lpParam;
	do {
		pThis->m_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
		if (pThis->m_hTimer == NULL) break;

		LARGE_INTEGER liDueTime;
		liDueTime.QuadPart = 0;
		BOOL bRet = SetWaitableTimer(
			pThis->m_hTimer,
			&liDueTime,
			pThis->m_nInterval,
			pThis->m_callbackFunc,
			pThis->m_lpParam,
			FALSE);
		do {
			SleepEx(30000, TRUE);
		} while (pThis->m_bRunning);
		return bRet;
	} while (0);
	return 0;
}


VOID  CTimerThread::Callback(
    __in_opt LPVOID lpArgToCompletionRoutine,
    __in     DWORD dwTimerLowValue,
    __in     DWORD dwTimerHighValue
    )
{
	TRACE("lpArgToCompletionRoutine %x dwTimerLowValue %d dwTimerHighValue %d\n",
		lpArgToCompletionRoutine,dwTimerLowValue,dwTimerHighValue);
}