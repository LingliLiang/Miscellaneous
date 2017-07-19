#include "StdAfx.h"
#include "UISliderControl.h"

const TCHAR* const DUI_CTR_SLIDERCONTROL = _T("SliderControl");

namespace DirectUI
{
	CSliderControlUI::CSliderControlUI() : m_uButtonState(0), m_nStep(1),m_bDoubleClicked(false),m_bReverse(false)
	{
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		m_szThumb.cx = m_szThumb.cy = 10;
		m_szThumbCenter.cx = m_szThumbCenter.cy = 0;
	}

	LPCTSTR CSliderControlUI::GetClass() const
	{
		return _T("SliderControlUI");
	}

	UINT CSliderControlUI::GetControlFlags() const
	{
		if (IsEnabled()) return UIFLAG_SETCURSOR;
		else return 0;
	}

	LPVOID CSliderControlUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_SLIDERCONTROL) == 0) return static_cast<CSliderControlUI*>(this);
		return CProgressUI::GetInterface(pstrName);
	}

	void CSliderControlUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if (!IsEnabled())
			m_uButtonState = 0;
	}

	int CSliderControlUI::GetChangeStep()
	{
		return m_nStep;
	}

	void CSliderControlUI::SetChangeStep(int step)
	{
		m_nStep = step;
	}

	void CSliderControlUI::SetThumbSize(SIZE szXY)
	{
		m_szThumb = szXY;
		if(m_szThumbCenter.cx == 0 && m_szThumbCenter.cy == 0)
		{
			m_szThumbCenter.cx = szXY.cx / 2;
			m_szThumbCenter.cy = szXY.cy / 2;
		}
	}

	void CSliderControlUI::SetThumbCenter(SIZE szXY)
	{
		m_szThumbCenter = szXY;
	}

	RECT CSliderControlUI::GetThumbRect() const
	{
		CRect rcSlot(CalcSlotRc());
		if (m_bHorizontal)
		{
			int length = rcSlot.GetWidth() * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			int left = rcSlot.left + length - m_szThumbCenter.cx;
			int top = (rcSlot.bottom + rcSlot.top) / 2 - m_szThumbCenter.cy;
			return CRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
		else 
		{
			int length = rcSlot.GetHeight() * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			int left = (rcSlot.right + rcSlot.left) / 2 - m_szThumbCenter.cx;
			int top = rcSlot.bottom - length - m_szThumbCenter.cy;
			return CRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
	}

	LPCTSTR CSliderControlUI::GetThumbImage() const
	{
		return m_sThumbImage;
	}

	void CSliderControlUI::SetThumbImage(LPCTSTR pStrImage)
	{
		m_sThumbImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CSliderControlUI::GetThumbHotImage() const
	{
		return m_sThumbHotImage;
	}

	void CSliderControlUI::SetThumbHotImage(LPCTSTR pStrImage)
	{
		m_sThumbHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CSliderControlUI::GetThumbPushedImage() const
	{
		return m_sThumbPushedImage;
	}

	void CSliderControlUI::SetThumbPushedImage(LPCTSTR pStrImage)
	{
		m_sThumbPushedImage = pStrImage;
		Invalidate();
	}
	
	LPCTSTR CSliderControlUI::GetForeBkImage() const
	{
		return m_sForeBkImage;
	}

	void CSliderControlUI::SetForeBkImage(LPCTSTR pStrImage)
	{
		m_sForeBkImage = pStrImage;
		Invalidate();
	}

	void CSliderControlUI::DoEvent(TEventUI& event)
	{
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) 
		{
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CProgressUI::DoEvent(event);
			return;
		}

		if(event.Type == UIEVENT_BUTTONDOWN)
		{
			bool bShiftKey = (GetKeyState(VK_SHIFT)&0x8000) != 0;
			if (bShiftKey)
			{
				return;
			}
			if(IsEnabled()) 
			{
				RECT rcThumb = GetThumbRect();
				if (::PtInRect(&rcThumb, event.ptMouse)) 
				{
					m_uButtonState |= UISTATE_CAPTURED;
				}

				m_pManager->SendNotify(this, DUI_MSGTYPE_THUMB_BUTTONDOWN);
			}
			return;
		}

		if (event.Type == UIEVENT_DBLCLICK)
		{
			m_bDoubleClicked = true;
		}

		if (event.Type == UIEVENT_BUTTONUP)
		{
			bool bShiftKey = (GetKeyState(VK_SHIFT)&0x8000) != 0;
			if (bShiftKey)
			{
				if ((m_uButtonState & UISTATE_CAPTURED) != 0) 
				{
					m_uButtonState &= ~UISTATE_CAPTURED;
				}
				return;
			}
			if (!IsEnabled())
				return;

			if (m_bDoubleClicked)
			{
				m_bDoubleClicked = false;
				return;
			}

			int nValue = m_nValue;
			if ((m_uButtonState & UISTATE_CAPTURED) != 0) 
			{
				m_uButtonState &= ~UISTATE_CAPTURED;
			}

			
			CRect rcSlot(CalcSlotRc());
			if (m_bHorizontal)
			{
				if (event.ptMouse.x >= rcSlot.right) nValue = m_nMax;
				else if (event.ptMouse.x <= rcSlot.left) nValue = m_nMin;
				else nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - rcSlot.left ) / rcSlot.GetWidth();
			}
			else 
			{
				if (event.ptMouse.y >= rcSlot.bottom ) nValue = m_nMin;
				else if (event.ptMouse.y <= rcSlot.top) nValue = m_nMax;
				else nValue = m_nMin + (m_nMax - m_nMin) * (rcSlot.bottom  - event.ptMouse.y ) / rcSlot.GetHeight();
			}

			if(m_nValue != nValue && nValue >= m_nMin && nValue <= m_nMax)
			{
				m_nValue = nValue;
				m_pManager->SendNotify(this, DUI_MSGTYPE_THUMB_BUTTONUP);
				m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
				Invalidate();
			}
			else
			{
				m_pManager->SendNotify(this, DUI_MSGTYPE_THUMB_BUTTONUP);
			}

			return;
		}

		if (event.Type == UIEVENT_CONTEXTMENU)
		{
			return;
		}

		if (event.Type == UIEVENT_SCROLLWHEEL) 
		{
			if (!IsEnabled())
				return;
			switch (LOWORD(event.wParam))
			{
			case SB_LINEUP:
				SetValue(GetValue() + GetChangeStep());
				m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
				return;
			case SB_LINEDOWN:
				SetValue(GetValue() - GetChangeStep());
				m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
				return;
			}
		}

		if (event.Type == UIEVENT_MOUSEMOVE)
		{
			if (IsEnabled()) {
				if ((m_uButtonState & UISTATE_CAPTURED) != 0) 
				{
					CRect rcSlot(CalcSlotRc());
					if (m_bHorizontal) 
					{
						if (event.ptMouse.x >= rcSlot.right) 
							m_nValue = m_nMax;
						else if (event.ptMouse.x <= rcSlot.left) 
							m_nValue = m_nMin;
						else 
							m_nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - rcSlot.left ) / rcSlot.GetWidth();
					}
					else 
					{
						if (event.ptMouse.y >= rcSlot.bottom ) 
							m_nValue = m_nMin;
						else if (event.ptMouse.y <= rcSlot.top) 
							m_nValue = m_nMax;
						else 
							m_nValue = m_nMin + (m_nMax - m_nMin) * (rcSlot.bottom - event.ptMouse.y ) / rcSlot.GetHeight();
					}
					m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);

					Invalidate();
				}

			}
			return;
		}

		if (event.Type == UIEVENT_SETCURSOR)
		{
			RECT rcThumb = GetThumbRect();
			if (IsEnabled() && ::PtInRect(&rcThumb, event.ptMouse))
			{
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
				return;
			}
		}

		if (event.Type == UIEVENT_MOUSEENTER)
		{
			if (IsEnabled())
			{
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}

		if (event.Type == UIEVENT_MOUSELEAVE)
		{
			if (IsEnabled()) 
			{
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}

		CControlUI::DoEvent(event);
	}

	void CSliderControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("thumbimage")) == 0) SetThumbImage(pstrValue);
		else if(_tcscmp(pstrName, _T("thumbhotimage")) == 0) SetThumbHotImage(pstrValue);
		else if(_tcscmp(pstrName, _T("thumbpushedimage")) == 0) SetThumbPushedImage(pstrValue);
		else if(_tcscmp(pstrName, _T("forebkimage")) == 0) SetForeBkImage(pstrValue);
		else if(_tcscmp(pstrName, _T("thumbcenter")) == 0)
		{
			SIZE szXY = {0};
			LPTSTR pstr = NULL;
			szXY.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szXY.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
			SetThumbCenter(szXY);
		}
		else if(_tcscmp(pstrName, _T("thumbsize")) == 0)
		{
			SIZE szXY = {0};
			LPTSTR pstr = NULL;
			szXY.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szXY.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
			SetThumbSize(szXY);
		}
		else if (_tcscmp(pstrName, _T("step")) == 0) SetChangeStep(_ttoi(pstrValue));
		else if (_tcscmp(pstrName, _T("insetslot")) == 0)
		{
			RECT rcPos = { 0 };
			LPTSTR pstr = NULL;
			rcPos.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcPos.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcPos.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcPos.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetSlotInset(rcPos);
		}
		else if (_tcscmp(pstrName, _T("reverse")) == 0) SetReverse(_tcscmp(pstrValue, _T("true")) == 0);
		else CProgressUI::SetAttribute(pstrName, pstrValue);
	}

	void CSliderControlUI::PaintStatusImage(HDC hDC)
	{
		if (m_nMax <= m_nMin) m_nMax = m_nMin + 1;
		if (m_nValue > m_nMax) m_nValue = m_nMax;
		if (m_nValue < m_nMin) m_nValue = m_nMin;
		
		CRect rcThumb = GetThumbRect();

		if (!m_sForeBkImage.IsEmpty()) 
		{
			CRect rcSlot(CalcSlotRc());
			rcSlot.MoveToXY(rcSlot.left - m_rcItem.left,rcSlot.top - m_rcItem.top);
			m_sForeImageModify.Empty();
			if (m_bStretchForeImage)
				m_sForeImageModify.SmallFormat(_T("dest='%d,%d,%d,%d' "), rcSlot.left, rcSlot.top, rcSlot.right, rcSlot.bottom);
			else
				m_sForeImageModify.SmallFormat(_T("dest='%d,%d,%d,%d' source='%d,%d,%d,%d' ")
				, rcSlot.left, rcSlot.top, rcSlot.right, rcSlot.bottom
				, rcSlot.left, rcSlot.top, rcSlot.right, rcSlot.bottom);
			int posFileFormat = m_sForeBkImage.Find(_T("file='"));
			CString strImg = m_sForeBkImage.GetData();
			if(posFileFormat != -1)
			{
				strImg.Insert(posFileFormat,m_sForeImageModify.GetData());
				m_sForeImageModify.Empty();
			}
			if (!DrawImage(hDC, (LPCTSTR)strImg,(LPCTSTR)m_sForeImageModify)) 
				m_sForeBkImage.Empty();
		}

		if (!m_sForeImage.IsEmpty()) 
		{
			CRect rcSlot(CalcSlotRc());
			rcSlot.MoveToXY(rcSlot.left - m_rcItem.left,rcSlot.top - m_rcItem.top);			
			if (m_bHorizontal) 
			{
				rcSlot.right = rcSlot.left + (rcSlot.GetWidth()*(m_nValue - m_nMin)) / (m_nMax - m_nMin);
			}
			else
			{
				rcSlot.top = rcSlot.bottom - (rcSlot.GetHeight()*(m_nValue - m_nMin)) / (m_nMax - m_nMin);
			}
			m_sForeImageModify.Empty();
			if (m_bStretchForeImage)
				m_sForeImageModify.SmallFormat(_T("dest='%d,%d,%d,%d' "), rcSlot.left, rcSlot.top, rcSlot.right, rcSlot.bottom);
			else
				m_sForeImageModify.SmallFormat(_T("dest='%d,%d,%d,%d' source='%d,%d,%d,%d' ")
				, rcSlot.left, rcSlot.top, rcSlot.right, rcSlot.bottom
				, rcSlot.left, rcSlot.top, rcSlot.right, rcSlot.bottom);
			int posFileFormat = m_sForeImage.Find(_T("file='"));
			CString strImg = m_sForeImage.GetData();
			if(posFileFormat != -1)
			{
				strImg.Insert(posFileFormat,m_sForeImageModify.GetData());
				m_sForeImageModify.Empty();
			}
			if (!DrawImage(hDC, (LPCTSTR)strImg,(LPCTSTR)m_sForeImageModify)) 
				m_sForeImage.Empty();
		}

		rcThumb.MoveToXY(rcThumb.left - m_rcItem.left,rcThumb.top - m_rcItem.top);
		if ((m_uButtonState & UISTATE_CAPTURED) != 0) 
		{
			if (!m_sThumbPushedImage.IsEmpty())
			{
				m_sImageModify.Empty();
				m_sImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
				if (!DrawImage(hDC, (LPCTSTR)m_sThumbPushedImage, (LPCTSTR)m_sImageModify)) 
					m_sThumbPushedImage.Empty();
			}
		}
		else if ((m_uButtonState & UISTATE_HOT) != 0)
		{
			if (!m_sThumbHotImage.IsEmpty()) 
			{
				m_sImageModify.Empty();
				m_sImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
				if (!DrawImage(hDC, (LPCTSTR)m_sThumbHotImage, (LPCTSTR)m_sImageModify)) 
					m_sThumbHotImage.Empty();
			}
		}
		if (!m_sThumbImage.IsEmpty()) 
		{
			m_sImageModify.Empty();
			m_sImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
			if (!DrawImage(hDC, (LPCTSTR)m_sThumbImage, (LPCTSTR)m_sImageModify)) 
				m_sThumbImage.Empty();
		}

		rcThumb = GetThumbRect();
		RECT rc = {m_rcItem.left,rcThumb.top+m_szThumbCenter.cy,m_rcItem.right,rcThumb.top+m_szThumbCenter.cy};

		CRenderEngine::DrawLine(hDC,rc,1,0xffffffff);
	}

	RECT CSliderControlUI::GetSlotInset() const
	{
		return m_rcSlotInset;
	}

	void CSliderControlUI::SetSlotInset(RECT rc)
	{
		m_rcSlotInset = rc;
	}

	RECT CSliderControlUI::CalcSlotRc() const
	{
		if(m_bHorizontal)
			return CRect(m_rcItem.left + m_rcSlotInset.left + m_szThumbCenter.cx,m_rcItem.top + m_rcSlotInset.top,m_rcItem.right - m_rcSlotInset.right - (m_szThumb.cx - m_szThumbCenter.cx) ,m_rcItem.bottom - m_rcSlotInset.bottom);
		else
			return CRect(m_rcItem.left + m_rcSlotInset.left,m_rcItem.top + m_rcSlotInset.top + m_szThumbCenter.cy,m_rcItem.right - m_rcSlotInset.right,m_rcItem.bottom - m_rcSlotInset.bottom - (m_szThumb.cy - m_szThumbCenter.cy));
	}

	void CSliderControlUI::SetReverse(bool b)
	{
		m_bReverse = b;
		Invalidate();
	}

	bool CSliderControlUI::GetReverse() const
	{
		return m_bReverse;
	}

	SIZE CSliderControlUI::EstimateSize(SIZE szAvailable)
	{
		return CControlUI::EstimateSize(szAvailable);
	}
}
