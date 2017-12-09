#include "StdAfx.h"
#include "UISpinButton.h"

static const float  PI = 3.1415926;

namespace DirectUI
{
	CSpinButtonUI::CSpinButtonUI()
		:m_uButtonState(0),m_uOutButtonState(0)
	{
		GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
		m_ptCenter.x = m_ptCenter.y = 0;
		m_ptCurrent.x = m_ptCurrent.y = 0;
		::memset(&m_gpCap,0,sizeof(_tagGearProp) - sizeof(CAngleRecord));
		m_gpCap.fMax = 360.0f;
		SetCapRadius(60);
		::memset(&m_gpOutRing,0,sizeof(_tagGearProp) - sizeof(CAngleRecord));
		m_gpOutRing.fMax = 360.0f;
		SetRingInnerRadius(60);
		SetRingRadius(80);
	}

	CSpinButtonUI::~CSpinButtonUI()
	{
		try
		{
			if(m_gpCap.hRgn)
			{
				::DeleteObject(m_gpCap.hRgn);
				m_gpCap.hRgn = NULL;
			}
			if(m_gpOutRing.hRgn)
			{
				::DeleteObject(m_gpOutRing.hRgn);
				m_gpOutRing.hRgn = NULL;
			}
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
			BOOL bInvalidate = FALSE;
			if (m_uButtonState & UISTATE_CAPTURED) 
			{
				AdjustAngle(tevent.ptMouse, m_gpCap, FALSE);
				bInvalidate = TRUE;
			}
			else if (m_uOutButtonState & UISTATE_CAPTURED) 
			{
				AdjustAngle(tevent.ptMouse, m_gpOutRing, FALSE);
				bInvalidate = TRUE;
			}
			else
			{
				auto bInCap = PtMouseInRegion(m_gpCap.hRgn, tevent.ptMouse, m_gpCap);
				auto bInRing = bInCap ? false : PtMouseInRegion(m_gpOutRing.hRgn, tevent.ptMouse, m_gpOutRing);

				if(bInCap && !(m_uButtonState & UISTATE_HOT))
				{
					m_uButtonState |= UISTATE_HOT;
					bInvalidate = TRUE;
				}
				else if(!bInCap && (m_uButtonState & UISTATE_HOT))
				{
					m_uButtonState &= ~UISTATE_HOT;
					bInvalidate = TRUE;
				}

				if(bInRing && !(m_uOutButtonState & UISTATE_HOT))
				{
					m_uOutButtonState |= UISTATE_HOT;
					bInvalidate = TRUE;
				}
				else if(!bInRing && (m_uOutButtonState & UISTATE_HOT))
				{
					m_uOutButtonState &= ~UISTATE_HOT;
					bInvalidate = TRUE;
				}
			} //not UISTATE_CAPTURED
			if(bInvalidate) Invalidate();
			return;
		}

		if (tevent.Type == UIEVENT_BUTTONDOWN)
		{
			if (IsEnabled())
			{
				if(PtMouseInRegion(m_gpCap.hRgn, tevent.ptMouse, m_gpCap))
				{
					m_uButtonState |= UISTATE_CAPTURED;
					m_uButtonState |= UISTATE_PUSHED;
					AdjustAngle(tevent.ptMouse, m_gpCap, TRUE);
					Invalidate();
				}
				else if(PtMouseInRegion(m_gpOutRing.hRgn, tevent.ptMouse, m_gpOutRing))
				{
					m_uOutButtonState |= UISTATE_CAPTURED;
					m_uOutButtonState |= UISTATE_PUSHED;
					AdjustAngle(tevent.ptMouse, m_gpOutRing, TRUE);
					Invalidate();
				}
			}
			return;
		}

		if (tevent.Type == UIEVENT_BUTTONUP)
		{
			if (m_uButtonState & UISTATE_CAPTURED)
			{
				if(PtMouseInRegion(m_gpCap.hRgn, tevent.ptMouse, m_gpCap))
					Activate();

				m_uButtonState &= ~(UISTATE_CAPTURED | UISTATE_PUSHED);
				Invalidate();
			}
			else if (m_uOutButtonState & UISTATE_CAPTURED)
			{
				if(PtMouseInRegion(m_gpOutRing.hRgn, tevent.ptMouse, m_gpOutRing))
					Activate();

				m_uOutButtonState &= ~(UISTATE_CAPTURED | UISTATE_PUSHED);
				Invalidate();
			}
			return;
		}

		if( tevent.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				m_uButtonState &= ~UISTATE_HOT;
				m_uOutButtonState &= ~UISTATE_HOT;
			}
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
		if (_tcscmp(pstrName, _T("radius")) == 0) SetCapRadius(_ttoi(pstrValue));
		if (_tcscmp(pstrName, _T("radiusring")) == 0) SetRingRadius(_ttoi(pstrValue));
		if (_tcscmp(pstrName, _T("radiusringinner")) == 0) SetRingInnerRadius(_ttoi(pstrValue));
		//else if (_tcscmp(pstrName, _T("centerpoint")) == 0) SetCenterPoint(pstrValue);
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
		else if (_tcscmp(pstrName, _T("ringnormalimage")) == 0) SetRingNormalImage(pstrValue);
		else if (_tcscmp(pstrName, _T("ringhotimage")) == 0) SetRingHotImage(pstrValue);
		else if (_tcscmp(pstrName, _T("ringpushimage")) == 0) SetRingpPushImage(pstrValue);
		else if (_tcscmp(pstrName, _T("ringdisableimage")) == 0) SetRingDisableImage(pstrValue);
		else if (_tcscmp(pstrName, _T("calfrontimage")) == 0) SetCalibrationFrontImage(pstrValue);
		else if (_tcscmp(pstrName, _T("calbackimage")) == 0) SetCalibrationBackImage(pstrValue);
		else if (_tcscmp(pstrName, _T("frontimage")) == 0) SetFrontImage(pstrValue);
		else if (_tcscmp(pstrName, _T("focusimage")) == 0) SetFocusImage(pstrValue);
		else CControlUI::SetAttribute(pstrName, pstrValue);		
	}

