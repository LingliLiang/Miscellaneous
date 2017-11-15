#include "stdafx.h"
#include "ColorPanel.h"
namespace NSCGUI
{

#define HSVCOLOR_NOTIFY_COLOR_SELECT _T("hsv_select_color")
#define COLORCHUNK_NOTIFY_COLOR_PRESSED _T("chunk_color_pressed")

	/**
	*CColorChunkUI
	*/
	CColorChunkUI::CColorChunkUI()
	{
		m_pOwner = NULL;
		m_bSelect = false;
		m_bHot = false;
	}

	LPVOID CColorChunkUI::GetInterface(LPCTSTR pstrName)
	{
		if(_tcscmp(pstrName, _T("ColorChunk")) == 0)
			return this;

		return __super::GetInterface(pstrName);
	}

	void CColorChunkUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		RECT rcTemp = {0};
		if(!::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) return;

		__super::DoPaint(hDC, rcPaint);

		if(m_bHot)
		{
			DirectUI::CRect rcHot = m_rcItem;
			CRenderEngine::DrawRect(hDC, rcHot, 1, RGB(0xFF, 0xFF, 0xFF));
		}
	}

	void CColorChunkUI::DoEvent(TEventUI& event)
	{
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
		{
			if(m_pOwner)
			{
				TNotifyUI notify;
				notify.sType = COLORCHUNK_NOTIFY_COLOR_PRESSED;
				notify.wParam = 0;
				notify.lParam = 0;
				notify.pSender = this;
				notify.ptMouse = event.ptMouse;
				m_pOwner->Notify(notify);
			}
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{

			m_bHot = true;
			Invalidate();
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			m_bHot = false;
			Invalidate();
		}

		__super::DoEvent(event);
	}

	void CColorChunkUI::SetOwner(INotifyUI * pOwner)
	{
		m_pOwner = pOwner;
	}

	void CColorChunkUI::Select(bool bSelect)
	{
		m_bSelect = bSelect;
	}

	bool CColorChunkUI::IsSelected()
	{
		return m_bSelect;
	}

	COLORREF CColorChunkUI::GetColor()
	{
		return m_dwBackColor;
	}

	void CColorChunkUI::SetDwColor(DWORD color)
	{
		m_dwBackColor = color;
		this->Invalidate();
	}

	void CColorChunkUI::SetColor(COLORREF color)
	{
		DWORD dwValue = 0xFF000000;
		BYTE r = GetRValue(color);
		BYTE g = GetGValue(color);
		BYTE b = GetBValue(color);
		dwValue |= RGB(b,g,r);
		m_dwBackColor = dwValue;
		this->Invalidate();
	}


	/**
	*CHSVContainerUI
	*/
	CHSVContainerUI::CHSVContainerUI()
	{
		m_bInit = false;
		m_bLBDown = false;
		m_uiWidth = 0;
		m_uiHeight = 0;
		m_dHue = 1.0;
		m_dAValue = 0.0;
		m_dBValue = 0.0;
		m_selColor = 0;
		m_strImageSelecter = _T("");
		m_nSelWidth = 0;
		m_nSelHeight = 0;
		m_ptSelect.x = 0;
		m_ptSelect.y = 0;
	}

	void CHSVContainerUI::Init()
	{
		if(!m_bInit)
		{
			m_uiWidth = m_cxyFixed.cx - m_rcInset.left - m_rcInset.right;
			m_uiHeight = m_cxyFixed.cy - m_rcInset.top - m_rcInset.bottom;
			m_rcColor = m_rcItem;
			m_rcColor.Deflate( &m_rcInset );
			m_bInit = true;
		}
	}

	void CHSVContainerUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("selecterimage")) == 0) SetSelecterImage(pstrValue,0,0);
		else if (_tcscmp(pstrName, _T("selectersize")) == 0) 
		{
			LPTSTR pstr = NULL;
			int width = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			int height = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			SetSelecterImage(_T(""),width,height);
		}
		__super::SetAttribute(pstrName, pstrValue);
	}

	void CHSVContainerUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		__super::DoPaint(hDC, rcPaint);

		m_rcColor = m_rcItem;
		m_rcColor.Deflate( &m_rcInset );

		PaintColorRect(hDC, rcPaint, m_rcColor,m_dHue);
		//PaintColorRect2(hDC, rcPaint, m_rcColor,RGB(255,0,0));
		PaintColorSelecter(hDC);
		for(int i = 0;i < m_items.GetSize();i++)
		{
			GetItemAt(i)->DoPaint(hDC, rcPaint);
		}

	}

	COLORREF CHSVContainerUI::hsvToRgb(double h, double s, double v)
	{
		if (h >= 1) h -= 1;
		if (h < 0) h += 1;
		if (s == 0)
			return RGB(v * 255, v * 255, v * 255);
		h *= 6;
		int i = (int)h;
		double f = h - i;
		double p = v * (1 - s);
		double q = v * (1 - s * f);
		double t = v * (1 - s * (1 - f));
		switch (i) {
		case 0:
			return RGB(v * 255, t * 255, p * 255);
		case 1:
			return RGB(q * 255, v * 255, p * 255);
		case 2:
			return RGB(p * 255, v * 255, t * 255);
		case 3:
			return RGB(p * 255, q * 255, v * 255);
		case 4:
			return RGB(t * 255, p * 255, v * 255);
		default:
			return RGB(v * 255, p * 255, q * 255);
		}
	}

	void CHSVContainerUI::PaintColorRect(HDC hDC, const RECT& rcPaint, const RECT& rcItem , double Hue)
	{
		// Create the alpha blending bitmap
		BITMAPINFO bmi;        // bitmap header
		ZeroMemory(&bmi, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = m_uiWidth;
		bmi.bmiHeader.biHeight = m_uiHeight;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = m_uiWidth * m_uiHeight * 4;

		BYTE *pvBits;          // pointer to DIB section
		HBITMAP hbitmap = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void **)&pvBits, NULL, 0);
		ZeroMemory(pvBits, bmi.bmiHeader.biSizeImage);
		BYTE * vBits = pvBits;
		for (int y = bmi.bmiHeader.biHeight-1; y >= 0; y--)
		{
			for (int x = 0; x < bmi.bmiHeader.biWidth; x++)
			{
				double b = (double)x / m_uiWidth;
				double a = 1.0 - (double)y / m_uiHeight;
				COLORREF color = hsvToRgb(m_dHue , a, b);
				*vBits++ = GetBValue(color);
				*vBits++ = GetGValue(color);
				*vBits++ = GetRValue(color);
				vBits++;
			}
		}

		RECT rcBit = { 0,0,m_uiWidth,m_uiHeight };
		RECT rcConner = { 0,0,0,0 };
		CRenderEngine::DrawImage(hDC, hbitmap, rcItem, rcPaint, rcBit, rcConner, false);
		::DeleteObject(hbitmap);
	}

	void CHSVContainerUI::PaintColorRect2(HDC hDC,const RECT& rcPaint, const RECT& rcColor , COLORREF color)
	{
		RECT rcBmpPart = {0,0,m_uiWidth,m_uiHeight};
		RECT rcCorner = {0};
		int desR = GetRValue(color);
		int desG =  GetGValue(color);
		int desB =  GetBValue(color);
		int m_nFixed = m_uiWidth;

		HDC hMemDC = ::CreateCompatibleDC(hDC);
		BYTE *pBuf;
		BITMAPINFO binfo;
		ZeroMemory(&binfo,sizeof(BITMAPINFO));
		binfo.bmiHeader.biBitCount=32;      //每个像素多少位，也可直接写24(RGB)或者32(RGBA)
		binfo.bmiHeader.biCompression=0;
		binfo.bmiHeader.biHeight=m_uiHeight;
		binfo.bmiHeader.biPlanes=1;
		binfo.bmiHeader.biSizeImage=m_uiWidth*4*m_uiHeight;
		binfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		binfo.bmiHeader.biWidth=m_uiWidth;
		HBITMAP crPalette = CreateDIBSection(hMemDC, &binfo, DIB_RGB_COLORS, (void **)&pBuf, NULL, NULL);
		ZeroMemory(pBuf, binfo.bmiHeader.biSizeImage);
		BYTE* pColorBit = &pBuf[0];
		for(int j = (int)m_uiHeight -1; j >= 0 ; j--)
		{
			for(int i = 0; i <  (int)m_uiWidth; i++)
			{
				int bottomR = 255*i/m_nFixed;
				int bottomG = 255*i/m_nFixed;
				int bottomB = 255*i/m_nFixed;
				//BGRA
				*pColorBit = (BYTE)( (double)desB*i/m_nFixed+(double)j/m_nFixed*(bottomB-(double)desB*i/m_nFixed) ) ;pColorBit++;
				*pColorBit = (BYTE)( (double)desG*i/m_nFixed+(double)j/m_nFixed*(bottomG-(double)desG*i/m_nFixed) );pColorBit++;
				*pColorBit = (BYTE)( (double)desR*i/m_nFixed+(double)j/m_nFixed*(bottomR-(double)desR*i/m_nFixed) );pColorBit++;
				pColorBit++;
			}
		}

		CRenderEngine::DrawImage(hDC, crPalette , rcColor, rcPaint , rcBmpPart, rcCorner, false);
		::DeleteObject(crPalette);
		::DeleteObject(hMemDC);
		::DeleteDC(hMemDC);
	}

	void CHSVContainerUI::PaintColorSelecter(HDC hDC)
	{
		//CUIString image;
		//image.Format(_T("file=%s' source='%d,%d,%d,%d' "));
		CRect rcPaintSub = this->GetPos();
		RECT rcSelItem = { 0,0,m_nSelWidth,m_nSelHeight };
		RECT rcPaint = { 0,0,rcSelItem.right - rcSelItem.left,rcSelItem.bottom - rcSelItem.top };
		rcPaint.left= m_rcItem.left + m_ptSelect.x - rcPaint.right / 2;
		rcPaint.top= m_rcItem.top + m_ptSelect.y - rcPaint.bottom / 2;
		rcPaint.right += rcPaint.left;
		rcPaint.bottom += rcPaint.top;
		rcSelItem = rcPaint;
		//if(rcPaintSub.left > rcPaint.left) 
		//	rcPaint.left = rcPaintSub.left;
		//if(rcPaintSub.top > rcPaint.top) 
		//	rcPaint.top = rcPaintSub.top;
		//if(rcPaintSub.right < rcPaint.right) 
		//	rcPaint.right = rcPaintSub.right;
		//if(rcPaintSub.bottom < rcPaint.bottom) 
		//	rcPaint.bottom = rcPaintSub.bottom;
		HRGN hRgn =  NULL;
		::GetClipRgn(hDC,hRgn);
		::SelectClipRgn(hDC, CreateRectRgn(rcPaintSub.left, rcPaintSub.top, rcPaintSub.right, rcPaintSub.bottom));
		CRenderEngine::DrawImageString(hDC,m_pManager,rcSelItem,rcPaint,m_strImageSelecter);
		::SelectClipRgn(hDC, hRgn);
	}

	void CHSVContainerUI::SetHue(UINT nHue)
	{
		if(nHue > 360)
			nHue = 360;
		m_dHue = nHue / 360.0;
		SelectColor();
		this->Invalidate();
	}

	UINT CHSVContainerUI::GetHue() //0~360
	{
		return UINT(m_dHue*360);
	}

	COLORREF CHSVContainerUI::GetSelectColor()
	{
		return m_selColor;
	}

	void CHSVContainerUI::SetSelecterPos()
	{
		m_ptSelect.x = (int)floor(m_dBValue * m_uiWidth);
		m_ptSelect.y = m_uiHeight -  (int)floor(m_dAValue * m_uiHeight);
		this->Invalidate();
	}

	void CHSVContainerUI::SetSelectColor(COLORREF color)
	{
		double r = (double)GetRValue(color)/255;
		double g = (double)GetGValue(color)/255;
		double b = (double)GetBValue(color)/255;
		double hue = 0.0;
		try{
		Rgb2Hsv(r, g, b, hue , m_dAValue, m_dBValue);
		}
		catch(...){}
		/*	
		bool bEquel = true;
		char tmp1[8];
		memset(tmp1, 0, 8);
		tmp1[6] = 0xf8;
		tmp1[7] = 0xff;
		for(int i = 0; i < 8; i++)
		{
		if(((char*)&hue)[i] != tmp1[i])
		{
		bEquel = false;
		}
		}
		if(bEquel == true)
		{
		hue = 0.0;
		}
		*/
		m_dHue = hue / 360;
		if(m_dHue > 1.0)
			m_dHue = 1.0;
		if(m_dHue < 0.0)
			m_dHue = 0.0;
		m_selColor = color;
		SetSelecterPos();
	}

	void CHSVContainerUI::Rgb2Hsv(double R, double G, double B, double& H, double& S, double&V)
	{
		// r,g,b values are from 0 to 1
		// h = [0,360], s = [0,1], v = [0,1]
		// if s == 0, then h = -1 (undefined)

		double min, max, delta,tmp;
		tmp = min(R, G);
		min = min( tmp, B );
		tmp = max( R, G);
		max = max(tmp, B );
		V = max; // v

		delta = max - min;

		if( max != 0 )
			S = delta / max; // s
		else
		{
			// r = g = b = 0 // s = 0, v is undefined
			S = 0;
			H = 0;
			return;
		}
		if(delta == 0)
		{
			H = 0;
			return;
		}
		if( R == max )
			H = ( G - B ) / delta; // between yellow & magenta
		else if( G == max )
			H = 2 + ( B - R ) / delta; // between cyan & yellow
		else
			H = 4 + ( R - G ) / delta; // between magenta & cyan

		H *= 60; // degrees
		if( H < 0 )
			H += 360;
	}

	void CHSVContainerUI::SetSelecterImage(LPCTSTR strImage, SHORT nWidth, SHORT nHeight)
	{
		if( _tcscmp(strImage, _T("")) != 0 )
		{
			m_strImageSelecter = strImage;
		}
		if(nWidth > 0 && nHeight > 0)
		{
			m_nSelWidth = nWidth;
			m_nSelHeight = nHeight;
		}
	}

	void CHSVContainerUI::SelectColor()
	{
		m_selColor = hsvToRgb(m_dHue, m_dAValue, m_dBValue);
		TNotifyUI notify;
		notify.sType = HSVCOLOR_NOTIFY_COLOR_SELECT;
		notify.wParam = 0;
		notify.lParam = m_selColor;
		if(this->OnNotify)
			OnNotify(&notify);
	}

	void CHSVContainerUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
	}

	void CHSVContainerUI::CalculateClickPos(POINT ptMouse)
	{
		m_ptSelect.y = ptMouse.y - m_rcColor.top;
		m_ptSelect.x = ptMouse.x - m_rcColor.left;
		m_dAValue = 1.0 - ((double)(m_ptSelect.y) / m_uiHeight);
		m_dBValue = (double)(m_ptSelect.x) / m_uiWidth;
		if(m_dAValue  <  numeric_limits<double>::epsilon())
		{
			m_ptSelect.y = m_uiHeight;
			m_dAValue = 0.0;
		}
		if(m_dAValue > 1.0)
		{
			m_ptSelect.y = 0;
			m_dAValue = 1.0;
		}
		if(m_dBValue  < numeric_limits<double>::epsilon())
		{
			m_ptSelect.x = 0;
			m_dBValue = 0.0;
		}
		if(m_dBValue > 1.0)
		{
			m_ptSelect.x = m_uiWidth;
			m_dBValue = 1.0;
		}
		SelectColor();
		Invalidate();
	}

	void CHSVContainerUI::DoEvent(TEventUI& event)
	{
		if(event.Type == UIEVENT_BUTTONDOWN)
		{
			m_bLBDown = true;
			CalculateClickPos(event.ptMouse);
		}
		else if(event.Type == UIEVENT_MOUSEMOVE)
		{
			if(m_bLBDown)
			{
				CalculateClickPos(event.ptMouse);
			}

		}
		else if(event.Type == UIEVENT_BUTTONUP)
		{
			m_bLBDown = false;
		}
		else if(event.Type == UIEVENT_MOUSELEAVE)
		{
		}
		__super::DoEvent(event);
	}



	/**
	*CColorPanel
	*/
