#include "stdafx.h"
#include "MessageBoxUI.h"

#define SKINFOLDER _T("");
static const TCHAR* xml_path = _T("messagebox\\");
static const TCHAR* xml = _T("messagebox.xml");

namespace DirectUI
{

	LPCTSTR CMessageBoxUI::GetWindowClassName() const
	{
		return _T("MessageBoxUI");
	}

	CUIString CMessageBoxUI::GetSkinFile()
	{
		return xml;
	}

	CUIString CMessageBoxUI::GetSkinFolder()
	{
		return SKINFOLDER;
	}

	void CMessageBoxUI::InitWindow()
	{

		CLabelUI* pIcon = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lb_icon")));
		CTextUI* pText = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("t_caption")));
		pText->SetText(m_strCaption);
		CButtonUI* m_pButtonOk = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_ok")));
		CButtonUI* m_pButtonNo = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_no")));
		CButtonUI* m_pButtonCancel = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_cancel")));
		static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("re_texts")))->SetText(m_strText);
		CHorizontalLayoutUI* pHor = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("hl_btn")));
		int iButton= m_uType & 0x0000000FL;
		int iIcon= m_uType & 0x000000F0L;
		//Button style
		switch(iButton)
		{
		case MB_YESNO:
			m_pButtonOk->SetVisible();
			//m_pButtonOk->SetText(_T("Ok"));
			m_pButtonNo->SetVisible();
			//m_pButtonNo->SetText(_T("No"));
			m_pButtonNo->SetFocus();
			break;
		case MB_YESNOCANCEL:
			m_pButtonOk->SetVisible();
			//m_pButtonOk->SetText(_T("Yes"));
			m_pButtonNo->SetVisible();
			//m_pButtonNo->SetText(_T("No"));
			m_pButtonCancel->SetVisible();
			//m_pButtonCancel->SetText(_T("Cancel"));
			m_pButtonCancel->SetFocus();
			break;
		default: //MB_OK
			{
				m_pButtonOk->SetVisible();
			//	m_pButtonOk->SetText(_T("Ok"));
				m_pButtonOk->SetFocus();
			}
			break;
		}
		//Icon
		CUIString bkimg;
		switch(iIcon)
		{
		case MB_ICONQUERY:
			bkimg.Format(_T("%s%s"), xml_path, _T("icon_query.png"));
			break;
		case MB_ICONWARNING:
						bkimg.Format(_T("%s%s"), xml_path, _T("icon_warning.png"));
			pText->SetTextColor(0xE6CD5C);
			break;
		case MB_ICONERROR:
						bkimg.Format(_T("%s%s"), xml_path, _T("icon_error.png"));
			pText->SetTextColor(0xFFFF0000);
			break;
		default: //MB_ICONINFO
						bkimg.Format(_T("%s%s"), xml_path, _T("icon_info.png"));
			break;
		}
		pIcon->SetBkImage(bkimg);
	}

	LRESULT CMessageBoxUI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if(uMsg == WM_NCLBUTTONDBLCLK) 
		{
			return true;
		}
		return CWindowUI::HandleMessage(uMsg, wParam, lParam);
	}

	void CMessageBoxUI::OnFinalMessage(HWND hWnd)
	{
		delete this;
	}

	void CMessageBoxUI::Notify(TNotifyUI& msg)
	{
		
		if(_tcscmp(msg.pSender->GetName() ,_T("re_texts")) == 0)
		{
			if(_tcscmp(msg.sType ,DUI_MSGTYPE_SETFOCUS) == 0)
			{
				//CreateCaret(msg.pSender->GetManager()->GetPaintWindow(),NULL,0,0);
				//HideCaret(msg.pSender->GetManager()->GetPaintWindow());
			}
			if(_tcscmp(msg.sType ,DUI_MSGTYPE_KILLFOCUS) == 0)
			{
				//HideCaret(msg.pSender->GetManager()->GetPaintWindow());
			}
			if(_tcscmp(msg.sType ,DUI_MSGTYPE_TIMER) == 0)
			{
			}
		}
		__super::Notify(msg);
	}

	LRESULT CMessageBoxUI::ResponseDefaultKeyEvent(WPARAM wParam)
	{
		if(wParam == VK_ESCAPE)
			Close(1);

		return true;
	}

	void CMessageBoxUI::ShowWindow(bool bShow, bool bTakeFocus)
	{
		__super::ShowWindow(bShow,bTakeFocus);
	}

	UINT CMessageBoxUI::ShowModal()
	{
		return __super::ShowModal();
	}

	void CMessageBoxUI::OnClick(TNotifyUI& msg)
	{
		int nRet = 0;
		if(_tcscmp(msg.pSender->GetClass() ,_T("ButtonUI")) == 0)
		{
			if(_tcscmp(msg.pSender->GetName() ,_T("btn_ok")) == 0)
			{
				if((m_uType & 0x0000000FL) != MB_OK)
				{
					nRet = IDYES;
				}
				else
				{
					nRet = IDOK;
				}
			}
			if(_tcscmp(msg.pSender->GetName() ,_T("btn_no")) == 0)
			{
				nRet = IDNO;
			}
			if(_tcscmp(msg.pSender->GetName() ,_T("btn_cancel")) == 0)
			{
				nRet = IDCANCEL;	
			}
			this->Close(nRet);
		}
		//
	}

	void CMessageBoxUI::SetCopyable(bool bCopy)
	{
		CRichEditUI* pRE = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("re_texts")));
		if(pRE)
		{
			pRE->SetEnabled(bCopy);	
		}
	}


} //namespace DirectUI


