#ifndef __THREAD_MESSAGE_WND_H__
#define __THREAD_MESSAGE_WND_H__

#include <functional>
namespace ThreadMsgWnd
{

	class CAutoEvent
	{
	public:
		CAutoEvent(const HANDLE& hEvent):m_hEvent(hEvent){}
		~CAutoEvent(){::SetEvent(m_hEvent);}
		const HANDLE& m_hEvent;
	};

	class CThreadWindow
	{
	public:
		CThreadWindow();
		~CThreadWindow();
		void Start();
		void Stop();

		HWND GetWindowHandle() {return m_hThreadWnd;}

		LRESULT SendWndMessage(UINT uMsg,WPARAM wParam,LPARAM lParam)
		{
			if(!m_bStarted) {
				return 0;
			}
			::PostMessage(m_hThreadWnd,uMsg,(WPARAM)wParam,(LPARAM)lParam);
			::WaitForSingleObject(hWaitEvent,INFINITE);
			return m_lastResult;
		}

		BOOL PostWndMessage(UINT uMsg,WPARAM wParam,LPARAM lParam)
		{
			if(!m_bStarted) {
				return 0;
			}
			return ::PostMessage(m_hThreadWnd,uMsg,(WPARAM)wParam,(LPARAM)lParam);
		}

		static unsigned long WINAPI Work(LPVOID params)
		{
			::CoInitialize(NULL);
			CThreadWindow* p  = static_cast<CThreadWindow*>(params);
			if(p)
			{
				p->DoWork();
				::CloseHandle(p->m_hThread);
				if(p->m_bAutoRelease)
					delete p;
			}
			::CoUninitialize();
			return 0;
		}
		static void PostWork(CThreadWindow* p, bool wait)
		{
			p->m_hThread = ::CreateThread(0, 0, Work, p, 0, &p->m_idThread);
			if(wait)
			{
				::WaitForSingleObject(p->hPostEvent,INFINITE);
			}
		}
		template <typename O, typename OFnType>
		void BindWindowProc(O* pthis, OFnType pFn)
		{
			m_bindProc =  std::bind(pFn, pthis, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		}

	protected:
		virtual bool DoWork();
		static LRESULT WINAPI WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

		BOOL m_bStarted;
		BOOL m_bAutoRelease;
		HWND m_hThreadWnd;
		HANDLE m_hThread;
		HANDLE hPostEvent;
		HANDLE hWaitEvent;
		LRESULT m_lastResult;
		unsigned long m_idThread;
		std::function<LRESULT(HWND ,UINT ,WPARAM ,LPARAM )> m_bindProc;
	};

	CThreadWindow::CThreadWindow()
		:m_bStarted(0),m_bAutoRelease(0),m_hThreadWnd(0),m_hThread(0),m_idThread(0)
	{
		hWaitEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);
		::ResetEvent(hWaitEvent);
		hPostEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);
		::ResetEvent(hPostEvent);
	}

	CThreadWindow::~CThreadWindow()
	{
		::CloseHandle(hWaitEvent);
		::CloseHandle(hPostEvent);
	}

	void CThreadWindow::Start()
	{
		PostWork(this,true);
	} 

	void CThreadWindow::Stop()
	{
		::PostMessage(m_hThreadWnd,WM_CLOSE,0,0);
		::WaitForSingleObject(m_hThread,INFINITE);
	}

	LRESULT WINAPI CThreadWindow::WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		CThreadWindow* pThis = NULL;
		 pThis = reinterpret_cast<CThreadWindow*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		HANDLE hEvent = NULL;
		 CAutoEvent event_(pThis ? pThis->hWaitEvent : hEvent);
		switch(uMsg)
		{
		case WM_DESTROY:
			{
				if(pThis){
					 ::SetWindowLongPtr(pThis->m_hThreadWnd, GWLP_USERDATA, 0L);
				}
				::PostThreadMessage(::GetCurrentThreadId(),WM_QUIT,0,0);
			}
		case WM_CREATE:
			{
				LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
				pThis = static_cast<CThreadWindow*>(lpcs->lpCreateParams);
				::SetWindowLongPtr(pThis->m_hThreadWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
			}
		}
		if(pThis)
		{
			pThis->m_lastResult = pThis->m_bindProc(hWnd,uMsg,wParam,lParam);
		}
		return DefWindowProc(hWnd,uMsg,wParam,lParam);
	}

	bool CThreadWindow::DoWork()
	{
		{
			CAutoEvent ev(hPostEvent);
			TCHAR szClassName[] = _T("ThreadWindowT");
			static WNDCLASSEX wndClass;
			wndClass.cbSize = sizeof(wndClass);
			wndClass.style = CS_HREDRAW | CS_VREDRAW;
			wndClass.lpfnWndProc = WindowProc;
			wndClass.cbClsExtra = 0;
			wndClass.cbWndExtra = 0;
			wndClass.hInstance = GetModuleHandle(0);
			wndClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
			wndClass.hCursor = LoadCursor(NULL,IDC_ARROW);
			wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
			wndClass.lpszMenuName = NULL;
			wndClass.lpszClassName = szClassName;
			wndClass.hIconSm = NULL;


			ATOM atom = RegisterClassEx(&wndClass);
			if(0 == atom)
			{
				ATLTRACE("ThreadWindow<T> RegisterClassEx failed - %d\n",GetLastError());
				if(GetLastError() != 1410)
				{
					return false;
				}
			}
			m_hThreadWnd = CreateWindowEx(0,szClassName,_T(""),WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,
				CW_USEDEFAULT,CW_USEDEFAULT,HWND_MESSAGE,NULL,NULL,this);
			if(m_hThreadWnd == NULL)
			{
				ATLTRACE("ThreadWindow<T> CreateWindowEx failed - %d\n",GetLastError());
				return false;
			}
			m_bStarted = 1;
			ATLTRACE("ThreadWindow<T>-%p Work() Started\n",this);
		}
		MSG msg;
		while(GetMessage(&msg,NULL,0,0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		ATLTRACE("ThreadWindow<T>-%p Shutdown!\n",this);
		m_bStarted = 0;
		return true;
	}

}

#endif //__THREAD_MESSAGE_WND_H__