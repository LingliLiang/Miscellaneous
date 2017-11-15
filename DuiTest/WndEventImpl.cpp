#include "WndEventImpl.h"
#include "ui\LayerLayoutUI.h"
#include "ui\UIAnalogStick.h"
#include "UI\BricksTabUI.h"
#include "UI\UISpinButton.h"

CControlUI* CWndEventImpl::CreateControl(LPCTSTR strControl)
{
	CControlUI* p = CLayerLayoutUI::CreateControl(strControl);
	if (!p)
	{
		if (_tcsicmp(strControl, DUI_CTR_ANALOGSTICK) == 0)
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
	}
	return p;
}


bool CWndEventImpl::Notify(TNotifyUI& msg)
{

	return false;
}


bool CWndEventImpl::OnClick(TNotifyUI& msg)
{
	CUIString sCtrlName = msg.pSender->GetName();

	if (sCtrlName == _T("testbtn"))
	{ 
	}

	return false;
}


LRESULT CWndEventImpl::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = 0;
	bHandled = FALSE;
	return lRes;
}