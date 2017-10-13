#include "StdAfx.h"
#include "UISpinButton.h"

static const float  PI = 3.1415926;

namespace DirectUI
{
	CSpinButtonUI::CSpinButtonUI()
		:m_nRadius(100),m_uButtonState(0),m_fAngle(0),m_nStickStep(0),m_nStep(0)
	{
		GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
		m_ptCenter.x = m_ptCenter.y = 0;
		m_ptCurrent.x = m_ptCurrent.y = 0;
		::memset(&m_gpCap,0,sizeof(_tagGearProp));
		m_gpCap.nRadius = 60;
		m_gpCap.nIterRadius = 0;
		m_gpCap.fAngleInit = 90.0f;
		::memset(&m_gpOutRing,0,sizeof(_tagGearProp));
		m_gpOutRing.nRadius = 80;
		m_gpOutRing.nIterRadius = 60;
	}

	CSpinButtonUI::~CSpinButtonUI()
	{
		try
		{
			GdiplusShutdown(m_gdiplusToken);
		}
		catch (...)
		{
			//throw "CSpinButtonUI::~CSpinButtonUI";
		}
	}

	LPCTSTR CSpinButtonUI::GetClass() const
	{
		return _T("SpinButtonUI");
	}

	UINT CSpinButtonUI::GetControlFlags() const
	{
		return (IsEnabled() ? UIFLAG_SETCURSOR : 0);
	}

