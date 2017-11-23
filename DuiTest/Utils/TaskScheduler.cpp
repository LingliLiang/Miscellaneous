#include "StdAfx.h"
#include "TaskScheduler.h"
#include "Timer.h"
#include <time.h>
#include <ATLComTime.h>

namespace TimerTask
{

	CTaskScheduler::CTaskScheduler(void)
	{
		::InitializeCriticalSection(&m_taskCS);
		ETRIGGER::CEventTrigger* p = ETRIGGER::CEventTrigger::GetInstance();
		p->SignInRecevier(this);
		
		m_hMainFrame = NULL;
		m_Tick.first = 0x00c0;
		m_Tick.second = 0;
		m_inc = 0;
		m_vecWhiteList.clear();
		//setup update tick thread
		p->SetTrigger(m_Tick.first, ETRIGGER::EVENTLOOP, (DWORD_PTR)this, 0, 300); //300ms
	}

	CTaskScheduler::~CTaskScheduler(void)
	{
		ClearTasks();
		ETRIGGER::CEventTrigger::Release();
		::DeleteCriticalSection(&m_taskCS);
	}

	TaskToken CTaskScheduler::Schedule(ITask* pTask, BOOL bNotAutoFree, BOOL btimer)
	{
		BOOL success = FALSE;
		pTask->b_timer = btimer;
		::InterlockedIncrement(&m_inc);
		if(bNotAutoFree) m_vecWhiteList.push_back(pTask);
		success = TaskInit(m_inc,*pTask);
		return success;
	}

	int CTaskScheduler::Drop(TaskToken token)
	{
		//check can find task
		BOOL find = FALSE;
		EnterCriticalSection(&m_taskCS);
		TasksIter taskiter = m_mapTasks.find(token);
		if(taskiter != m_mapTasks.end())
		{
			find = TRUE;
			Drop(taskiter);
		}
		LeaveCriticalSection(&m_taskCS);
		return find;
	}

	void CTaskScheduler::Drop(TasksIter &iter)
	{
		Clear(iter->second);
		iter = m_mapTasks.erase(iter);
	}

	int CTaskScheduler::Override(TaskToken token, ITask* pTask)
	{
		return 0;
	}

	int CTaskScheduler::TaskInit(TaskToken token, ITask &itask)
	{
		token += m_Tick.first;
		itask.m_token = token;
		EnterCriticalSection(&m_taskCS);
		m_mapTasks[token] = &itask;
		TaskHitStart(&itask);
		LeaveCriticalSection(&m_taskCS);
		return itask.m_token;
	}

	int CTaskScheduler::TaskHitStart(ITask *pTask)
	{
		int ret = 0;
		auto Hit = [&]()->void
		{
			pTask->b_run = 1;
			if (pTask->b_timer) {
				::SetTimer(m_hMainFrame, pTask->m_token, pTask->t_detail.time_stamp.period_time, 0);
			}
			else
			{
				TriggerPtr pTrigger;
				if (pTask->t_detail.time_stamp.period_time > 0)
				{
					pTask->t_detail.time_stamp.period_time < 10 ? pTask->t_detail.time_stamp.period_time = 10 : 0;
					pTrigger->SetTrigger(pTask->m_token, ETRIGGER::EVENTLOOP, (DWORD_PTR)this, 0, pTask->t_detail.time_stamp.period_time);
				}
				else
				{
					pTask->b_onTask = 1; //不是周期执行的,任务任务马上开始
					pTrigger->SetTrigger(pTask->m_token, ETRIGGER::EVENTNOW, (DWORD_PTR)this);
				}
			}
			
		};

		if (pTask != NULL && StartTime(pTask->t_detail.time_stamp, m_Tick.second)) {
			Hit();
			ret = 1;
			return ret;
		}
		TasksIter iter = m_mapTasks.begin();
		while (iter != m_mapTasks.end())
		{
			pTask = iter->second;
			if (!pTask->b_run && StartTime(pTask->t_detail.time_stamp, m_Tick.second))
			{
				Hit();
				ret = 1;
			}
			iter++;
		}
		return ret;
	}

