#include "StdAfx.h"
#include "UIEditSlider.h"

namespace DirectUI
{
	CEditSliderUI::CEditSliderUI():m_strEditName(_T("")),m_strSliderName(_T("")),m_pEdit(NULL),m_pSlider(NULL),\
									m_bEditState(false),m_fDefaultValue(1.0f),m_fMultipleValue(1.0f),m_bInitDone(false), \
									m_bEditFocus(false)
	{
	}


	CEditSliderUI::~CEditSliderUI()
	{
		if (m_pManager)
		{
			m_pManager->KillTimer(this, DEFAULT_TIMER_ID);
		}
	}


	LPCTSTR CEditSliderUI::GetClass() const
	{
		return _T("EditSliderUI");
	}


	UINT CEditSliderUI::GetControlFlags() const
	{
		if (IsEnabled()) return UIFLAG_SETCURSOR;
		else return 0;
	}


	void CEditSliderUI::SetEnabled(bool bEnable /* = true */)
	{
		CControlUI::SetEnabled(bEnable);
	}


	LPVOID CEditSliderUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_SLIDER) == 0) return static_cast<CEditSliderUI*>(this);
		return CVerticalLayoutUI::GetInterface(pstrName);
	}


	void CEditSliderUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("slidername")) == 0)
		{
			SetSliderName(pstrValue);
		}
		else if (_tcscmp(pstrName, _T("editname")) == 0)
		{
			SetEditName(pstrValue);
		}
		else if (_tcscmp(pstrName, _T("defaultvalue")) == 0)
		{
			float fValue = _ttof(pstrValue);
			SetDefaultValue(fValue);
		}
		else if (_tcscmp(pstrName, _T("multiple")) == 0)
		{
			float fValue = _ttof(pstrValue);
			SetMultipleValue(fValue);
		}
		else
		{
			CVerticalLayoutUI::SetAttribute(pstrName, pstrValue);
		}
	}


	void CEditSliderUI::DoEvent(TEventUI& event)
	{
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) 
		{
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CVerticalLayoutUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_DBLCLICK)
		{
			CSliderUI* pSliderUI = static_cast<CSliderUI*>(m_pManager->FindControl(GetSliderName()));
			if (pSliderUI)
			{
				bool bEnabled = pSliderUI->IsEnabled();
				if (!bEnabled) return;
			}

			if (!m_bInitDone)
			{
				m_bInitDone = true;
				if (m_pManager)
				{
					m_pManager->SetTimer(this, DEFAULT_TIMER_ID, DEFAULT_TIME);
				}
				CControlUI* pControl = static_cast<CControlUI*>(m_pManager->FindControl(GetEditName()));
				if (pControl)
				{
					pControl->OnNotify += MakeDelegate(this, &CEditSliderUI::OnNotify);
				}
			}

			bool bShiftKey = (GetKeyState(VK_SHIFT)&0x8000);
			
			if (bShiftKey)
			{
				//set default value;
				CSliderUI* pSlider = static_cast<CSliderUI*>(m_pManager->FindControl(GetSliderName()));
				if (pSlider)
				{
					pSlider->SetValue(GetDefaultValue());
					m_pManager->SendNotify(pSlider, DUI_MSGTYPE_VALUECHANGED);
				}
			}
			else
			{
				int nValue = -1;
				CSliderUI* pSlider = static_cast<CSliderUI*>(m_pManager->FindControl(GetSliderName()));
				if (pSlider)
				{
					nValue = pSlider->GetValue();
					pSlider->SetVisible(false);
				}
				CEditUI* pEdit = static_cast<CEditUI*>(m_pManager->FindControl(GetEditName()));
				if (pEdit)
				{
					CUIString strValue = _T("");
					strValue.Format(_T("%g"), nValue*GetMultipleValue());
					pEdit->SetText(strValue);
					pEdit->SetVisible(true);
					pEdit->SetFocus();
					pEdit->SetSelAll();
					SetEditState(true);
					m_bEditFocus = true;
				}
			}

			//m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
			Invalidate();
			return;
		}

		if (event.Type == UIEVENT_MOUSELEAVE)
		{
			return;
		}

		if (event.Type == UIEVENT_KILLFOCUS)
		{
			return;
		}

		if ((event.Type==UIEVENT_TIMER) && (event.wParam==DEFAULT_TIMER_ID))
		{
			float fValue = 0.0f;
			CEditUI* pEdit = static_cast<CEditUI*>(m_pManager->FindControl(GetEditName()));
			if (pEdit)
			{
				if (m_bEditFocus)
				{
					return;
				}
				pEdit->SetVisible(false);
				CUIString strText = pEdit->GetText();
				fValue = _ttof(strText);
			}

			CSliderUI* pSlider = static_cast<CSliderUI*>(m_pManager->FindControl(GetSliderName()));
			if (pSlider)
			{
				if (IsEditState())
				{
					int nMaxValue = pSlider->GetMaxValue();
					int nMinValue = pSlider->GetMinValue();

					int nValue = fValue/GetMultipleValue();

					nValue = ((nValue<=nMinValue)?(nMinValue):(nValue));
					nValue = ((nValue>=nMaxValue)?(nMaxValue):(nValue));

					pSlider->SetValue(nValue);
					SetEditState(false);
					m_pManager->SendNotify(pSlider, DUI_MSGTYPE_VALUECHANGED);
				}
				pSlider->SetVisible(true);
			}

			m_bEditFocus = false;
			//Invalidate();
			return;
		}

		CControlUI::DoEvent(event);
	}


	bool CEditSliderUI::OnNotify(void *pNotify)
	{
		TNotifyUI* pMsg = (TNotifyUI*)pNotify;
		if (!pMsg) return false;

		if (pMsg->sType == DUI_MSGTYPE_KILLFOCUS)
		{
			m_bEditFocus = false;
		}
		else if (pMsg->sType == DUI_MSGTYPE_RETURN)
		{
			m_bEditFocus = false;
		}
		return false;
	}


	void CEditSliderUI::SetEditName(LPCTSTR pstrName)
	{
		m_strEditName = pstrName;
	}


	LPCTSTR CEditSliderUI::GetEditName() const
	{
		return m_strEditName;
	}


	void CEditSliderUI::SetSliderName(LPCTSTR pstrName)
	{
		m_strSliderName = pstrName;
	}


	LPCTSTR CEditSliderUI::GetSliderName() const
	{
		return m_strSliderName;
	}


	void CEditSliderUI::SetEditState(bool bEdit)
	{
		m_bEditState = bEdit;
	}


	bool CEditSliderUI::IsEditState() const
	{
		return m_bEditState;
	}


	void CEditSliderUI::SetDefaultValue(float fValue)
	{
		m_fDefaultValue = fValue;
	}


	float CEditSliderUI::GetDefaultValue() const
	{
		return m_fDefaultValue;
	}


	void CEditSliderUI::SetMultipleValue(float fValue)
	{
		m_fMultipleValue = fValue;
	}


	float CEditSliderUI::GetMultipleValue() const
	{
		return m_fMultipleValue;
	}
}