	void CSpinButtonUI::SetPos(RECT rc)
	{
		m_ptCenter.x = ((rc.right - rc.left) / 2);
		m_ptCenter.y = ((rc.bottom - rc.top) / 2);
		__super::SetPos(rc);
	}

	CAngleRecord::CAngleRecord()
		:m_nQuadrant(0),m_b4to1(FALSE),m_b1to4(FALSE),m_fOffsetAngle(0),
		m_bClockwise(FALSE),m_fLastAngle(0),m_fFirstAngle(0)
	{

	}

	float CAngleRecord::CalculateAngle(
		float* pfRoll, /*转动过的角度, + 顺时针 -逆时针, 表示转过的圈数(360为一圈)*/
		const POINT ptMove, /*当前可移动点的位置*/
		const POINT ptCenter, /*圆心点的位置*/
		ULONG* pulDistance, /*可移动点与圆心点的距离*/
		BOOL bFirst)
	{
		float fAngle = 0.0;
		LONG nXCoord = ptCenter.x - ptMove.x;
		LONG nYCoord = ptCenter.y - ptMove.y;
		float ptRadius = sqrt(float((nXCoord * nXCoord) + (nYCoord * nYCoord))); //两点距离
		if(pulDistance) *pulDistance = (ULONG)ptRadius;
		float sinf = (float)abs(nYCoord) / ptRadius;
		float fSlope = asinf(sinf); 
		fAngle = fSlope * 180 / PI;//圆心对Y方向的角度

		//取以Y轴正方向开始转动的角度
		//笛卡尔坐标系(Cartesian coordinates)
		//第一象限90-
		if (nXCoord <= 0 && nYCoord >= 0)
		{
			if(m_nQuadrant == 4) m_b4to1 = TRUE;
			fAngle = 90.0 - fAngle;
			m_nQuadrant = 1;
		}
		//第二象限90+
		else if (nXCoord <= 0 && nYCoord <= 0)
		{
			fAngle = 90.0 + fAngle;
			m_nQuadrant = 2;
		}
		//第三象限270-
		else if (nXCoord >= 0 && nYCoord <= 0)
		{
			fAngle = 270.0 - fAngle;
			m_nQuadrant = 3;
		}
		//第四象限270+
		else if (nXCoord >= 0 && nYCoord >= 0)
		{
			if(m_nQuadrant == 1) m_b1to4 = TRUE;
			fAngle = 270.0 + fAngle;
			m_nQuadrant = 4;
		}

		if(bFirst)
		{
			m_fFirstAngle = fAngle;
			m_fLastAngle = fAngle;
			m_bClockwise = FALSE;
			m_fOffsetAngle = 0;
			//DUI__Trace(_T("FirstAngle----     %f"), m_fFirstAngle);
		}

		if(m_b1to4) m_fOffsetAngle -= 360.0;
		if(m_b4to1) m_fOffsetAngle += 360.0;

		float fLastAngle = m_fLastAngle; //fmodf(m_fLastAngle, 360.0);
		float fVar = fAngle + m_fOffsetAngle  - fLastAngle; //旋转角度
		m_bClockwise = fVar > 0; //旋转方向

		m_fLastAngle += fVar;

		//DUI__Trace(_T("NewLastAngle----   %f  OldLastAngle---- %f  fVar----  %f"), m_fLastAngle, fLastAngle, fVar);
		if(pfRoll) *pfRoll = *pfRoll + fVar; //实际改变增量值
		m_b4to1 = m_b1to4 = FALSE;
		return fAngle;
	}

