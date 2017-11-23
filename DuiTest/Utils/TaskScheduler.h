#pragma once
#include "EventTrigger.h"
#include <sys\timeb.h>
#include <string>
//#include <mutex>

/**
TimerTask 计划任务控制,将任务在程序实例内定时化,周期化执行.
**/
namespace TimerTask
{
	class CTaskScheduler;
	typedef ULONG TaskToken ;

	struct _tagTimeStamp //millisecond
	{

		ULONG64 start_time;
		ULONG64 end_time;
		ULONG period_time;
	};

	struct _tagTaskDetail
	{
		struct _tagTimeStamp time_stamp;
		void* description;
		_tagTaskDetail():description(0)
		{
			time_stamp.start_time = 0;
			time_stamp.end_time = 0;
			time_stamp.period_time = 0;
		}

	};

	inline int EndTime(struct _tagTimeStamp& stamp, ULONG64 now);
	inline int StartTime(struct _tagTimeStamp& stamp, ULONG64 now);

	//sql datetime string to sec since 1970-01-01 00:00:01.
	ULONG DatetimeStr2Sec_CMOS(CString sqlDate);
	ULONG64 DatetimeStr2MSec(CString sqlDate);
	CString DatetimeStrNow();
	std::string DatetimeCStrNow();
	void CorrectSystemTime(CString sqlDate);
	/**
	** task timestamp defines
	*start is start-time
	0 - immediately; if you want dely start task, set start = dely-sec + now systime;
	? - when time hit start call dotask() even passed time;
	*end is end-time
	0 - no end, dotask() done,will call FreeDetail() immediately;
	? - when time hit end call FreeDetail();
	*period is loop falg
	0 - no loop; ? -loop ms

	** examples
	** xxx = systime ; 0 = not provide
	** start 0 end 0 period 0 - start immediately,run once
	** start xxx end 0 period 0 - starting after hit xxx (systime in ms) ,delay once
	** start xxx end 0 period ? - starting after hit xxx (systime in ms) ,no end, loop ? ms
	** start xxx end xxx period 0 - starting after hit xxx (systime in ms) ,end in time xxx, no loop
	**/
	class ITask
	{
		friend class CTaskScheduler;
	public:
		ITask():m_token(0),b_run(0),b_onTask(0),b_timer(0){}
		virtual void DoTask(void* description) = 0; //exec task
		virtual void FreeDetail() = 0; //auto clear task info when time ended
		_tagTaskDetail t_detail;
		TaskToken m_token;
		std::string t_name;
	private:
		int b_run;
		int b_onTask;
		int b_timer;
	};


	class CTaskScheduler 
		: public ETRIGGER::IERecevier
	{
	public:
		typedef std::map<ULONG, ITask*>::iterator TasksIter;

		CTaskScheduler(void);
		~CTaskScheduler(void);

		//option: use timer instand of thread's Event
		void SetMainHanlde(HWND hwnd);
		BOOL OnTimer(UINT_PTR nIDEvent); //must call this in messageporc

		virtual DWORD_PTR ReceiveId(); //allways return this's hex

		//ITask must new instance, and auto release by TaskManager
		TaskToken Schedule(ITask* pTask, BOOL bNotAutoFree = FALSE, BOOL btimer = FALSE);
		int Drop(TaskToken token);
		int Override(TaskToken token, ITask* pTask);

		
	private:
		inline void Drop(TasksIter &iter);

		//threads Receive tasks
		virtual void ReceiveEvent(DWORD_PTR dwSign, LPVOID lpParam);
		
		//setup task
		int TaskInit(TaskToken token, ITask &itask);
		//hit test task end,for timer
		int TaskHitEnd(ITask *pTask);
		int TaskHitStart(ITask *pTask);
		void DoDelayFree();

		void DoUpateTick();

		void Clear(ITask *pTask);
		void ClearTasks();
	private:
		HWND m_hMainFrame;//handle of window
		std::map<ULONG,ITask*> m_mapTasks;
		std::pair<ULONG, ULONG64> m_Tick; //time tick thread
		std::vector<ITask*> m_vecWhiteList; //no free by CTaskScheduler list
		std::vector<std::pair<ITask*,HANDLE*>> m_vecDelayFreeList; //Delay free list
		ULONG m_inc;
		CRITICAL_SECTION m_taskCS;
	};

}// namespace TimerTask
