#include "StdAfx.h"
#include "WndEventImpl.h"

#define SKINFILE  _T("main_frame.xml");
#define SKINFOLDER _T("skin\\")

class CDuitestWnd : public CWindowUI
{

public:
	CDuitestWnd():m_pEvent(nullptr){}
	~CDuitestWnd(){}

	LPCTSTR GetWindowClassName() const;

	virtual CUIString GetSkinFile();
	virtual CUIString GetSkinFolder();

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void OnClick(TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void Notify(TNotifyUI& msg);

	std::unique_ptr<IWndEvent> m_pEvent;
};

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	HRESULT Hr = ::OleInitialize(NULL);
	if( FAILED(Hr) ) return 0;
	CPaintManagerUI::SetInstance(hInstance);
	TCHAR pFileName[MAX_PATH];   
	int nPos = GetCurrentDirectory( MAX_PATH, pFileName);
	::StrCat(pFileName,_T("\\"));
	::StrCat(pFileName,SKINFOLDER);
	CPaintManagerUI::SetResourcePath(pFileName);
	std::auto_ptr<CDuitestWnd> pWin(new CDuitestWnd);
	pWin->m_pEvent.reset(new CWndEventImpl);
	pWin->Create(NULL,_T("DuitestWnd"),UI_WNDSTYLE_FRAME,UI_WNDSTYLE_EX_FRAME);
	pWin->CenterWindow();
	pWin->ShowWindow();

	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::Term();

	::OleUninitialize();
	return 0;
}

LPCTSTR CDuitestWnd::GetWindowClassName() const
{
	return _T("DuitestWnd");
}

CUIString CDuitestWnd::GetSkinFile()
{
	return SKINFILE;
}

CUIString CDuitestWnd::GetSkinFolder()
{
	return _T("");
}

CControlUI* CDuitestWnd::CreateControl(LPCTSTR strControl)
{
	if(m_pEvent)
	{
		return m_pEvent->CreateControl(strControl);
	}
	return nullptr;
}

void CDuitestWnd::Notify(TNotifyUI& msg)
{
	if(m_pEvent)
	{
		if(m_pEvent->Notify(msg)) return;
	}
	__super::Notify(msg);
}

void CDuitestWnd::OnClick(TNotifyUI& msg)
{
	if(m_pEvent)
	{
		if(m_pEvent->OnClick(msg)) return;
	}
	CUIString sCtrlName = msg.pSender->GetName();
	if (sCtrlName == _T("closebtn") )
	{
		Close();
		::PostQuitMessage(0);
		return; 
	}
	else if(sCtrlName == _T("minbtn"))
	{ 
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); 
		return; 
	}
	else if (sCtrlName == _T("maxbtn"))
	{ 
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); 
		return; 
	}
	else if (sCtrlName == _T("restorebtn"))
	{ 
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); 
		return; 
	}
}

LRESULT CDuitestWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_pEvent){
		BOOL bHandled = FALSE;
		auto lRes = m_pEvent->HandleMessage(uMsg, wParam, lParam, bHandled);
		if (bHandled) return lRes;
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}