	void CTaskScheduler::DoDelayFree()
	{
		std::vector<std::pair<ITask*, HANDLE*>>::iterator iter = m_vecDelayFreeList.begin();
		while (iter != m_vecDelayFreeList.end()) {
			if (*(iter->second) == INVALID_HANDLE_VALUE)
			{
				delete iter->first;
				iter = m_vecDelayFreeList.erase(iter);
			}
			else {
				iter++;
			}
		}
	}

	int CTaskScheduler::TaskHitEnd(ITask *pTask)
	{
		int ret = 0;
		TasksIter iter = m_mapTasks.begin();
		while (iter != m_mapTasks.end())
		{
			pTask = iter->second;
			if (pTask->b_run && !pTask->b_onTask && EndTime(pTask->t_detail.time_stamp, m_Tick.second))
			{
				Drop(iter);
				ret = 1;
			}
			else
			{
				iter++;
			}
		}
		return ret;
	}

	void CTaskScheduler::DoUpateTick()
	{
		EnterCriticalSection(&m_taskCS);
		struct timeb ti;
		ftime(&ti);
		m_Tick.second = (ULONG64)ti.time*1000 + ti.millitm;
		//Timing::Timer clock;
		//double d = clock.AbsoluteTime();
		TaskHitStart(NULL);
		//d = clock.AbsoluteTime() - d;
		//ATLTRACE("%lf\n",d);
		TaskHitEnd(NULL);
		DoDelayFree();
		LeaveCriticalSection(&m_taskCS);
	}

	void CTaskScheduler::Clear(ITask * pTask)
	{
		HANDLE* pEvent = NULL;
		if (pTask->b_timer) {
			::KillTimer(m_hMainFrame, pTask->m_token);
		}
		else {
			TriggerPtr pTrigger;
			pTrigger->BreakTrigger(pTask->m_token, &pEvent);
		}
		pTask->FreeDetail();
		std::vector<ITask*>::iterator wlIter = std::find(m_vecWhiteList.begin(),m_vecWhiteList.end(),pTask);
		if (wlIter == m_vecWhiteList.end())
		{
			if (pEvent != NULL && *pEvent != INVALID_HANDLE_VALUE)
				m_vecDelayFreeList.push_back(make_pair(pTask, pEvent));//没有在白名单,未结束任务延迟删除
			else
				delete pTask; //没有在白名单,直接删除
		}
		else
		{
			pTask->m_token = 0;
			pTask->b_run = 0;
			pTask->t_detail.time_stamp.end_time = 0;
			pTask->t_detail.time_stamp.period_time = 0;
			pTask->t_detail.time_stamp.start_time = 0;
			m_vecWhiteList.erase(wlIter);
		}
		pTask = NULL;
	}

	void CTaskScheduler::ClearTasks()
	{
		TriggerPtr pTrigger;
		EnterCriticalSection(&m_taskCS);
		TasksIter iter = m_mapTasks.begin();
		while (iter != m_mapTasks.end())
		{
			Clear(iter->second);
			iter = m_mapTasks.erase(iter);
			if(iter != m_mapTasks.end())
				iter++;
		}
		LeaveCriticalSection(&m_taskCS);
	}

	void CTaskScheduler::SetMainHanlde(HWND hwnd)
	{
		if(::IsWindow(hwnd))
			m_hMainFrame = hwnd;
	}
	
	BOOL CTaskScheduler::OnTimer(UINT_PTR nIDEvent)
	{
		BOOL done = FALSE;
		//EnterCriticalSection(&m_taskCS); //不需要在这里锁定防止死锁
		TasksIter iter = m_mapTasks.find(nIDEvent);
		if (iter != m_mapTasks.end())
		{
			ITask* pTask = iter->second;
			pTask->b_onTask = 1;
			pTask->DoTask(pTask->t_detail.description);
			done = TRUE;
			pTask->b_onTask = 0;
		}
		//LeaveCriticalSection(&m_taskCS);
		return done;
	}

	void CTaskScheduler::ReceiveEvent(DWORD_PTR dwSign, LPVOID lpParam)
	{
		if (m_Tick.first == dwSign)
		{
			DoUpateTick();
			return;
		}
		OnTimer(dwSign);
	}

	DWORD_PTR CTaskScheduler::ReceiveId()
	{
		return (DWORD_PTR)this;
	}