	LPVOID CSpinButtonUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_SPINBUTTON) == 0)
			return static_cast<CSpinButtonUI*>(this);

		return CControlUI::GetInterface(pstrName);
	}

	LPCTSTR CSpinButtonUI::GetCapNormalImage() const
	{
		return m_strCapNormalImg;
	}

	void CSpinButtonUI::SetCapNormalImage(LPCTSTR pStrImage)
	{
		m_strCapNormalImg = pStrImage;
	}

	LPCTSTR CSpinButtonUI::GetCapHotImage() const
	{
		return m_strCapHotImg;
	}

	void CSpinButtonUI::SetCapHotImage(LPCTSTR pStrImage)
	{
		m_strCapHotImg = pStrImage;
	}

	LPCTSTR CSpinButtonUI::GetCapPushImage() const
	{
		return m_strCapPushImg;
	}

	void CSpinButtonUI::SetCapPushImage(LPCTSTR pStrImage)
	{
		m_strCapPushImg = pStrImage;
	}

	LPCTSTR CSpinButtonUI::GetCapDisableImage() const
	{
		return m_strCapDisableImg;
	}

	void CSpinButtonUI::SetCapDisableImage(LPCTSTR pStrImage)
	{
		m_strCapDisableImg = pStrImage;
	}

	int	CSpinButtonUI::GetRadius() const
	{
		return m_nRadius;
	}

	void CSpinButtonUI::SetRadius(LPCTSTR pStrRadius)
	{
		ASSERT(pStrRadius);
		m_nRadius = _ttoi(pStrRadius);
	}

	POINT CSpinButtonUI::GetCenterPoint() const
	{
		return m_ptCenter;
	}

	void CSpinButtonUI::SetCenterPoint(LPCTSTR pStrPoint)
	{
		ASSERT(pStrPoint);
		LPTSTR pstr = NULL;
		m_ptCenter.x = _tcstol(pStrPoint, &pstr, 10);  ASSERT(pstr);    
		m_ptCenter.y = _tcstol(pstr + 1, &pstr, 10);   ASSERT(pstr);   
	}


	SIZE CSpinButtonUI::EstimateSize(SIZE szAvailable)
	{
		if (m_cxyFixed.cy == 0) 
			return CSize(m_cxyFixed.cx, 8);

		return CControlUI::EstimateSize(szAvailable);
	}

	bool CSpinButtonUI::Activate()
	{
		if (!CControlUI::Activate()) return false;
		if (m_pManager) m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);

		return true;
	}

	void CSpinButtonUI::DoEvent(TEventUI& tevent)
	{
		if (!IsMouseEnabled() && 
			tevent.Type > UIEVENT__MOUSEBEGIN && 
			tevent.Type < UIEVENT__MOUSEEND) 
		{
			if (m_pParent) 
			{
				m_pParent->DoEvent(tevent);
			}
			return;
		}

		if (tevent.Type == UIEVENT_MOUSEMOVE)
		{
			if ((m_uButtonState & UISTATE_CAPTURED) != 0) 
			{
				AdjustAngle(tevent.ptMouse);
				Invalidate();
			}
			return;
		}

		if (tevent.Type == UIEVENT_BUTTONDOWN)
		{
			if (IsEnabled())
			{
				if (::PtInRect(&m_rcItem, tevent.ptMouse)) 
					m_uButtonState |= UISTATE_CAPTURED;
			}
			return;
		}

		if (tevent.Type == UIEVENT_BUTTONUP)
		{
			if (m_uButtonState & UISTATE_CAPTURED)
			{
				if (::PtInRect(&m_rcItem, tevent.ptMouse))
					Activate();

				m_uButtonState &= ~UISTATE_CAPTURED;
				Invalidate();
			}
			return;
		}

		if (tevent.Type == UIEVENT_CONTEXTMENU)
		{
			return;
		}

		if (tevent.Type == UIEVENT_KEYDOWN)
		{
			if (IsKeyboardEnabled()) 
			{
				if (tevent.chKey == VK_SPACE || tevent.chKey == VK_RETURN)  
				{
					Activate();
					return;
				}
			}
		}

		if (tevent.Type == UIEVENT_SETFOCUS) 
		{
			Invalidate();
		}
		if ( tevent.Type == UIEVENT_KILLFOCUS) 
		{
			Invalidate();
		}

		CControlUI::DoEvent(tevent);
	}

	void CSpinButtonUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("radius")) == 0) SetRadius(pstrValue);
		else if (_tcscmp(pstrName, _T("centerpoint")) == 0) SetCenterPoint(pstrValue);
		else if (_tcscmp(pstrName, _T("imgrowcol")) == 0)
		{
			LPTSTR pstr = NULL;
			SIZE sz = { 0 };
			sz.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			sz.cy = _tcstol(pstr + 1, &pstr, 10);   ASSERT(pstr);
		}
		else if (_tcscmp(pstrName, _T("imgrect")) == 0)
		{
			RECT rc = { 0 };
			LPTSTR pstr = NULL;
			rc.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			rc.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rc.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rc.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
		}
		else if (_tcscmp(pstrName, _T("capnormalimage")) == 0) SetCapNormalImage(pstrValue);
		else if (_tcscmp(pstrName, _T("caphotimage")) == 0) SetCapHotImage(pstrValue);
		else if (_tcscmp(pstrName, _T("cappushimage")) == 0) SetCapPushImage(pstrValue);
		else if (_tcscmp(pstrName, _T("capdisableimage")) == 0) SetCapDisableImage(pstrValue);
		else if (_tcscmp(pstrName, _T("ringnormalimage")) == 0) SetCapNormalImage(pstrValue);
		else if (_tcscmp(pstrName, _T("ringhotimage")) == 0) SetCapHotImage(pstrValue);
		else if (_tcscmp(pstrName, _T("ringpushimage")) == 0) SetCapPushImage(pstrValue);
		else if (_tcscmp(pstrName, _T("ringdisableimage")) == 0) SetCapDisableImage(pstrValue);
		else if (_tcscmp(pstrName, _T("calimage")) == 0) m_strCalibrationFrontImg = (pstrValue);
		else CControlUI::SetAttribute(pstrName, pstrValue);		
	}

	void CSpinButtonUI::AdjustAngle(POINT & ptMove)
	{
		REAL fAngle = 0.0;
		LONG CenterX = m_rcItem.left + m_ptCenter.x;
		LONG CenterY = m_rcItem.top + m_ptCenter.y;
		LONG nXCoord = CenterX - ptMove.x;
		LONG nYCoord = CenterY - ptMove.y;
		ULONG Length = (nXCoord * nXCoord) + (nYCoord * nYCoord);
		REAL ptRadius = sqrt(REAL(Length));
		REAL sinf = (REAL)abs(nYCoord) / ptRadius;
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
		//DUI__Trace(_T("%f"), m_fAngle);
		if (Length <= m_nRadius * m_nRadius)
		{
			m_ptCurrent.x = ptMove.x;
			m_ptCurrent.y = ptMove.y;
			m_nStickStep = (unsigned int)((float)m_nStep*(ptRadius/m_nRadius));
		}
		else
		{
			fAngle -= 90.0;
			m_ptCurrent.x = CenterX + LONG(m_nRadius*cos(fAngle*PI / 180));
			m_ptCurrent.y = CenterY + LONG(m_nRadius*sin(fAngle*PI / 180));
			m_nStickStep = m_nStep -1;
		}
		m_gpCap.fAngle = m_fAngle;
		//DUI__Trace(_T("%d"), m_nStickStep);
	}

	void CSpinButtonUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem)) return;
		Gdiplus::Graphics graphics(hDC);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		m_pGraphics.reset(&graphics);
		// 绘制循序：背景颜色->背景图->状态图->文本->边框
		if (m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0)
		{
			CRenderClip roundClip;
			CRenderClip::GenerateRoundClip(hDC, m_rcPaint,  m_rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
			PaintBkColor(hDC);
			PaintBkImage(hDC);
			PaintStatusImage(hDC);
			PaintText(hDC);
			PaintBorder(hDC);
		}
		else 
		{
			PaintBkColor(hDC);
			PaintBkImage(hDC);
			PaintStatusImage(hDC);
			PaintText(hDC);
			PaintBorder(hDC);
		}
		m_pGraphics.release();
	}

	void CSpinButtonUI::PaintStatusImage(HDC hDC)
	{
		if (m_ptCenter.x == 0 || m_ptCenter.y == 0)
			return;
		if(!m_pGraphics.get()) return;

		if( !IsEnabled() ){
			m_uButtonState |= UISTATE_DISABLED;
			m_uOutButtonState |= UISTATE_DISABLED;
		}
		else {
			m_uButtonState &= ~ UISTATE_DISABLED;
			m_uOutButtonState &= ~ UISTATE_DISABLED;
		}

		std::unique_ptr<Gdiplus::Bitmap> uniImg;

		auto PaintImg=[&](CUIString& drawImg, _tagGearProp* prop)->void
		{
			CUIString strImg;
			CUIString strPathImage = CPaintManagerUI::GetResourcePath();
			GetImageProp(drawImg, &strImg,  NULL, NULL);
			strPathImage += strImg;
			uniImg.reset(new Gdiplus::Bitmap(strPathImage.GetData()));
			if(!strImg.IsEmpty() && uniImg.get()){
				Matrix m;
				Rect rcDst,rcSrc;
				PointF ptCenter(m_rcItem.left + m_ptCenter.x, m_rcItem.top + m_ptCenter.y);
				GetImageProp(drawImg, NULL,  &rcSrc, &rcDst);
				if(!rcDst.IsEmptyArea())
				{
					rcDst.Offset(m_rcItem.left,m_rcItem.top);
					ptCenter.X = rcDst.X + rcDst.Width/2;
					ptCenter.Y = rcDst.Y + rcDst.Height/2;
				}
				else
				{
					rcDst.X = m_rcItem.left;
					rcDst.Y = m_rcItem.top;
					rcDst.Width = m_rcItem.right - m_rcItem.left;
					rcDst.Height = m_rcItem.bottom - m_rcItem.top;
				}
				if(rcSrc.IsEmptyArea())
				{
					rcSrc.Width = uniImg->GetWidth();
					rcSrc.Height = uniImg->GetHeight();
				}
				if(prop){
					m.RotateAt((*prop).fAngle + (*prop).fAngleInit, ptCenter);
					m_pGraphics->SetTransform(&m);
				}
				m_pGraphics->DrawImage(uniImg.get(), rcDst, rcSrc.X, rcSrc.Y, rcSrc.Width, rcSrc.Height,UnitPixel);
				if(prop){
					m_pGraphics->ResetTransform();
				}
			}
			else
			{
				drawImg.Empty();
			}
		};

		//绘制得到焦点图片	
		if(!m_strFocusImg.IsEmpty() && IsFocused()){
			PaintImg(m_strFocusImg,NULL);
		}

		//绘制刻度的背景图片
		if(!m_strCalibrationBackImg.IsEmpty()){
			PaintImg(m_strCalibrationBackImg,NULL);
		}

		//绘制刻度前景图片,该图片随角度裁剪
		if(!m_strCalibrationFrontImg.IsEmpty()){
			CUIString strImg;
			CUIString strPathImage = CPaintManagerUI::GetResourcePath();
			GetImageProp(m_strCalibrationFrontImg, &strImg,  NULL, NULL);
			strPathImage += strImg;
			uniImg.reset(new Gdiplus::Bitmap(strPathImage.GetData()));

			if(!strImg.IsEmpty() && uniImg.get()){
				Rect rcDst,rcSrc;
				PointF ptCenter(m_rcItem.left + m_ptCenter.x, m_rcItem.top + m_ptCenter.y);
				GetImageProp(m_strCalibrationFrontImg, NULL,  &rcSrc, &rcDst);
				if(!rcDst.IsEmptyArea())
				{
					rcDst.Offset(m_rcItem.left,m_rcItem.top);
					ptCenter.X = rcDst.X + rcDst.Width/2;
					ptCenter.Y = rcDst.Y + rcDst.Height/2;
				}
				else
				{
					rcDst.X = m_rcItem.left;
					rcDst.Y = m_rcItem.top;
					rcDst.Width = m_rcItem.right - m_rcItem.left;
					rcDst.Height = m_rcItem.bottom - m_rcItem.top;
				}
				if(rcSrc.IsEmptyArea())
				{
					rcSrc.Width = uniImg->GetWidth();
					rcSrc.Height = uniImg->GetHeight();
				}

				GraphicsPath pathPie;
				pathPie.AddPie(rcDst,-90,m_gpCap.fAngle);
				Region rgnOld;
				m_pGraphics->GetClip(&rgnOld);
				m_pGraphics->SetClip(&pathPie);
				m_pGraphics->DrawImage(uniImg.get(), rcDst, rcSrc.X, rcSrc.Y, rcSrc.Width, rcSrc.Height,UnitPixel);
				m_pGraphics->SetClip(&rgnOld);
			}
			else
				m_strCalibrationFrontImg.Empty();
		}

		//绘制外轮选转按钮(主按钮外边按钮)的图片
		if(!m_strRingNormalImg.IsEmpty()){
			CUIString* pstrImg = NULL;
			if(m_uOutButtonState & UISTATE_HOT)
				pstrImg = &m_strRingHotImg;
			else if(m_uOutButtonState & UISTATE_PUSHED)
				pstrImg = &m_strRingPushImg;
			else if(m_uOutButtonState & UISTATE_DISABLED)
				pstrImg = &m_strRingDisableImg;
			else
				pstrImg = &m_strRingNormalImg;

			PaintImg(*pstrImg,&m_gpOutRing);
		}

		//绘制主选转按钮的状态图片
		if(!m_strCapNormalImg.IsEmpty()){
			CUIString* pstrImg =  NULL;
			if(m_uButtonState & UISTATE_HOT)
				pstrImg = &m_strCapHotImg;
			else if(m_uButtonState & UISTATE_PUSHED)
				pstrImg = &m_strCapPushImg;
			else if(m_uButtonState & UISTATE_DISABLED)
				pstrImg = &m_strCapDisableImg;
			else
				pstrImg = &m_strCapNormalImg;

			PaintImg(*pstrImg,&m_gpCap);
		}

		//绘制前景图片	
		if(!m_strFrontImg.IsEmpty()){
			PaintImg(m_strFrontImg,NULL);
		}


		// 绘制鼠标位置的十字标示
		INT cross = 5;
		Pen pen(Color::AliceBlue,1);
		m_pGraphics->DrawLine(&pen, Point(m_ptCurrent.x - cross, m_ptCurrent.y), Point(m_ptCurrent.x + cross, m_ptCurrent.y));
		m_pGraphics->DrawLine(&pen, Point(m_ptCurrent.x, m_ptCurrent.y - cross), Point(m_ptCurrent.x, m_ptCurrent.y + cross));
	}

	void CSpinButtonUI::PaintText(HDC hDC){
	}

	void CSpinButtonUI::GetImageProp(const CUIString& strImg, CUIString* pstrFile, Gdiplus::Rect* prcSrc, Gdiplus::Rect* prcDest)
	{

		int chPos = 0,chEndPos = 0;
		if(pstrFile)
		{
			pstrFile->Empty();
			if( (chPos = strImg.Find(_T("file='"))) != -1)// file=''
			{
				chPos += 6; //len of  file='
				chEndPos = strImg.Find(_T('\''),chPos);
				pstrFile->Assign(strImg.GetData()+chPos,chEndPos);
			}
			else
			{
				*pstrFile = strImg;
			}
		}

		if(prcSrc)
		{
			if( (chPos = strImg.Find(_T("source='"))) != -1)// source=''
			{
				RECT rc = {0};
				TCHAR rcStr[MAX_PATH] = {0};
				chPos += 8; //len of  source='
				chEndPos = strImg.Find(_T('\''),chPos);
				_tcsncpy_s(rcStr,strImg.GetData()+chPos,chEndPos);
				if(_stscanf_s(rcStr,_T("%d,%d,%d,%d"), &rc.left, &rc.top, &rc.right, &rc.bottom) == 4)
				{
					prcSrc->X = rc.left;
					prcSrc->Y = rc.top;
					prcSrc->Width = rc.right;
					prcSrc->Height = rc.bottom;
				}
			}
		}

		if(prcDest)
		{
			if( (chPos = strImg.Find(_T("dest='"))) != -1)// dest=''
			{
				RECT rc = {0};
				TCHAR rcStr[MAX_PATH] = {0};
				chPos += 6; //len of  dest='
				chEndPos = strImg.Find(_T('\''),chPos);
				_tcsncpy_s(rcStr,strImg.GetData()+chPos,chEndPos);
				if(_stscanf_s(rcStr,_T("%d,%d,%d,%d"), &rc.left, &rc.top, &rc.right, &rc.bottom) == 4)
				{
					prcDest->X = rc.left;
					prcDest->Y = rc.top;
					prcDest->Width = rc.right;
					prcDest->Height = rc.bottom;
				}
			}
		}

	}

}