#pragma once
#ifndef __NS_FLOATWINDOW_H__
#define __NS_FLOATWINDOW_H__



template<class TLayout , class OSrcThis>
class CFloatWindowUI :  protected CWindowUI
{
#define ADD_HEIGHT 60
	typedef DWORD (OSrcThis::*iProc)(_In_ CPaintManagerUI*); //init proc
	typedef DWORD (OSrcThis::*cProc)(_In_ CPaintManagerUI*); //last clear up proc
public:
	CFloatWindowUI(void);
	~CFloatWindowUI(void);
	HWND GetHWND() const{return __super::GetHWND();}
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	//WindowUI
	virtual CUIString GetSkinFolder(){
		return _T("");
	}
	virtual CUIString GetSkinFile(){return _T("common\\floatwin.xml");}
	virtual LPCTSTR GetWindowClassName(void) const {return _T("FloatWinUI") ;}
	virtual void InitWindow();
	virtual void Notify(TNotifyUI& msg);
	virtual void OnFinalMessage(HWND hWnd){delete this;}
	virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
public:
	//FloatUI
	void SetLayout(TLayout* pLayout);
	void SetPaintManager(CPaintManagerUI* pPaintManager)
	{
		m_pParentPM = pPaintManager;
	}

	void ShowFloatWindow();

	void SetInitProc(iProc proc){if(proc) m_iProc = proc;}
	void SetCallBackProc(cProc proc){if(proc) m_cProc = proc;}
	void SetAttachWnd(OSrcThis* pThis){if(pThis) m_pParentThis = pThis;}
private:
	bool m_bFullScreen;
	bool m_bClosed;
	UINT m_nLayoutIndex;
	TLayout* m_orgLayout;
	UINT m_nOrgLayoutHeight;
	CContainerUI* m_parentLayout;
	CVerticalLayoutUI* m_vLayout;
	CPaintManagerUI* m_pParentPM;

	OSrcThis* m_pParentThis;
	iProc m_iProc;
	cProc m_cProc;
};

template<class TLayout , class OSrcThis>
CFloatWindowUI<TLayout , OSrcThis>::CFloatWindowUI(void)
	:m_bFullScreen(false),
	m_nLayoutIndex(0),
	m_orgLayout(NULL),
	m_parentLayout(NULL),
	m_vLayout(NULL),
	m_pParentPM(NULL),
	m_pParentThis(NULL)
{
	m_nOrgLayoutHeight = 0;
	m_iProc = NULL;
	m_cProc = NULL;
	m_bClosed = false;
}

template<class TLayout , class OSrcThis>
CFloatWindowUI<TLayout , OSrcThis>::~CFloatWindowUI(void)
{
}

template<class TLayout , class OSrcThis>
void CFloatWindowUI<TLayout , OSrcThis>::InitWindow()
{
	m_vLayout = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("vl_ctl")));
	//call first
	if(m_pParentThis != NULL && m_iProc != NULL)
	{
		(m_pParentThis->*m_iProc)(&m_PaintManager);
	}
}

template<class TLayout , class OSrcThis>
LRESULT CFloatWindowUI<TLayout , OSrcThis>::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_CLOSE)
	{
		if(!m_bClosed)
		{
			m_bClosed = true;
			//call back
			if(m_pParentThis != NULL && m_cProc != NULL)
			{
				(m_pParentThis->*m_cProc)(&m_PaintManager);
			}
			m_vLayout->RemoveNotDestroy(m_orgLayout);
			m_orgLayout->SetFixedHeight(m_nOrgLayoutHeight);
			m_parentLayout->AddAt(m_orgLayout,m_nLayoutIndex);
		}
	}
	return __super::HandleMessage(uMsg,wParam,lParam);
}

template<class TLayout , class OSrcThis>
void CFloatWindowUI<TLayout , OSrcThis>::Notify(TNotifyUI& msg)
{
//#ifdef _DEBUG
//	OutputDebugString(msg.sType);
//	OutputDebugString(_T("\n"));
//#endif
	if(_tcscmp(msg.sType ,_T("click")) == 0)
	{
		if(_tcscmp(msg.pSender->GetName() ,_T("close")) == 0)
		{
			Close();
		}
		//if(_tcscmp(msg.pSender->GetName() ,_T("max")) == 0)
		//{
		//	if(!::IsZoomed(m_hWnd))
		//	{
		//		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE,0);
		//	}
		//	else
		//	{
		//		SendMessage(WM_SYSCOMMAND, SC_RESTORE,0);
		//	}
		//}
	}
	if(_tcscmp(msg.sType ,DUI_MSGTYPE_SELECTCHANGED) == 0)
	{
		if(_tcscmp(msg.pSender->GetName() ,_T("pin")) == 0)
		{
			if(((COptionUI*)msg.pSender)->IsSelected())
			{
				::SetWindowPos(m_hWnd,HWND_TOPMOST, 0, 0, 0, 0,SWP_NOSIZE | SWP_NOMOVE);
			}
			else
			{
				::SetWindowPos(m_hWnd,HWND_NOTOPMOST,0, 0, 0, 0,SWP_NOSIZE | SWP_NOMOVE);
			}
		}
		if(_tcscmp(msg.pSender->GetName() ,_T("fullscr")) == 0)
		{
			static CRect winRect;
			if(m_bFullScreen = !m_bFullScreen) //赋值再判段
			{
				int   nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
				int   nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
				m_PaintManager.SetCaptionRect(CRect(0,0,0,0));
				m_PaintManager.SetSizeBox(CRect(0,0,0,0));
				::GetWindowRect(m_hWnd,&winRect);
				::SetWindowPos(m_hWnd,HWND_TOPMOST, 0, 0, nScreenWidth, nScreenHeight,0);
			}
			else
			{
				m_PaintManager.SetCaptionRect(CRect(0,0,0,40));
				m_PaintManager.SetSizeBox(CRect(4,4,4,4));
				::SetWindowPos(m_hWnd,HWND_NOTOPMOST, winRect.left, winRect.top, winRect.GetWidth(), winRect.GetHeight(),0);
			}
		}
	}
	m_pParentPM->SendNotify(msg);
	__super::Notify(msg);
}

