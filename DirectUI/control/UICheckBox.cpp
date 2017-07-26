#include "stdafx.h"
#include "UICheckBox.h"

namespace DirectUI
{
	CCheckBoxUI::CCheckBoxUI()
	{
		m_isAutoCheck = true;
	}

	LPCTSTR CCheckBoxUI::GetClass() const
	{
		return _T("CheckBoxUI");
	}

	void CCheckBoxUI::SetCheck(bool bCheck)
	{
		Selected(bCheck);
	}

	bool  CCheckBoxUI::GetCheck() const
	{
		return IsSelected();
	}

	void CCheckBoxUI::SetAutoCheck(bool isAutoCheck)
	{
		m_isAutoCheck = isAutoCheck;
	}

	bool CCheckBoxUI::GetAutoCheck() const
	{
		return m_isAutoCheck;
	}

	bool CCheckBoxUI::Activate()
	{
		return  m_isAutoCheck ? __super::Activate() : CButtonUI::Activate();
	}


	LPCTSTR CCheckBoxUI::GetSelectedNormalImage()
	{
		return m_sSelectedNormalImage;
	}

	void CCheckBoxUI::SetSelectedNormalImage(LPCTSTR pStrImage)
	{
		m_sSelectedNormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CCheckBoxUI::GetSelectedSelectedImage()
	{
		return m_sSelectedSelectedImage;
	}

	void CCheckBoxUI::SetSelectedSelectedImage(LPCTSTR pStrImage)
	{
		m_sSelectedSelectedImage = pStrImage;
		Invalidate();
	}

	//************************************
	// Method:    GetSelectedHotImage
	// FullName:  CCheckBoxUI::GetSelectedHotImage
	// Access:    public 
	// Returns:   LPCTSTR
	// Qualifier:
	// Node:	  
	//************************************
	LPCTSTR CCheckBoxUI::GetSelectedDisabledImage()
	{
		return m_sSelectedDisabledImage;
	}
	//************************************
	// Method:    SetSelectedHotImage
	// FullName:  CCheckBoxUI::SetSelectedHotImage
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPCTSTR pStrImage
	// Node:	  
	//************************************
	void CCheckBoxUI::SetSelectedDisabledImage( LPCTSTR pStrImage )
	{
		m_sSelectedDisabledImage = pStrImage;
		Invalidate();
	}

	void CCheckBoxUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("selectednormalimage")) == 0 ) SetSelectedNormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("selectedselectedimage")) == 0 ) SetSelectedSelectedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("selecteddisabledimage")) == 0 ) SetSelectedDisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("autocheck")) == 0) SetAutoCheck(  _tcscmp(pstrValue, _T("true")) == 0 );
		else COptionUI::SetAttribute(pstrName, pstrValue);
	}

	void CCheckBoxUI::PaintStatusImage(HDC hDC)
	{
		m_uButtonState &= ~UISTATE_PUSHED;
		//CUIString str;
		//str.Format(_T("button:%x\r\n"), m_uButtonState);
		//OutputDebugString(str);
		if (IsSelected()) {
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				if( !m_sSelectedSelectedImage.IsEmpty() ) {
					if( !DrawImage(hDC, (LPCTSTR)m_sSelectedSelectedImage) ) m_sSelectedSelectedImage.Empty();
					else goto Label_ForeImage;
				} 
			} else if( (m_uButtonState & UISTATE_HOT) != 0) {
				if (!m_sSelectedHotImage.IsEmpty()) {
					if( !DrawImage(hDC, (LPCTSTR)m_sSelectedHotImage) )
						m_sSelectedHotImage.Empty();
					else goto Label_ForeImage;
				}
			} else if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
				if (!m_sSelectedDisabledImage.IsEmpty()) {
					if( !DrawImage(hDC, (LPCTSTR)m_sSelectedDisabledImage) )
						m_sSelectedDisabledImage.Empty();
					else goto Label_ForeImage;
				}
			} else {
				if (!m_sSelectedNormalImage.IsEmpty()) {
					if( !DrawImage(hDC, (LPCTSTR)m_sSelectedNormalImage) )
						m_sSelectedNormalImage.Empty();
					else goto Label_ForeImage;
				}
			}
		} else {
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				if( !m_sSelectedImage.IsEmpty() ) {
					if( !DrawImage(hDC, (LPCTSTR)m_sSelectedImage) ) m_sSelectedImage.Empty();
					else goto Label_ForeImage;
				} 
			}
		}
		
		

		COptionUI::PaintStatusImage(hDC);

Label_ForeImage:
		if( !m_sForeImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sForeImage) ) m_sForeImage.Empty();
		}
	}
}
