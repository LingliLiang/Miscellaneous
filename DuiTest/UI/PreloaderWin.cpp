#include "stdafx.h"
#include "PreloaderWin.h"

#define WIN_NAME  _T("PreloaderWin")
#define PRELOADER_UI_NAME  _T("PreloaderUI")
#define PRELOADER_UI_SKIN  _T("common\\perloader_ui.xml")
#define PRELOADER_UI_SKIN_FOLDER  _T("skin\\default")

#define PRELOADER_UI_TEXT  _T("text_loader")
#define PRELOADER_UI_ICON  _T("icon_loader")
#define PRELOADER_UI_CANCEL  _T("btn_cancel")

//////////////////////////////////////////////////////////////////////////
CPreloaderWin::CPreloaderWin(HWND hParent)
{
	m_hThread = NULL;
	RECT rc  = {0};
	HMONITOR hScreenMonitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	
	CRect rcWindow;
	if(hParent == NULL)
	{
		MONITORINFO mi = {0};
		mi.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hScreenMonitor, &mi);

		rcWindow = mi.rcWork;
	}
	else
	{
		GetWindowRect(hParent, &rcWindow);
	}



	Create(hParent, WIN_NAME, WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED, rcWindow);

	ASSERT(m_hWnd);

	SetLayeredWindowAttributes(m_hWnd, RGB(0,0,0), 180, LWA_ALPHA);
	m_pLoaderUI = new CPreloaderUI(m_hWnd);
}

LPCTSTR CPreloaderWin::GetWindowClassName() const
{
	return WIN_NAME;
}

LRESULT CPreloaderWin::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_ERASEBKGND)
	{
		HDC hdc = GetDC(m_hWnd);

		RECT rc ={0};
		GetClientRect(m_hWnd, &rc);

		FillRect(hdc, &rc, CreateSolidBrush(RGB(0,0,0)) );
		ReleaseDC(m_hWnd, hdc);
	}
	else if(uMsg == WM_CLOSE)
	{
		m_pLoaderUI->Close();
	}
	else if(uMsg == WM_USER+0x1)
	{
		if(m_hThread)
		{
			::TerminateThread(m_hThread,0);
			::CloseHandle(m_hThread);
		}
		Close();
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CPreloaderWin::SetLoaderText(LPCTSTR lpszText)
{
	if(m_pLoaderUI)
	{
		m_pLoaderUI->SetLoaderText(lpszText);
	}
}

//////////////////////////////////////////////////////////////////////////
CPreloaderUI::CPreloaderUI(HWND hParent)
	:m_hParentWnd(hParent)
{
	m_pLoaderText = NULL;
	m_pLoaderIcon = NULL;
	if (!::IsWindow(m_hParentWnd)) return;
	CRect rcWindow;
	GetWindowRect(m_hParentWnd, &rcWindow);
	HWND hWnd = ::CWindowUI::Create(m_hParentWnd, PRELOADER_UI_NAME, UI_WNDSTYLE_CHILD, UI_CLASSSTYLE_CONTAINER
		,0, 0, 0, 0);
	m_PaintManager.SetMinInfo(rcWindow.GetWidth(), rcWindow.GetHeight());
	if (::IsWindow(hWnd))
	{
		::SetWindowPos(hWnd, NULL, 0, 0, rcWindow.GetWidth(), rcWindow.GetHeight(), SWP_SHOWWINDOW);
	}
}

LPCTSTR CPreloaderUI::GetWindowClassName() const
{
	return PRELOADER_UI_NAME;
}

CUIString CPreloaderUI::GetSkinFile()
{
	return PRELOADER_UI_SKIN;
}

CUIString CPreloaderUI::GetSkinFolder()
{
	return PRELOADER_UI_SKIN_FOLDER;
}

void CPreloaderUI::InitWindow()
{
	m_pLoaderIcon = (CLabelEx*)m_PaintManager.FindControl(PRELOADER_UI_ICON);
	m_pLoaderText = (CTextUI*)m_PaintManager.FindControl(PRELOADER_UI_TEXT);
	::SetTimer(m_hWnd,1,30,0);
	//m_pLoaderIcon->Play(m_pLoaderIcon->GetBkImage());
}

void CPreloaderUI::OnFinalMessage(HWND hWnd)
{
	CWindowUI::OnFinalMessage(hWnd);
	delete this;
}

LRESULT CPreloaderUI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_CLOSE)
	{
		m_pLoaderIcon->Stop(m_pLoaderIcon->GetBkImage());
	}
	else if(uMsg == WM_TIMER)
	{
		if(wParam == 1)
		{
			::KillTimer(m_hWnd,1);
			m_pLoaderIcon->Play(m_pLoaderIcon->GetBkImage());
		}
	}
	return CWindowUI::HandleMessage(uMsg, wParam, lParam);
}

void CPreloaderUI::Notify(TNotifyUI& msg)
{
	if(msg.pSender->GetName().CompareNoCase(PRELOADER_UI_CANCEL) == 0 && msg.sType == DUI_MSGTYPE_CLICK)
	{
		::PostMessageA(m_hParentWnd,WM_USER+0x1,0,0);
	}
}

void CPreloaderUI::SetLoaderText(LPCTSTR lpszText)
{
	if(m_pLoaderText)
	{
		m_pLoaderText->SetText(lpszText);
	}
}