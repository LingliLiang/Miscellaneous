#include "WndEventImpl.h"
#include "ui\LayerLayoutUI.h"
#include "ui\UIAnalogStick.h"
#include "UI\BricksTabUI.h"
#include "UI\UISpinButton.h"
#include "UI\UILayerControl.h"

CWndEventImpl::CWndEventImpl(HWND& hWnd)
	:m_hWnd(hWnd)
{

}

CWndEventImpl::~CWndEventImpl()
{
}


CControlUI* CWndEventImpl::CreateControl(LPCTSTR strControl)
{
	CControlUI* p = 0;
	if (p = CLayerLayoutUI::CreateControl(strControl))
	{
	}
	else if (_tcsicmp(strControl, DUI_CTR_ANALOGSTICK) == 0)
	{
		p = new CAnalogStickUI;
	}
	else if (_tcsicmp(strControl, DUI_CTR_BRICKSTAB) == 0)
	{
		p = new CBricksTabUI;
	}
	else if (_tcsicmp(strControl, DUI_CTR_SPINBUTTON) == 0)
	{
		p = new CSpinButtonUI;
	}
	else if (_tcsicmp(strControl, _T("LayerControl")) == 0)
	{
		p = new CLayerControlUI;
		p->OnInit += MakeDelegate(this, &CWndEventImpl::OnInit);
	}
	return p;
}


bool CWndEventImpl::OnInit(LPVOID lpControl)
{
	CControlUI* pControl = (CControlUI*)(lpControl);
	if(pControl)
	{
		if(pControl->GetInterface(_T("LayerControl")))
		{
			auto pLCtl = static_cast<CLayerControlUI*>(pControl);
			pLCtl->SetWMONMOVE(WM_MOVINGNOTIFY);
			::PostMessage(m_hWnd,WM_MOVINGNOTIFY,(WPARAM)pLCtl->GetHWND(),0);	
		}

	}
	return true;
}


void CWndEventImpl::InitWindow()
{

}


bool CWndEventImpl::Notify(TNotifyUI& msg)
{

	return false;
}

#include "TimerThread.h"
std::shared_ptr<CTimerThread> timerThd;

bool CWndEventImpl::OnClick(TNotifyUI& msg)
{
	CUIString sCtrlName = msg.pSender->GetName();

	if (sCtrlName == _T("testbtn"))
	{ 
		timerThd.reset(new CTimerThread);
	}
	else if (sCtrlName == _T("testbtn1"))
	{ 
		if(timerThd)
		{
			timerThd->CreateTimer(1000,0);
		}
	}
	else if (sCtrlName == _T("testbtn2"))
	{ 
		if(timerThd)
		{
			timerThd->KillTimer();
		}
	}
	return false;
}


LRESULT CWndEventImpl::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
{
	LRESULT lRes = 0;
	bHandle = FALSE;

	if(WM_MOVE == uMsg)
	{
		lRes = HandleWindowMove(uMsg, wParam, lParam, bHandle);
	}
	else if(WM_MOVINGNOTIFY == uMsg)
	{
		lRes = HandleAddMoveNotify(uMsg, wParam, lParam, bHandle);
	}

	return lRes;
}


LRESULT CWndEventImpl::HandleAddMoveNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
{
	bHandle = TRUE;
	HWND hWnd = (HWND)wParam;
	if(::IsWindow(hWnd)){
		if(std::find(m_vecPopChild.begin(),m_vecPopChild.end(),hWnd) == m_vecPopChild.end())
			m_vecPopChild.push_back(hWnd);
	}
	return 0;
}


LRESULT CWndEventImpl::HandleWindowMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
{
	//bHandle = FALSE;
	for(auto iter = m_vecPopChild.begin(); m_vecPopChild.end() != iter; NULL)
	{
		HWND& hWnd = *iter;
		if(::IsWindow(hWnd)){
			::PostMessage(hWnd, WM_MOVINGNOTIFY, (WPARAM)m_hWnd, lParam);
		}
		else
		{
			iter = m_vecPopChild.erase(iter);
			continue;
		}
		++iter;
	}
	//::OutputDebugStringA("HandleWindowMove\n");
	return 0;
}