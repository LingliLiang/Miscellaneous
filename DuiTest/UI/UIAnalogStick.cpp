#include "StdAfx.h"
#include "UIAnalogStick.h"

#define PI 3.1415926
const TCHAR*  DUI_CTR_ANALOGSTICK = _T("AnalogStick");

namespace DirectUI
{
	CAnalogStickUI::CAnalogStickUI()
		:m_nRadius(100),m_uButtonState(0),m_fAngle(0)
	{
		m_ptCenter.x = m_ptCenter.y = 0;
		m_ptCurrent.x = m_ptCurrent.y = 0;
	}

	CAnalogStickUI::~CAnalogStickUI()
	{
	}

	LPCTSTR CAnalogStickUI::GetClass() const
	{
		return _T("AnalogStickUI");
	}

	UINT CAnalogStickUI::GetControlFlags() const
	{
		return (IsEnabled() ? UIFLAG_SETCURSOR : 0);
	}

	LPVOID CAnalogStickUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_ANALOGSTICK) == 0)
			return static_cast<CAnalogStickUI*>(this);

		return CControlUI::GetInterface(pstrName);
	}

	LPCTSTR CAnalogStickUI::GetNormalImage() const
	{
		return m_strNormalImage.c_str();
	}

	void CAnalogStickUI::SetNormalImage(LPCTSTR pStrImage)
	{
		m_strNormalImage = pStrImage;
	}

	LPCTSTR CAnalogStickUI::GetHotImage() const
	{
		return m_strHotImage.c_str();
	}

	void CAnalogStickUI::SetHotImage(LPCTSTR pStrImage)
	{
		m_strHotImage = pStrImage;
	}

	LPCTSTR CAnalogStickUI::GetStickImage() const
	{
		return m_strAnalogStick.c_str();
	}

	void CAnalogStickUI::SetStickImage(LPCTSTR pStrImage)
	{
		m_strAnalogStick = pStrImage;
	}

	int	CAnalogStickUI::GetRadius() const
	{
		return m_nRadius;
	}

	void CAnalogStickUI::SetRadius(LPCTSTR pStrRadius)
	{
		ASSERT(pStrRadius);
		m_nRadius = _ttoi(pStrRadius);
	}

	POINT CAnalogStickUI::GetCenterPoint() const
	{
		return m_ptCenter;
	}

	void CAnalogStickUI::SetCenterPoint(LPCTSTR pStrPoint)
	{
		ASSERT(pStrPoint);
        LPTSTR pstr = NULL;
		m_ptCenter.x = _tcstol(pStrPoint, &pstr, 10);  ASSERT(pstr);    
        m_ptCenter.y = _tcstol(pstr + 1, &pstr, 10);   ASSERT(pstr);   
	}

	SIZE CAnalogStickUI::GetStickImageRowCol() const
	{
		return m_szRowcol;
	}

	void CAnalogStickUI::SetStickImageRowCol(SIZE szRowcol)
	{
		m_szRowcol = szRowcol;
		m_nStep = m_szRowcol.cy * m_szRowcol.cx;
	}

	RECT CAnalogStickUI::GetStickRect()
	{
		return m_rcAnalogStickBase;
	}

	SIZE CAnalogStickUI::EstimateSize(SIZE szAvailable)
	{
		if (m_cxyFixed.cy == 0) 
			return CSize(m_cxyFixed.cx, 8);

		return CControlUI::EstimateSize(szAvailable);
	}

	void CAnalogStickUI::SetStickRect(RECT rc)
	{
		m_rcAnalogStickBase = rc;
	}

	bool CAnalogStickUI::Activate()
	{
		if (!CControlUI::Activate()) return false;
		if (m_pManager) m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);

		return true;
	}

	void CAnalogStickUI::DoEvent(TEventUI& event)
	{
		if (!IsMouseEnabled() && 
			event.Type > UIEVENT__MOUSEBEGIN && 
			event.Type < UIEVENT__MOUSEEND) 
		{
			if (m_pParent) 
			{
				m_pParent->DoEvent(event);
			}
			return;
		}

		if (event.Type == UIEVENT_MOUSEMOVE)
		{
			if ((m_uButtonState & UISTATE_CAPTURED) != 0) 
			{
				AdjustAngle(event.ptMouse);
				Invalidate();
			}
			return;
		}

		if (event.Type == UIEVENT_BUTTONDOWN)
		{
			if (IsEnabled())
			{
				if (::PtInRect(&m_rcItem, event.ptMouse)) 
					m_uButtonState |= UISTATE_CAPTURED;
			}
			return;
		}

		if (event.Type == UIEVENT_BUTTONUP)
		{
			if ((m_uButtonState & UISTATE_CAPTURED) != 0 )
			{
				if (::PtInRect(&m_rcItem, event.ptMouse))
					Activate();

				m_uButtonState &= ~UISTATE_CAPTURED;
				Invalidate();
			}
			return;
		}

		if (event.Type == UIEVENT_CONTEXTMENU)
		{
			return;
		}

		if (event.Type == UIEVENT_KEYDOWN)
		{
			if (IsKeyboardEnabled()) 
			{
				if (event.chKey == VK_SPACE || event.chKey == VK_RETURN)  
				{
					Activate();
					return;
				}
			}
		}

		if (event.Type == UIEVENT_SETFOCUS) 
		{
			Invalidate();
		}
		if ( event.Type == UIEVENT_KILLFOCUS) 
		{
			Invalidate();
		}

		CControlUI::DoEvent(event);
	}

	void CAnalogStickUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("radius")) == 0) SetRadius(pstrValue);
		else if (_tcscmp(pstrName, _T("centerpoint")) == 0) SetCenterPoint(pstrValue);
		else if (_tcscmp(pstrName, _T("imgrowcol")) == 0)
		{
			LPTSTR pstr = NULL;
			SIZE sz = { 0 };
			sz.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			sz.cy = _tcstol(pstr + 1, &pstr, 10);   ASSERT(pstr);
			SetStickImageRowCol(sz);
		}
		else if (_tcscmp(pstrName, _T("imgrect")) == 0)
		{
			RECT rc = { 0 };
			LPTSTR pstr = NULL;
			rc.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			rc.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rc.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rc.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			SetStickRect(rc);
		}
		else if (_tcscmp(pstrName, _T("normalimage")) == 0) SetNormalImage(pstrValue);
		else if (_tcscmp(pstrName, _T("hotimage")) == 0) SetHotImage(pstrValue);
		else CControlUI::SetAttribute(pstrName, pstrValue);		
	}

	void CAnalogStickUI::AdjustAngle(POINT & ptMove)
	{
		REAL fAngle = 0.0;
		LONG CenterX = m_rcItem.left + m_ptCenter.x;
		LONG CenterY = m_rcItem.top + m_ptCenter.y;
		LONG nXCoord = CenterX - ptMove.x;
		LONG nYCoord = CenterY - ptMove.y;
		ULONG Length = (nXCoord * nXCoord) + (nYCoord * nYCoord);
		REAL ptMoveK = sqrt(REAL(Length));
		REAL sinf = (REAL)abs(nYCoord) / ptMoveK;
		REAL fSlope = asinf(sinf);
		fAngle = fSlope * 180 / PI;
		//第一象限90-
		if (nXCoord <= 0 && nYCoord >= 0)
		{
			fAngle = 90.0 - fAngle;
		}
		//第二象限90+
		else if (nXCoord <= 0 && nYCoord <= 0)
		{
			fAngle = 90.0 + fAngle;
		}
		//第三象限270-
		else if (nXCoord >= 0 && nYCoord <= 0)
		{
			fAngle = 270.0 - fAngle;
		}
		//第四象限270+
		else if (nXCoord >= 0 && nYCoord >= 0)
		{
			fAngle = 270.0 + fAngle;
		}

		m_fAngle = fAngle;
		DUI__Trace(_T("%f"), m_fAngle);
		if (Length <= m_nRadius * m_nRadius)
		{
			m_ptCurrent.x = ptMove.x;
			m_ptCurrent.y = ptMove.y;
		}
		else
		{
			fAngle -= 90.0;
			m_ptCurrent.x = CenterX + LONG(m_nRadius*cos(fAngle*PI / 180));
			m_ptCurrent.y = CenterY + LONG(m_nRadius*sin(fAngle*PI / 180));
		}
	}

	void CAnalogStickUI::PaintStatusImage(HDC hDC)
	{
		if (m_ptCenter.x == 0 || m_ptCenter.y == 0)
			return;

		Gdiplus::Graphics graphics(hDC);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		Matrix m;
		PointF ptCenter(m_rcItem.left + m_ptCenter.x, m_rcItem.top + m_ptCenter.y);
		m.RotateAt(m_fAngle, ptCenter);
		graphics.SetTransform(&m);
		CUIString strImage = CPaintManagerUI::GetResourcePath();
		strImage += m_strNormalImage.c_str();
		Image image(strImage);
		Rect rcDst(m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top);
		RECT rcImgSrc = GetAnalogStickImageRect(0);
		Rect rcSrc(rcImgSrc.left, rcImgSrc.top, rcImgSrc.right - rcImgSrc.left, rcImgSrc.bottom - rcImgSrc.top);
		graphics.DrawImage(&image, rcDst, rcSrc.X, rcSrc.Y, rcSrc.Width, rcSrc.Height,Unit::UnitPixel);
		graphics.ResetTransform();
		Pen pen(Color::RosyBrown,2);
		INT cross = 4;
		graphics.DrawLine(&pen, Point(m_ptCurrent.x - cross, m_ptCurrent.y), Point(m_ptCurrent.x + cross, m_ptCurrent.y));
		graphics.DrawLine(&pen, Point(m_ptCurrent.x, m_ptCurrent.y - cross), Point(m_ptCurrent.x, m_ptCurrent.y + cross));
	}

	RECT CAnalogStickUI::GetAnalogStickImageRect(unsigned int step)
	{
		CRect rcSrc = {0};
		if (step >= m_nStep) return rcSrc;
		rcSrc = m_rcAnalogStickBase;
		//0 base
		unsigned int row = step / m_szRowcol.cy;
		unsigned int col = step % m_szRowcol.cy;
		rcSrc.MoveToXY(col*rcSrc.GetWidth(),row*rcSrc.GetHeight());
		return rcSrc;
	}

}