template<class TLayout , class OSrcThis>
LRESULT CFloatWindowUI<TLayout , OSrcThis>::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;

	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
	CRect rcWork = oMonitor.rcWork;
	CRect rcMonitor = oMonitor.rcMonitor;
	rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

	// 计算最大化时，正确的原点坐标
	//Work screen
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;
	
	if(m_bFullScreen)
	{
		//full screen
		lpMMI->ptMaxSize.x = rcMonitor.GetWidth();
		lpMMI->ptMaxSize.y = rcMonitor.GetHeight();
		lpMMI->ptMaxTrackSize.x = rcMonitor.GetWidth();
		lpMMI->ptMaxTrackSize.y = rcMonitor.GetHeight();
	}
	else
	{
		lpMMI->ptMaxSize.x = rcWork.GetWidth();
		lpMMI->ptMaxSize.y = rcWork.GetHeight();
		lpMMI->ptMaxTrackSize.x = rcWork.GetWidth();
		lpMMI->ptMaxTrackSize.y = rcWork.GetHeight();
	}
	
	lpMMI->ptMinTrackSize.x = m_PaintManager.GetMinInfo().cx;
	lpMMI->ptMinTrackSize.y = m_PaintManager.GetMinInfo().cy;

	bHandled = FALSE;
	return 0;
}

template<class TLayout , class OSrcThis>
LRESULT CFloatWindowUI<TLayout , OSrcThis>::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int nWidth = LOWORD(lParam); // width of client area
	int nHeight = HIWORD(lParam); // height of client area
	if(m_orgLayout)
		m_orgLayout->SetFixedHeight(nHeight - ADD_HEIGHT);
	return S_OK;
}

////////////////
template<class TLayout , class OSrcThis>
void CFloatWindowUI<TLayout , OSrcThis>::SetLayout(TLayout* pLayout)
{
	m_orgLayout = pLayout;
	//获取Layout的父Layout
	m_parentLayout = ((CContainerUI*)m_orgLayout->GetParent());
	//获取Layout的位置
	m_nLayoutIndex = m_parentLayout->GetItemIndex(m_orgLayout);
	//从源界面移除,并不销毁layout
	m_parentLayout->RemoveNotDestroy(m_orgLayout);
}

template<class TLayout , class OSrcThis>
void CFloatWindowUI<TLayout , OSrcThis>::ShowFloatWindow()
{
	int height = m_orgLayout->GetFixedHeight() + ADD_HEIGHT;
	int width = m_orgLayout->GetFixedWidth();
	m_nOrgLayoutHeight = height - ADD_HEIGHT;
	if(width == 0) width =1280;
	if(height == 0) height =720;
	this->Create(m_pParentThis->GetHWND(), _T("FloatWinUI"), UI_WNDSTYLE_FRAME, UI_WNDSTYLE_EX_DIALOG, 0, 0, width, height,0);
	m_vLayout->AddAt(m_orgLayout,2);
	m_PaintManager.NeedUpdate();
	this->CenterWindow();
	this->ShowWindow();
}

template <class T , class O>
HWND FloatWindow(T* pLayoutSrc, CPaintManagerUI* pSrcPm, O* pSrcThis =NULL , DWORD (O::*CallFirstProc)(_In_ CPaintManagerUI*) = NULL ,DWORD (O::*CallBackProc)(_In_ CPaintManagerUI*) = NULL)
{
	CFloatWindowUI<T,O> *pFloatw = new CFloatWindowUI<T,O>;

	//set up
	pFloatw->SetLayout(pLayoutSrc);
	pFloatw->SetPaintManager(pSrcPm);
	pFloatw->SetInitProc(CallFirstProc);
	pFloatw->SetCallBackProc(CallBackProc);
	pFloatw->SetAttachWnd(pSrcThis);

	pFloatw->ShowFloatWindow();
	return pFloatw->GetHWND();
}


#endif  //__NS_FLOATWINDOW_H__
