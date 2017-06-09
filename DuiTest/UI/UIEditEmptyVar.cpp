#include "stdafx.h"
#include "UIEdit.h"
#include "UIEditEmptyVar.h"
//#define  DUI_CTR_EDIT_EVAR                       (_T("EditEmptyVar")) uidefine.h
//else if (_tcscmp(pstrClass, DUI_CTR_EDIT_EVAR) == 0)	  pControl = new CEditEmptyVarUI; uidlgbuilder.cpp
//#include "Control/UIEditEmptyVar.h" uilib.h
////exmaple       <EditEmptyVar width = "120" height = "30" emptytext = "ÎÒÊÇ¿ÕµÄ" text = "asddsad" etdropcolor = "#FF242424" bkcolor = "#FF242424" nativebkcolor = "#FF242424" textcolor = "#FFF3F3F3" / >

namespace DirectUI
{
	CEditEmptyVarUI::CEditEmptyVarUI()
	{
		m_dwDropColor = 0xff3F3F3F;
	}

	LPCTSTR CEditEmptyVarUI::GetClass() const
	{
		return _T("EditEmptyVarUI");
	}

	LPVOID CEditEmptyVarUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_EDIT_EVAR) == 0)
			return static_cast<CEditEmptyVarUI*>(this);

		return CEditUI::GetInterface(pstrName);
	}

	void CEditEmptyVarUI::DoEvent(TEventUI& event)
	{
		CEditUI::DoEvent(event);
	}

	LPCTSTR CEditEmptyVarUI::GetEmptyText()
	{
		return m_sEmptyText;
	}

	void CEditEmptyVarUI::SetEmptyText(LPCTSTR pStrText)
	{
		m_sEmptyText = pStrText;
		Invalidate();
	}

	void CEditEmptyVarUI::SetDropColor(DWORD dwTextColor)
	{
		m_dwDropColor = dwTextColor;
		Invalidate();
	}

	void CEditEmptyVarUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("emptytext")) == 0 ) SetEmptyText(pstrValue);
		else if (_tcscmp(pstrName, _T("etdropcolor")) == 0)
		{
			if (*pstrValue == _T('#'))
				pstrValue = ::CharNext(pstrValue);

			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetDropColor(clrColor);
		}
		else CEditUI::SetAttribute(pstrName, pstrValue);
	}

	void CEditEmptyVarUI::PaintText(HDC hDC)
	{
		bool emptyText = false;
		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		if (m_sText.IsEmpty())
		{
			if (m_sEmptyText.IsEmpty()) return;
			m_sText = m_sEmptyText;
			emptyText = true;
		}

		CUIString sText = m_sText;
		if (emptyText) m_sText.Empty();
		if( m_bPasswordMode ) {
			if (!emptyText){
				sText.Empty();
				LPCTSTR p = m_sText.GetData();
				while( *p != _T('\0') ) {
					sText += m_cPasswordChar;
					p = ::CharNext(p);
				}
			}
		}

		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;
		if( IsEnabled() ) {
			CRenderEngine::DrawText(hDC, m_pManager, rc, sText, emptyText ? m_dwTextColor-m_dwDropColor : m_dwTextColor, \
				m_iFont, DT_SINGLELINE | m_uTextStyle);
		}
		else {
			CRenderEngine::DrawText(hDC, m_pManager, rc, sText, emptyText ? m_dwTextColor - m_dwDropColor : m_dwDisabledTextColor, \
				m_iFont, DT_SINGLELINE | m_uTextStyle);

		}
	}
}
