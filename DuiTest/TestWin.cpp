#include "stdafx.h"
#include "TestWin.h"
#include "LayerLayoutUI.h"


#define SKINFILE  _T("TestWin.xml");
#define SKINFOLDER _T("skin\\default");
LPCTSTR CTestWin::GetWindowClassName() const
{
	return _T("TestWinUI");
}

CUIString CTestWin::GetSkinFile()
{
	return SKINFILE;
}

CUIString CTestWin::GetSkinFolder()
{
	return SKINFOLDER;
}

CControlUI* CTestWin::CreateControl(LPCTSTR strControl)
{
	return CLayerLayoutUI::CreateControl(strControl);
}

void CTestWin::OnClick(TNotifyUI& msg)
	{
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
		}	else if (sCtrlName == _T("testbtn"))
		{ 
			CLayerLayoutUI* p =(CLayerLayoutUI*)GetControl(_T("layerlist"));
			if(p)
			{
				p->BindFunc(4,&CTestWin::Drop,this);
			}
			return; 
		}
		
		return;
	}

void CTestWin::Drop()
{
	::OutputDebugString(_T("Drop Target!\n"));
}