	void CSpinButtonUI::AdjustAngle(POINT & ptMove, _tagGearProp& gp, BOOL isbegin)
	{
		REAL fAngle = 0.0;
		POINT ptCenter = gp.ptCenter; /*{m_rcItem.left + m_ptCenter.x, m_rcItem.top + m_ptCenter.y};*/
		ULONG Length = 0;
		if(isbegin)
			fAngle = gp.angle.CalculateAngle(NULL, ptMove, ptCenter, &Length, isbegin);
		else
			fAngle = gp.angle.CalculateAngle(&gp.fAngle, ptMove, ptCenter, &Length, isbegin);
		DUI__Trace(_T("GearPropAngle----     %f"), gp.fAngle);
		if (Length <= gp.nRadius)
		{
			m_ptCurrent.x = ptMove.x;
			m_ptCurrent.y = ptMove.y;
		}
		else
		{
			fAngle -= 90.0;
			m_ptCurrent.x = ptCenter.x + LONG(gp.nRadius*cos(fAngle*PI / 180));
			m_ptCurrent.y = ptCenter.y + LONG(gp.nRadius*sin(fAngle*PI / 180));
		}
	}

	bool CSpinButtonUI::PtMouseInRegion(HRGN& hRgn, const POINT& ptMouse, _tagGearProp& gp)
	{
		CPoint ptCurr(ptMouse.x-m_rcItem.left, ptMouse.y - m_rcItem.top);
		ptCurr.x -= (m_ptCenter.x - gp.nRadius);
		ptCurr.y -= (m_ptCenter.y - gp.nRadius);
		if(::PtInRegion(gp.hRgn, ptCurr.x, ptCurr.y))
		{
			return true;
		}
		return false;
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
			int nAlpha = 255;
			CUIString strPathImage = CPaintManagerUI::GetResourcePath();
			GetImageProp(drawImg, &strImg,  NULL, NULL, &nAlpha);
			strPathImage += strImg;
			uniImg.reset(new Gdiplus::Bitmap(strPathImage.GetData()));
			if(!strImg.IsEmpty() && uniImg.get()){
				Matrix m;
				ImageAttributes imageAtt;  
				ImageAttributes* pAttributes = NULL;
				Rect rcDst,rcSrc;
				PointF ptCenter(m_rcItem.left + m_ptCenter.x, m_rcItem.top + m_ptCenter.y);
				GetImageProp(drawImg, NULL,  &rcSrc, &rcDst, NULL);
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
					prop->ptCenter.x = (LONG)ptCenter.X;
					prop->ptCenter.y = (LONG)ptCenter.Y;
					m.RotateAt((*prop).fAngle + (*prop).fAngleInit, ptCenter);
					m_pGraphics->SetTransform(&m);
				}
				if(nAlpha != 255)
				{
					REAL m[][5] = {{1,0,0,0,0}, {0,1,0,0,0}, {0,0,1,0,0}, {0,0,0,(float)nAlpha/255,0}, {0,0,0,0,1} };  
					imageAtt.SetColorMatrix((ColorMatrix*)&m, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap); 
					pAttributes = &imageAtt;
				}
				m_pGraphics->DrawImage(uniImg.get(), rcDst, rcSrc.X, rcSrc.Y, rcSrc.Width, rcSrc.Height,UnitPixel,pAttributes);
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
			int nAlpha = 255;
			CUIString strImg;
			CUIString strPathImage = CPaintManagerUI::GetResourcePath();
			GetImageProp(m_strCalibrationFrontImg, &strImg,  NULL, NULL, &nAlpha);
			strPathImage += strImg;
			uniImg.reset(new Gdiplus::Bitmap(strPathImage.GetData()));

			if(!strImg.IsEmpty() && uniImg.get()){
				ImageAttributes imageAtt;  
				ImageAttributes* pAttributes = NULL;
				Rect rcDst,rcSrc;
				PointF ptCenter(m_rcItem.left + m_ptCenter.x, m_rcItem.top + m_ptCenter.y);
				GetImageProp(m_strCalibrationFrontImg, NULL,  &rcSrc, &rcDst, NULL);
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
				if(nAlpha != 255)
				{
					REAL m[][5] = {{1,0,0,0,0}, {0,1,0,0,0}, {0,0,1,0,0}, {0,0,0,(float)nAlpha/255,0}, {0,0,0,0,1} };  
					imageAtt.SetColorMatrix((ColorMatrix*)&m, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap); 
					pAttributes = &imageAtt;
				}
				GraphicsPath pathPie;
				pathPie.AddPie(rcDst,-90,m_gpCap.fAngle);
				Region rgnOld;
				m_pGraphics->GetClip(&rgnOld);
				m_pGraphics->SetClip(&pathPie);
				m_pGraphics->DrawImage(uniImg.get(), rcDst, rcSrc.X, rcSrc.Y, rcSrc.Width, rcSrc.Height,UnitPixel, pAttributes);
				m_pGraphics->SetClip(&rgnOld);
			}
			else
				m_strCalibrationFrontImg.Empty();
		}