	int EndTime(_tagTimeStamp & stamp, ULONG64 now)
	{
		BOOL result = 0;
		if(stamp.period_time >=10 &&stamp.end_time == 0)
		{
			//loop, no stop
		}
		else if(stamp.end_time == 0)
		{
			//stop
			result = 1;
		}
		else if(stamp.end_time <= now)
		{
			//passed time, stop
			result = 1;
		}
		return result;
	}

	int StartTime(_tagTimeStamp & stamp, ULONG64 now)
	{
		BOOL result = 0;
		if(stamp.start_time == 0)
		{
			//immediately start
			result = 1;
		}
		else if(stamp.start_time <= now)
		{
			//passed start time, start
			result = 1;
		}
		return result;
	}

	static inline unsigned long CMOS_mktime (
		unsigned int year, unsigned int mon,
		unsigned int day, unsigned int hour,
		unsigned int min, unsigned int sec)
	{
		if (0 >= (int) (mon -= 2)) {    /* 1..12 -> 11,12,1..10 */
			mon += 12;      /* Puts Feb last since it has leap day */
			year -= 1;
		}

		return (((
			(unsigned long) (year/4 - year/100 + year/400 + 367*mon/12 + day) +
			year*365 - 719499
			)*24 + hour /* now have hours */
			)*60 + min /* now have minutes */
			)*60 + sec; /* finally seconds */
	}

	//datetime       8 bytes   YYYY-MM-DD HH:MM:SS   1000-01-01 00:00:00 ~ 9999-12-31 23:59:59 
	//timestamp      4 bytes   YYYY-MM-DD HH:MM:SS   1970-01-01 00:00:01 ~ 2038 
	ULONG DatetimeStr2Sec_CMOS(CString sqlDate)
	{
		struct tm target = {0};
		_stscanf_s(sqlDate.GetBuffer(),_T("%d-%d-%d %d:%d:%d"),&target.tm_year,&target.tm_mon,&target.tm_mday,
			&target.tm_hour,&target.tm_min,&target.tm_sec);
		return CMOS_mktime(target.tm_year,target.tm_mon,
			target.tm_mday,target.tm_hour,target.tm_min,target.tm_sec);
	}

	ULONG64 DatetimeStr2MSec(CString sqlDate)
	{
		if(sqlDate.IsEmpty()) return 0;
		struct tm target = {0};
		_stscanf_s(sqlDate.GetBuffer(),_T("%d-%d-%d %d:%d:%d"),&target.tm_year,&target.tm_mon,&target.tm_mday,
			&target.tm_hour,&target.tm_min,&target.tm_sec);
		target.tm_isdst = 0;
		target.tm_mon--;
		target.tm_wday = 0;
		target.tm_yday = 0;
		target.tm_year -= 1900;
		return ((ULONG64)mktime(&target))*1000;
	}

	CString DatetimeStrNow()
	{
		struct tm target = {0};
		time_t ti = time(0);
		localtime_s(&target,&ti);
		
		CHAR strDate[256]={0};
		strftime(strDate,sizeof(strDate),"%Y-%m-%d %H:%M:%S %A %Z",&target);
#ifdef UNICODE
		return (LPCWSTR)CA2W(strDate);
#else
		return strDate;
#endif
	}

	std::string DatetimeCStrNow()
	{
		struct tm target = {0};
		time_t ti = time(0);
		localtime_s(&target,&ti);
		
		CHAR strDate[256]={0};
		strftime(strDate,sizeof(strDate),"%Y-%m-%d %H:%M:%S %A %Z",&target);
		return strDate;
	}

	void CorrectSystemTime(CString sqlDate)
	{
		if(sqlDate.IsEmpty()) return;
		LONG64 local = ((LONG64)time(0))*1000;
		LONG64 remote = (LONG64)TimerTask::DatetimeStr2MSec(sqlDate.GetBuffer());
		if( abs( remote - local) < 4000) return;
		//时间间隔超过4秒
		COleDateTime dtm;
		dtm.ParseDateTime(sqlDate.GetBuffer());
		SYSTEMTIME systime = {0};
		dtm.GetAsSystemTime(systime);
		::SetLocalTime(&systime);
	}
}// namespace TimerTask