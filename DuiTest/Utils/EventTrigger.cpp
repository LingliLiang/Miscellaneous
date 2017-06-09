#include "StdAfx.h"
#include "EventTrigger.h"

namespace ETRIGGER
{

	DWORD WINAPI CEventTrigger::DispatchProc(LPVOID lpParam)
	{
		CEventTrigger* pThis = static_cast<CEventTrigger*>(lpParam);
		while (TRUE)
		{
			::WaitForSingleObject(pThis->m_hDispEvent, INFINITE);
			if(pThis->m_dispThreadID == 0)
				break;
			DISPINFO dispInfo;
			EnterCriticalSection(&pThis->m_dispCS);
			if (pThis->m_dispQueue.size())
			{
				dispInfo = pThis->m_dispQueue.front();
				pThis->m_dispQueue.pop();
			}
			else
			{
				::ResetEvent(pThis->m_hDispEvent);
			}
			LeaveCriticalSection(&pThis->m_dispCS);
			PEVENTINFO info = new EVENTINFO;
			*info = dispInfo.eInfo;
			::CloseHandle(::CreateThread(NULL, 0, CEventTrigger::EventFireProc, info, 0, 0));
		}
		TRACE("CEventTrigger::DispatchProc Exit!\n");
		return 0;
	}

	DWORD WINAPI CEventTrigger::EventFireProc(LPVOID lpParam)
	{
		PEVENTINFO eInfo = (PEVENTINFO)lpParam;
		
		//fire event now
		if (eInfo->dwType == EVENTNOW)
		{
			eInfo->pIEvent->ReceiveEvent(eInfo->dwSign,eInfo->lpParam);
		}
		//delay fire event
		else if (eInfo->dwType == EVENTDELAY)
		{
			::Sleep(eInfo->dwMilliseconds);
			eInfo->pIEvent->ReceiveEvent(eInfo->dwSign,eInfo->lpParam);
		}
		//Loop and fire event interval time
		else if (eInfo->dwType == EVENTLOOP)
		{
			while (WaitForSingleObject(eInfo->hEvent, eInfo->dwMilliseconds) != WAIT_OBJECT_0)
			{
				//do fire
				eInfo->pIEvent->ReceiveEvent(eInfo->dwSign,eInfo->lpParam);
			}
		}
		TRACE("CEventTrigger::EventFireProc Exit - Sign - %d Type - %d!\n",eInfo->dwSign,eInfo->dwType);
		delete eInfo;
		return 0;
	}


	CEventTrigger* CEventTrigger::m_pInstance = NULL;
	long CEventTrigger::m_nRef = 0;

	CEventTrigger::CEventTrigger(void)
	{
		::InitializeCriticalSection(&m_dispCS);

		CString strUni;
		strUni.Format(_T("EventTrigger-DispEvent-%x"),this);
		//创建分发事件控制
		m_hDispEvent = ::CreateEvent(NULL, FALSE, TRUE, strUni.GetBuffer());
		::ResetEvent(m_hDispEvent); //清除信号
		//启动分发线程
		m_hDispHandle = ::CreateThread(NULL, 0, CEventTrigger::DispatchProc, this, 0, &m_dispThreadID);
	}


	CEventTrigger::~CEventTrigger(void)
	{
		::DeleteCriticalSection(&m_dispCS);
	}


	void CEventTrigger::ClearTrigger()
	{
		//退出定时线程
		HandleMap::iterator mapIter = m_triHandleMap.begin();
		while(mapIter != m_triHandleMap.end())
		{
			if(mapIter->second)
			{
				::SetEvent(mapIter->second);
				::CloseHandle(mapIter->second);
			}
			mapIter++;
		}

		//退出分发线程
		m_dispThreadID = 0;
		::SetEvent(m_hDispEvent);
		::WaitForSingleObject(m_hDispHandle, INFINITE);
		::CloseHandle(m_hDispHandle);
		::CloseHandle(m_hDispEvent);

		TRACE("CEventTrigger::ClearTrigger\n");
	}


	bool CEventTrigger::SignInRecevier(IERecevier* pRecevier)
	{
		if(pRecevier == NULL)
		{
			return false;
		}

		TriggerMap::iterator mapIter;
		mapIter = m_triEventMap.find((DWORD_PTR)pRecevier);
		if(mapIter != m_triEventMap.end())
		{
			return true;
		}

		m_triEventMap[(DWORD_PTR)pRecevier] = pRecevier;
		return true;
	}


	bool CEventTrigger::LogOutRecevier(IERecevier* pRecevier)
	{
		if(pRecevier == NULL)
		{
			return false;
		}

		TriggerMap::iterator mapIter;
		mapIter = m_triEventMap.find((DWORD_PTR)pRecevier);
		if(mapIter != m_triEventMap.end())
		{
			m_triEventMap.erase(mapIter);
		}
		return true;
	}


	DWORD_PTR CEventTrigger::SetTrigger(DWORD_PTR dwSign, DWORD dwEventType, DWORD_PTR dwRecevierId,
		LPVOID lpParam/* = 0*/, DWORD dwTime/* = 0*/)
	{
		DispInfo dispInfo;
		TriggerMap::iterator mapIter;
		mapIter = m_triEventMap.find(dwRecevierId);
		if(mapIter == m_triEventMap.end())
		{
			return 0;
		}
		HandleMap::iterator mapIter2;
		mapIter2 = m_triHandleMap.find(dwSign);
		if(mapIter2 != m_triHandleMap.end())
		{
			return -1;
		}
		dispInfo.eInfo.dwSign = dwSign;
		dispInfo.eInfo.dwType = dwEventType;
		dispInfo.eInfo.pIEvent = mapIter->second;
		dispInfo.eInfo.hEvent = NULL;
		dispInfo.eInfo.dwMilliseconds = dwTime;
		dispInfo.eInfo.lpParam = lpParam;
		if(dwEventType == EVENTNOW)
		{

		}
		else if(dwEventType == EVENTDELAY)
		{

		}
		else if(dwEventType == EVENTLOOP)
		{
			//创建分发事件控制
			HANDLE hEvent = ::CreateEvent(NULL, TRUE, TRUE, NULL);
			if(hEvent == NULL)
				return 0;
			::ResetEvent(hEvent); //清除信号
			dispInfo.eInfo.hEvent = hEvent;
		}
		m_triHandleMap[dwSign] = dispInfo.eInfo.hEvent;
		EnterCriticalSection(&m_dispCS);
		m_dispQueue.push(dispInfo);
		LeaveCriticalSection(&m_dispCS);
		::SetEvent(m_hDispEvent); //启动disp
		return dwSign;
	}


	BOOL CEventTrigger::BreakTrigger(DWORD_PTR dwSign)
	{
		HandleMap::iterator mapIter;
		mapIter = m_triHandleMap.find(dwSign);
		if(mapIter != m_triHandleMap.end())
		{
			if(mapIter->second)
			{
				::SetEvent(mapIter->second);
				::CloseHandle(mapIter->second);
			}
			m_triHandleMap.erase(mapIter);
		}

		return TRUE;
	}
}

