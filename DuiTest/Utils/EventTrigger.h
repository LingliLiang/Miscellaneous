#pragma once
#include <map>
#include <queue>

namespace ETRIGGER
{
	class CEventTrigger;

	enum EventType
	{
		EVENTNOW = 0x0001,
		EVENTDELAY = 0x0002,
		EVENTLOOP = 0x0004
	};

	interface IERecevier
	{
	public:
		virtual void ReceiveEvent(DWORD_PTR dwSign, LPVOID lpParam) = 0;
		virtual DWORD_PTR ReceiveId() = 0;
	};

	typedef struct EventInfo
	{
		DWORD dwSign;
		DWORD dwType;
		IERecevier* pIEvent;
		LPVOID lpParam;
		DWORD dwMilliseconds;
		HANDLE hEvent;
	}EVENTINFO,*PEVENTINFO;

	typedef struct DispInfo
	{
		EventInfo eInfo;
	}DISPINFO,*PDISPINFO;

	class CEventTrigger
	{
		static DWORD WINAPI DispatchProc(LPVOID lpParam);
		static DWORD WINAPI EventFireProc(LPVOID lpParam);
	private:
		CEventTrigger(void);
		void ClearTrigger();
	public:
		~CEventTrigger(void);

		bool SignInRecevier(IERecevier* pRecevier);
		bool LogOutRecevier(IERecevier* pRecevier);

		DWORD_PTR SetTrigger(DWORD_PTR dwSign, DWORD dwEventType, DWORD_PTR dwRecevierId, LPVOID lpParam = 0, DWORD dwTime = 0);
		BOOL BreakTrigger(DWORD_PTR dwSign);

	private:
		typedef std::map<DWORD_PTR,IERecevier*> TriggerMap;
		TriggerMap m_triEventMap;
		typedef std::map<DWORD_PTR,HANDLE> HandleMap;
		HandleMap m_triHandleMap;
		typedef std::queue<DISPINFO> DispQueue;
		DispQueue m_dispQueue;

		
		DWORD m_dispThreadID;
		DWORD m_dwThreadID2;

		HANDLE m_hDispHandle;
		HANDLE m_hDispEvent;

		CRITICAL_SECTION m_dispCS;

		///*** 单例模式控制***///
	public:
		static BOOL ExistsInstance()
		{
			return m_nRef;
		}

		static CEventTrigger * GetInstance()
		{
			if(m_pInstance == NULL)
			{
				m_nRef = 0;
				m_pInstance = new CEventTrigger();
			}
			::InterlockedIncrement(&m_nRef);
			return m_pInstance;
		}

		static long Release()
		{
			if(m_nRef == 0)
				return 0;
			if (::InterlockedDecrement(&m_nRef) != 0)
				return m_nRef;
			m_pInstance->ClearTrigger();
			delete m_pInstance;
			m_pInstance = NULL;
			return 0;
		}
	private:
		static CEventTrigger *m_pInstance;
		static long m_nRef;
	};

	template <class T>
	class TTriggerPtr
	{
	public:
		TTriggerPtr()
		{
			p = T::GetInstance();
		}
		~TTriggerPtr()
		{
			if (p)
				assert(p->Release()!=0);
		}
		T* operator->() const
		{
			return p;
		}
		void Release()
		{
			T* pTemp = p;
			if (pTemp)
			{
				p = NULL;
				pTemp->Release();
			}
		}
		T* p;
	};
}

typedef ETRIGGER::TTriggerPtr<ETRIGGER::CEventTrigger> TriggerPtr;