		//绘制外轮选转按钮(主按钮外边按钮)的图片
		if(!m_strRingNormalImg.IsEmpty()){
			CUIString* pstrImg = NULL;
			if(m_uOutButtonState & UISTATE_DISABLED)
				pstrImg = &m_strRingDisableImg;
			else if(m_uOutButtonState & UISTATE_PUSHED)
				pstrImg = &m_strRingPushImg;
			else if(m_uOutButtonState & UISTATE_HOT)
				pstrImg = &m_strRingHotImg;
			else
				pstrImg = &m_strRingNormalImg;

			PaintImg(*pstrImg,&m_gpOutRing);
		}

		//绘制主选转按钮的状态图片
		if(!m_strCapNormalImg.IsEmpty()){
			CUIString* pstrImg =  NULL;
			if(m_uButtonState & UISTATE_DISABLED)
				pstrImg = &m_strCapDisableImg;
			else if(m_uButtonState & UISTATE_PUSHED)
				pstrImg = &m_strCapPushImg;
			else if(m_uButtonState & UISTATE_HOT)
				pstrImg = &m_strCapHotImg;
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

	void CSpinButtonUI::GetImageProp(const CUIString& strImg, CUIString* pstrFile, Gdiplus::Rect* prcSrc, Gdiplus::Rect* prcDest, int* pAlpha)
	{

		int chPos = 0,chEndPos = 0;
		if(pstrFile)
		{
			pstrFile->Empty();
			if( (chPos = strImg.Find(_T("file='"))) != -1)// file=''
			{
				chPos += 6; //len of  file='
				chEndPos = strImg.Find(_T('\''),chPos);
				pstrFile->Assign(strImg.GetData()+chPos,chEndPos - chPos);
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
				_tcsncpy_s(rcStr,strImg.GetData()+chPos,chEndPos - chPos);
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
				_tcsncpy_s(rcStr,strImg.GetData()+chPos,chEndPos - chPos);
				if(_stscanf_s(rcStr,_T("%d,%d,%d,%d"), &rc.left, &rc.top, &rc.right, &rc.bottom) == 4)
				{
					prcDest->X = rc.left;
					prcDest->Y = rc.top;
					prcDest->Width = rc.right;
					prcDest->Height = rc.bottom;
				}
			}
		}

		if(pAlpha)
		{
			if( (chPos = strImg.Find(_T("fade='"))) != -1)// fade=''
			{
				RECT rc = {0};
				TCHAR rcStr[MAX_PATH] = {0};
				chPos += 6; //len of  fade='
				chEndPos = strImg.Find(_T('\''),chPos);
				_tcsncpy_s(rcStr,strImg.GetData()+chPos,chEndPos - chPos);
				if(_stscanf_s(rcStr,_T("%d"), pAlpha) == 1)
				{
				}
			}

		}
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

	LPCTSTR CSpinButtonUI::GetRingNormalImage() const
	{
		return m_strRingNormalImg;
	}

	void CSpinButtonUI::SetRingNormalImage(LPCTSTR pStrImage)
	{
		m_strRingNormalImg = pStrImage;
	}

	LPCTSTR CSpinButtonUI::GetRingHotImage() const
	{
		return m_strRingHotImg;
	}

	void CSpinButtonUI::SetRingHotImage(LPCTSTR pStrImage)
	{
		m_strRingHotImg = pStrImage;
	}

	LPCTSTR CSpinButtonUI::GetRingPushImage() const
	{
		return m_strRingPushImg;
	}

	void CSpinButtonUI::SetRingpPushImage(LPCTSTR pStrImage)
	{
		m_strRingPushImg = pStrImage;
	}

	LPCTSTR CSpinButtonUI::GetRingDisableImage() const
	{
		return m_strRingDisableImg;
	}

	void CSpinButtonUI::SetRingDisableImage(LPCTSTR pStrImage)
	{
		m_strRingDisableImg = pStrImage;
	}

	LPCTSTR CSpinButtonUI::GetCalibrationFrontImage() const
	{
		return m_strCalibrationFrontImg;
	}

	void CSpinButtonUI::SetCalibrationFrontImage(LPCTSTR pStrImage)
	{
		m_strCalibrationFrontImg = pStrImage;
	}

	LPCTSTR CSpinButtonUI::GetCalibrationBackImage() const
	{
		return m_strCalibrationBackImg;
	}

	void CSpinButtonUI::SetCalibrationBackImage(LPCTSTR pStrImage)
	{
		m_strCalibrationBackImg = pStrImage;
	}

	LPCTSTR CSpinButtonUI::GetFocusImage() const
	{
		return m_strFocusImg;
	}

	void CSpinButtonUI::SetFocusImage(LPCTSTR pStrImage)
	{
		m_strFocusImg = pStrImage;
	}

	LPCTSTR CSpinButtonUI::GetFrontImage() const
	{
		return m_strFrontImg;
	}

	void CSpinButtonUI::SetFrontImage(LPCTSTR pStrImage)
	{
		m_strFrontImg = pStrImage;
	}

	int	CSpinButtonUI::GetCapRadius() const
	{
		return m_gpCap.nRadius;
	}

	void CSpinButtonUI::SetCapRadius(UINT nRadius)
	{
		if(m_gpCap.hRgn)
		{
			::DeleteObject(m_gpCap.hRgn);
			m_gpCap.hRgn = NULL;
		}
		m_gpCap.nRadius = nRadius;
		if(!m_gpCap.hRgn)
		{
			m_gpCap.hRgn = ::CreateEllipticRgn(0,0,nRadius<<1,nRadius<<1);
		}
	}

	int	CSpinButtonUI::GetRingRadius() const
	{
		return m_gpOutRing.nRadius;
	}

	void CSpinButtonUI::SetRingRadius(UINT nRadius)
	{
		if(m_gpOutRing.hRgn)
		{
			::DeleteObject(m_gpOutRing.hRgn);
			m_gpOutRing.hRgn = NULL;
		}
		m_gpOutRing.nRadius = nRadius;
		if(!m_gpOutRing.hRgn)
		{
			int x = nRadius<<1;
			int x1 = nRadius - m_gpOutRing.nInnerRadius;
			int x2 = x - x1;
			m_gpOutRing.hRgn = ::CreateEllipticRgn(0,0,x,x);
			HRGN hRgn = ::CreateEllipticRgn(x1,x1,x2,x2);
			::CombineRgn(m_gpOutRing.hRgn,m_gpOutRing.hRgn,hRgn,RGN_XOR);
			::DeleteObject(hRgn);
		}
	}

	int	CSpinButtonUI::GetRingInnerRadius() const
	{
		return m_gpOutRing.nInnerRadius;
	}

	void CSpinButtonUI::SetRingInnerRadius(UINT nRadius)
	{
		m_gpOutRing.nInnerRadius = nRadius;
	}

	POINT CSpinButtonUI::GetCenterPoint() const
	{
		return m_ptCenter;
	}

	void CSpinButtonUI::SetCapMax(float fValue)
	{
		if(fValue < m_gpCap.fMin)
			fValue = m_gpCap.fMin;
		m_gpCap.fMax = fValue;
	}

	float CSpinButtonUI::GetCapMax() const
	{
		return m_gpCap.fMax;
	}

	void CSpinButtonUI::SetRingMax(float fValue)
	{
		if(fValue < m_gpOutRing.fMin )
			fValue = m_gpOutRing.fMin;
		m_gpOutRing.fMax = fValue;
	}

	float CSpinButtonUI::GetRingMax() const
	{
		return m_gpOutRing.fMax;
	}

	void CSpinButtonUI::SetCapMin(float fValue)
	{
		if(fValue > m_gpCap.fMax)
			fValue = m_gpCap.fMax;
		m_gpCap.fMin = fValue;
	}

	float CSpinButtonUI::GetCapMin() const	
	{
		return m_gpCap.fMin;
	}

	void CSpinButtonUI::SetRingMin(float fValue)
	{
		if(fValue > m_gpOutRing.fMax)
			fValue = m_gpOutRing.fMax;
		m_gpOutRing.fMin = fValue;
	}

	float CSpinButtonUI::GetRingMin() const
	{
		return m_gpOutRing.fMin;
	}

	void CSpinButtonUI::SetCapSpace(float fValue)
	{
		if(fValue > 0) m_gpCap.fSpace = fValue;
	}

	float CSpinButtonUI::GetCapSpace() const
	{
		return m_gpCap.fSpace;
	}

	void CSpinButtonUI::SetRingSpace(float fValue)
	{
		if(fValue > 0) m_gpOutRing.fSpace = fValue;
	}

	float CSpinButtonUI::GetRingSpace() const
	{
		return m_gpOutRing.fSpace;
	}

}/////namespace DirectUI