#define COLORPANELXML _T("color_panel\\color_panel.xml")
#define SKIN_FOLDER _T("")

	CColorPanel::CColorPanel()
	{
		m_bAutoUpate = false;
		m_dwValue = 0xFF000000;

		m_pValueEdit = NULL;
		m_pHSVContainer = NULL;
		m_pHueSlider = NULL;
		m_pAlphaSlider = NULL;

		m_pManager = NULL;
		m_pHitControl = NULL;
		m_strClassName = _T("ColorPanel");
	}

	void CColorPanel::Init(HWND hwndParent, POINT point, CPaintManagerUI * pManager,  DWORD color, bool autoUpate)
	{
		m_pManager = pManager;
		m_bAutoUpate = autoUpate;
		Create( hwndParent, m_strClassName, UI_WNDSTYLE_FRAME, UI_CLASSSTYLE_CONTAINER, 0, 0, 0, 0, 0);
		::MoveWindow(*this, point.x, point.y, 185, 229, false);
		m_dwValue = color;
		SetColorEditText();
		m_pHSVContainer->SetSelectColor(GetColor());
		m_pHueSlider->SetValue(m_pHSVContainer->GetHue());
		m_PaintManager.FindSubControlByName(NULL, _T("current_color"))->SetBkColor(m_dwValue);
	}

	void CColorPanel::SetDisContorl(CControlUI* pDisable)
	{
		if(pDisable)
		{
			m_pHitControl = pDisable;
			m_pHitControl->SetEnabled(false);
		}
	}

	void CColorPanel::InitWindow()
	{
		CControlUI * pControl = NULL;

		pControl = m_PaintManager.FindSubControlByName(NULL, _T("value_edit"));
		ASSERT(pControl);
		m_pValueEdit = static_cast<CEditUI *>(pControl);
		//m_pValueEdit->SetNotifyFocusOut(true);
	
		pControl = m_PaintManager.FindSubControlByName(NULL, _T("hsv_color_container"));
		ASSERT(pControl);
		m_pHSVContainer = static_cast<CHSVContainerUI *>(pControl);
		m_pHSVContainer->OnNotify += MakeDelegate(this, &CColorPanel::OnHSVNotify);

		pControl = m_PaintManager.FindSubControlByName(NULL, _T("hue_slider"));
		ASSERT(pControl);
		m_pHueSlider = static_cast<CSliderUI *>(pControl);
		if(m_pHueSlider)
		{
			m_pHueSlider->SetStretchForeImage(false);
			m_pHueSlider->SetValue(0);
		}

		pControl = m_PaintManager.FindSubControlByName(NULL, _T("alpha_slider"));
		if(pControl)
		{
			m_pAlphaSlider = static_cast<CSliderUI *>(pControl);
			if(m_pAlphaSlider)
			{
				m_pAlphaSlider->SetStretchForeImage(false);
				m_pAlphaSlider->SetValue(255);
			}
		}
		else
		{
			SIZE winSize = m_PaintManager.GetMaxInfo();
			winSize.cy -= 20;
			m_PaintManager.SetMaxInfo(winSize.cx,winSize.cy);
		}
		
	}

	CUIString CColorPanel::GetSkinFolder()
	{
		return SKIN_FOLDER;
	}

	CUIString CColorPanel::GetSkinFile()
	{
		return COLORPANELXML;
	}

	LPCTSTR CColorPanel::GetWindowClassName( ) const
	{
		return m_strClassName;
	}

	void CColorPanel::Notify(TNotifyUI& msg)
	{
		if(msg.sType == DUI_MSGTYPE_VALUECHANGED)
		{
			if(m_pAlphaSlider && msg.pSender == m_pAlphaSlider)
			{
				DWORD dwAlpha = m_pAlphaSlider->GetValue();
				dwAlpha <<= 24;
				m_dwValue &= 0x00FFFFFF;
				m_dwValue |= dwAlpha;

				SetColorEditText();
				if(m_bAutoUpate)
					SendNotify(COLORPANEL_MSGTYPE_ALPHA, m_pAlphaSlider->GetValue(), 0);
			}
			else if(m_pHueSlider &&  msg.pSender == m_pHueSlider)
			{
				if(m_pHSVContainer->GetHue() != m_pHueSlider->GetValue())
					m_pHSVContainer->SetHue(m_pHueSlider->GetValue());
			}

		}
		else	if( _tcscmp(msg.sType ,DUI_MSGTYPE_RETURN) == 0
			|| _tcscmp(msg.sType ,DUI_MSGTYPE_ESCAPE) == 0) //回车,退出
		{
			if(msg.pSender->GetName() == m_pValueEdit->GetName())
				m_pHSVContainer->SetFocus();
		}
		else if(msg.sType == DUI_MSGTYPE_KILLFOCUS)
		{
			if(msg.pSender->GetName() != m_pValueEdit->GetName())
			{
				return;
			}

			CUIString strText = m_pValueEdit->GetText();
			for(int i = 0; i < strText.GetLength(); i++)
			{
				TCHAR szChar = strText.GetAt(i);
				if( (szChar >= _T('A') && szChar <= _T('F') )  ||  ( szChar >= _T('a') && szChar <= _T('f') ) || ( szChar >= _T('0') && szChar <= _T('9') ) )
					NULL;
				else
				{
					SetColorEditText();
					return;
				}
			}

			CUIString strAlpha = strText.Left(2);
			CUIString strColor = strText.Right(6);

			DWORD dwAlpha = 0;
			_stscanf_s(strAlpha, _T("%x"), &dwAlpha);
			DWORD dwColor = 0;
			_stscanf_s(strColor, _T("%x"), &dwColor);

			if(m_pAlphaSlider && dwAlpha != m_pAlphaSlider->GetValue())
			{
				m_pAlphaSlider->SetValue(dwAlpha);
				SendNotify(COLORPANEL_MSGTYPE_ALPHA, m_pAlphaSlider->GetValue(), 0);
			}
			m_dwValue = (dwAlpha << 24) | dwColor;

			m_pHSVContainer->SetSelectColor(GetColor());
			m_pHueSlider->SetValue(m_pHSVContainer->GetHue());
			m_PaintManager.FindSubControlByName(NULL, _T("current_color"))->SetBkColor(m_dwValue);
			SetColorEditText();
			if(m_bAutoUpate)
				SendNotify(COLORPANEL_MSGTYPE_SELECT, GetColor(), GetAlpha());



		}
		else if(msg.sType == COLORCHUNK_NOTIFY_COLOR_PRESSED)
		{
			DWORD bkColor = msg.pSender->GetBkColor();
			bkColor &= 0x00FFFFFF;
			m_dwValue &= 0xFF000000;
			m_dwValue |= bkColor;

			m_pHSVContainer->SetSelectColor(GetColor());
			m_pHueSlider->SetValue(m_pHSVContainer->GetHue());
			SetColorEditText();
			m_PaintManager.FindSubControlByName(NULL, _T("current_color"))->SetBkColor(m_dwValue);
			if(m_bAutoUpate)
				SendNotify(COLORPANEL_MSGTYPE_SELECT, GetColor(), GetAlpha());
		}
	}

	bool CColorPanel::OnHSVNotify(LPVOID notify)
	{
		TNotifyUI msg = *((TNotifyUI*)notify);
		if(msg.sType == HSVCOLOR_NOTIFY_COLOR_SELECT)
		{
			COLORREF color = (DWORD)msg.lParam;
			DWORD dwValue = 0;
			DWORD dwAlpha = GetAlpha();
			BYTE r = GetRValue(color);
			BYTE g = GetGValue(color);
			BYTE b = GetBValue(color);

			dwValue |= (DWORD)(dwAlpha << 24);
			dwValue |= RGB(b,g,r);

			m_dwValue = dwValue;

			SetColorEditText();
			m_PaintManager.FindSubControlByName(NULL, _T("current_color"))->SetBkColor(m_dwValue);
			if(m_bAutoUpate)
				SendNotify(COLORPANEL_MSGTYPE_SELECT, GetColor(), GetAlpha());
		}
		return true;
	}

	CControlUI* CColorPanel::CreateControl(LPCTSTR pstrClass)
	{
		CControlUI * pControlUI = NULL;

		if(_tcsicmp(pstrClass, _T("HSVContainer")) == 0)
		{
			CHSVContainerUI * pColorContainer = new CHSVContainerUI;
			pControlUI = pColorContainer;
		}
		else if(_tcsicmp(pstrClass, _T("ColorChunk")) == 0)
		{
			CColorChunkUI* pColorChunk = new CColorChunkUI;
			pColorChunk->SetOwner(this);
			pControlUI = pColorChunk;
		}

		return pControlUI;
	}

	LRESULT CColorPanel::ResponseDefaultKeyEvent(WPARAM wParam)
	{
		if(wParam == VK_ESCAPE)
		{
			Close(1);
		}
		return 0;
	}

	void CColorPanel::OnFinalMessage(HWND hWnd)
	{
		delete this;
	}

	LRESULT CColorPanel::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if(uMsg == WM_SYSCOMMAND)
		{
			if( GET_SC_WPARAM(wParam) != SC_MOVE)
				return true;

		}
		if(uMsg == WM_TIMER)
		{
			if(wParam== 102)
			{
				::KillTimer(m_hWnd,102);
				if(!m_bAutoUpate)
				{
					//SendNotify(COLORPANEL_MSGTYPE_ALPHA,  GetAlpha(), 0);
					SendNotify(COLORPANEL_MSGTYPE_SELECT, GetColor(), GetAlpha());
				}
				Close(1);
				if(m_pHitControl) m_pHitControl->SetEnabled();
			}
		}
		if(uMsg == WM_ACTIVATE) 
		{
			if(LOWORD(wParam)!=WA_INACTIVE)
			{
				// application is being activated
			}
			else
			{
				// application is being deactivated
				::SetTimer(m_hWnd,102,10,0);
			}
			return true;
		}

		return __super::HandleMessage(uMsg, wParam, lParam);
	}

	void CColorPanel::SetColorEditText()
	{
		CUIString strValue;
		strValue.Format(_T("%08x"),m_dwValue);
		strValue.MakeUpper();
		if(m_pValueEdit)
			m_pValueEdit->SetText(strValue);
	}

	void CColorPanel::SetValue(DWORD dwValue)
	{
		m_dwValue = dwValue;

		CUIString strValue;
		strValue.Format(_T("%08x"), dwValue);
		strValue.MakeUpper();
		m_pValueEdit->SetText(strValue);

		COLORREF color = GetColorByValue(dwValue);
		m_pHSVContainer->SetSelectColor(color);

	}

	void CColorPanel::SetValue(BYTE dwAlpha, COLORREF color)
	{
		DWORD dwValue = (DWORD)(dwAlpha << 24);

		BYTE r = GetRValue(color);
		BYTE g = GetGValue(color);
		BYTE b = GetBValue(color);

		dwValue |= RGB(b,g,r);

		SetValue(dwValue);
	}

	DWORD CColorPanel::GetValue()
	{
		return m_dwValue;
	}

	COLORREF CColorPanel::GetColor()
	{
		return GetColorByValue(m_dwValue);
	}

	BYTE CColorPanel::GetAlpha()
	{
		return GetAlphaByValue(m_dwValue);
	}

	COLORREF CColorPanel::GetColorByValue(DWORD dwValue)
	{
		COLORREF color = dwValue & 0x00FFFFFF;
		BYTE r = (BYTE)(color >> 16);
		BYTE g = (BYTE)(color >> 8);
		BYTE b = (BYTE)(color);
		color = RGB(r,g,b);

		return color;
	}

	BYTE CColorPanel::GetAlphaByValue(DWORD dwValue)
	{
		return  (BYTE)(dwValue >> 24);
	}

	void CColorPanel::SendNotify(CUIString strMsg,WPARAM wParam,LPARAM lParam)
	{
		if(m_pManager)
		{
			TNotifyUI notify;
			notify.sType = strMsg;
			notify.wParam = wParam;
			notify.lParam = lParam;
			notify.pSender = m_PaintManager.FindSubControlByName(NULL, _T("current_color"));
			m_pManager->SendNotify(notify);
		}
	}

} //namespace NSCGUI