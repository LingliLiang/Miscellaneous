#include "stdafx.h"
#include "UILayerControl.h"

namespace DirectUI
{

	CLayerControlUI::CLayerControlUI()
		:m_bHasParent(TRUE),
		m_hDcOffscreen(NULL),
		m_hDcBackground(NULL),
		m_hbmpOffscreen(NULL),
		m_hbmpBackground(NULL),
		m_bFirstLayout(TRUE),
		m_bLayeredWindow(FALSE)
	{
	}

	CLayerControlUI::~CLayerControlUI()
	{
		if( m_hDcOffscreen != NULL ){ ::DeleteDC(m_hDcOffscreen); m_hDcOffscreen = NULL;}
		if( m_hDcBackground != NULL ){ ::DeleteDC(m_hDcBackground); m_hDcBackground = NULL;}
		if( m_hbmpOffscreen != NULL ){ ::DeleteObject(m_hbmpOffscreen); m_hbmpOffscreen = NULL;}
		if( m_hbmpBackground != NULL ){ ::DeleteObject(m_hbmpBackground); m_hbmpBackground = NULL;}
	}

	//-----------------------------CControlUI---------------------------------

	LPCTSTR CLayerControlUI::GetClass() const
	{
		return _T("LayerControlUI");
	}

	LPVOID  CLayerControlUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, _T("LayerControl")) == 0) return this;
		return NULL;
	}

	void CLayerControlUI::DoInit()
	{
		if(m_pManager && m_hWnd == NULL)
		{
			Create(m_bHasParent ? m_pManager->GetPaintWindow() : NULL,
				GetWindowClassName(),
				WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				0);
		}
		DWORD dwStyle = ::GetClassLong(*this,GCL_STYLE);
		dwStyle |= CS_DBLCLKS;
		SetClassLong(m_hWnd, GCL_STYLE,dwStyle);
		__super::DoInit();
	}

	void CLayerControlUI::Invalidate()
	{
		::InvalidateRect(m_hWnd,NULL,TRUE);
		__super::Invalidate();
	}

	void CLayerControlUI::SetInternVisible(bool bVisible)
	{
		CControlUI::SetInternVisible(bVisible);
	}

	void CLayerControlUI::SetVisible(bool bVisible)
	{
		CControlUI::SetVisible(bVisible);
		if(m_hWnd) ::ShowWindow(m_hWnd, bVisible); 
	}

	void CLayerControlUI::SetPos(RECT rc)
	{
		__super::SetPos(rc);
		if(m_pManager){
			POINT pt = { rc.left, rc.top};
			::ClientToScreen(m_pManager->GetPaintWindow(),&pt);
			::SetWindowPos(m_hWnd, NULL, pt.x, pt.y , rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	void CLayerControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("hasparent")) == 0) 
			SetHasParentWnd(_tcscmp(pstrValue, _T("true")) == 0);
		else if (_tcscmp(pstrName, _T("layered")) == 0) 
			SetLayeredWnd(_tcscmp(pstrValue, _T("true")) == 0);
		else if (_tcscmp(pstrName, _T("skinfile")) == 0) SetSkinFile(pstrValue);
		else 
			__super::SetAttribute(pstrName, pstrValue);
	}

	void CLayerControlUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		// 不绘制
		__super::DoPaint(hDC, rcPaint);
	}


	BOOL CLayerControlUI::IsHasParentWnd() const
	{
		return m_bHasParent;
	}

	void CLayerControlUI::SetHasParentWnd(BOOL b)
	{
		if (m_bHasParent == b) return;
		m_bHasParent = b;
	}

	BOOL CLayerControlUI::IsLayeredWnd() const
	{
		return m_bLayeredWindow;
	}

	void CLayerControlUI::SetLayeredWnd(BOOL b)
	{
		if (m_bLayeredWindow == b) return;
		m_bLayeredWindow = b;
	}

	void CLayerControlUI::SetSkinFile(LPCTSTR pstrSkin)
	{
		m_strSkin = pstrSkin;
	}


	//------------------------------CWindowUI--------------------------------

	LPCTSTR CLayerControlUI::GetWindowClassName() const
	{
		return _T("CLayerControlUI");
	}

	CUIString CLayerControlUI::GetSkinFile()
	{
		return  m_strSkin;
	}

	CUIString CLayerControlUI::GetSkinFolder()
	{
		return _T(""); //不需要返回,使用默认目录
	}

	void CLayerControlUI::InitWindow()
	{
		m_PaintManager.SetBackgroundTransparent(FALSE);
		if( m_hDcOffscreen != NULL ){ ::DeleteDC(m_hDcOffscreen); m_hDcOffscreen = NULL;}
		if( m_hDcBackground != NULL ){ ::DeleteDC(m_hDcBackground); m_hDcBackground = NULL;}
		if( m_hbmpOffscreen != NULL ){ ::DeleteObject(m_hbmpOffscreen); m_hbmpOffscreen = NULL;}
		if( m_hbmpBackground != NULL ){ ::DeleteObject(m_hbmpBackground); m_hbmpBackground = NULL;}
		::PostMessage(g_hMsgWnd,NS_WM_CHILD_JOIN,(WPARAM)m_hWnd,0);
	}

	LRESULT CLayerControlUI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if(uMsg == WM_PAINT)
		{
			BOOL bHandled = FALSE;
			LRESULT lres = OnPaint(wParam, lParam, bHandled);
			if(bHandled) return lres;
		}
		else if(uMsg == NS_WM_CHILD_MOVE){
			CPoint pt(lParam);
			pt.x += m_rcItem.left;
			pt.y += m_rcItem.top;
			::SetWindowPos(m_hWnd, NULL, pt.x, pt.y , 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
			//::OutputDebugStringA("CLayerControlUI - WM_MOVE\n");
			return 0;
		}

		return __super::HandleMessage(uMsg, wParam, lParam);
	}

	void CLayerControlUI::OnFinalMessage(HWND hWnd)
	{
		m_hWnd = NULL;
		__super::OnFinalMessage(hWnd);
	}

	CControlUI* CLayerControlUI::CreateControl(LPCTSTR pstrClass)
	{
		return NULL;
	}

	LRESULT CLayerControlUI::ResponseDefaultKeyEvent(WPARAM wParam)
	{
		if (wParam == VK_RETURN || wParam == VK_ESCAPE)
			return TRUE;
		return FALSE;
	}

	namespace {
		CControlUI* CALLBACK __FindControlFromUpdate(CControlUI* pThis, LPVOID pData)
		{
			return pThis->IsUpdateNeeded() ? pThis : NULL;
		}

		HBITMAP CreateBitmap(HDC hDC, int cx, int cy, BYTE** pBits)
		{
			//建立up-left Bitmap
			BITMAPINFO bmi;
			::ZeroMemory(&bmi, sizeof(BITMAPINFO));
			LPBITMAPINFO lpbiSrc = NULL;

			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = cx;
			bmi.bmiHeader.biHeight = -cy;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = cx * cy * 4;
			bmi.bmiHeader.biClrUsed = 0;
			HBITMAP hBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void **)pBits, NULL, NULL);

			return hBitmap;
		}
	} //unnamed namespace

	LRESULT CLayerControlUI::OnPaint(WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(m_bLayeredWindow)
		{
			DWORD dwExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
			if((dwExStyle&WS_EX_LAYERED) != WS_EX_LAYERED)
				SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle|WS_EX_LAYERED);

			bHandled = TRUE;
			auto m_pRoot = m_PaintManager.GetRoot();
			HWND m_hWndPaint = m_hWnd;
			bool m_bOffscreenPaint = true;
			HDC m_hDcPaint = m_PaintManager.GetPaintDC();
			RECT rcClient = { 0 };
			::GetClientRect(m_hWndPaint, &rcClient);
			int nClientWidth = rcClient.right - rcClient.left;
			int nClientHeight = rcClient.bottom - rcClient.top;


			// Should we paint?
			RECT rcPaint = { 0 };
			if (!::GetUpdateRect(m_hWndPaint, &rcPaint, FALSE)) return true;
			if (m_pRoot == NULL) 
			{
				PAINTSTRUCT ps = { 0 };
				::BeginPaint(m_hWndPaint, &ps);
				::EndPaint(m_hWndPaint, &ps);
				return 0;
			}            
			// Do we need to resize anything?
			// This is the time where we layout the controls on the form.
			// We delay this even from the WM_SIZE messages since resizing can be
			// a very expensize operation.
			if (m_pRoot->IsUpdateNeeded()) 
			{
				//  m_bUpdateNeeded = false;
				RECT rcClient = { 0 };
				::GetClientRect(m_hWndPaint, &rcClient);
				if (!::IsRectEmpty(&rcClient))
				{
					if (m_pRoot->IsUpdateNeeded()) 
					{
						if( !::IsIconic(m_hWndPaint))
							m_pRoot->SetPos(rcClient);
						if (m_hDcOffscreen != NULL) ::DeleteDC(m_hDcOffscreen);
						if (m_hDcBackground != NULL) ::DeleteDC(m_hDcBackground);
						if (m_hbmpOffscreen != NULL) ::DeleteObject(m_hbmpOffscreen);
						if (m_hbmpBackground != NULL) ::DeleteObject(m_hbmpBackground);
						m_hDcOffscreen = NULL;
						m_hDcBackground = NULL;
						m_hbmpOffscreen = NULL;
						m_hbmpBackground = NULL;
					}
					else 
					{
						CControlUI* pControl = NULL;
						while( pControl = m_pRoot->FindControl(__FindControlFromUpdate, NULL, UIFIND_VISIBLE | UIFIND_ME_FIRST) ) 
							pControl->SetPos(pControl->GetPos());
					}
					// We'll want to notify the window when it is first initialized
					// with the correct layout. The window form would take the time
					// to submit swipes/animations.
					if (m_bFirstLayout) 
					{
						m_bFirstLayout = false;
						m_PaintManager.SendNotify(m_pRoot, DUI_MSGTYPE_WINDOWINIT,  0, 0, false);
					}
				}
			}
			// Set focus to first control?

			//
			// Render screen
			//


			// Prepare offscreen bitmap?
			if( m_bOffscreenPaint && m_hbmpOffscreen == NULL )
			{
				m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
				m_hbmpOffscreen = CreateBitmap(m_hDcPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, &m_pBmpOffscreenBits);
				//m_hbmpOffscreen = ::CreateCompatibleBitmap(m_hDcPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
				ASSERT(m_hDcOffscreen);
				ASSERT(m_hbmpOffscreen);
			}
			// Begin Windows paint
			PAINTSTRUCT ps = { 0 };
			::BeginPaint(m_hWndPaint, &ps);
			if( m_bOffscreenPaint )
			{
				HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
				int iSaveDC = ::SaveDC(m_hDcOffscreen);

				for(int y = rcPaint.top; y < rcPaint.bottom; ++y)
				{
					ZeroMemory((unsigned int*)m_pBmpOffscreenBits + y * nClientWidth + rcPaint.left, (rcPaint.right - rcPaint.left) * 4);
				}

				m_pRoot->DoPaint(m_hDcOffscreen, rcPaint);
				//for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
				//    CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_aPostPaintControls[i]);
				//    pPostPaintControl->DoPostPaint(m_hDcOffscreen, ps.rcPaint);
				//}

				for (int y = rcPaint.top; y < rcPaint.bottom; ++y)
				{
					for (int x = rcPaint.left; x < rcPaint.right; ++x)
					{				   
						int i = (y*nClientWidth + x) * 4;
						if ((m_pBmpOffscreenBits[i + 3] == 0) &&
							(m_pBmpOffscreenBits[i + 0] != 0 ||
							m_pBmpOffscreenBits[i + 1] != 0 ||
							m_pBmpOffscreenBits[i + 2] != 0))
						{
							m_pBmpOffscreenBits[i + 3] = 255;
						}

					}
				}

				::RestoreDC(m_hDcOffscreen, iSaveDC);

				//::BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
				//    ps.rcPaint.bottom - ps.rcPaint.top, m_hDcOffscreen, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

				RECT rcWnd = {0};
				::GetWindowRect(m_hWnd, &rcWnd);
				POINT pt = {rcWnd.left, rcWnd.top};
				SIZE szWindow = {rcWnd.right-rcWnd.left, rcWnd.bottom-rcWnd.top};
				POINT ptSrc = {0, 0};
				BLENDFUNCTION blendPixelFunction = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
				::UpdateLayeredWindow(m_hWnd, NULL, &pt, &szWindow, m_hDcOffscreen, 
					&ptSrc, 0, &blendPixelFunction, ULW_ALPHA);

				::SelectObject(m_hDcOffscreen, hOldBitmap);
			}
			// All Done!
			::EndPaint(m_hWndPaint, &ps);

			// If any of the painting requested a resize again, we'll need
			// to invalidate the entire window once more.
			if( m_pRoot->IsUpdateNeeded() ) {
				::InvalidateRect(m_hWndPaint, NULL, FALSE);
			}

		}       
		return 0;
	}


} //namespace DirectUI