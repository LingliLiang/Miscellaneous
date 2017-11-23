#include "stdafx.h"
#include "UILayerControl.h"

namespace DirectUI
{

	CLayerControlUI::CLayerControlUI()
		:m_bHasParent(TRUE),
		m_bPaintInit(TRUE),
		m_hDcOffscreen(NULL),
		m_hbmpOffscreen(NULL),
		m_bFirstLayout(TRUE),
		m_bLayeredWindow(FALSE),
		m_nClipState(CLIP_NON-1),
		m_bParentClip(FALSE),
		m_bWndVisible(TRUE),
		m_nWMOnMove(0)
	{
	}

	CLayerControlUI::~CLayerControlUI()
	{
		if( m_hDcOffscreen != NULL ){ ::DeleteDC(m_hDcOffscreen); m_hDcOffscreen = NULL;}
		if( m_hbmpOffscreen != NULL ){ ::DeleteObject(m_hbmpOffscreen); m_hbmpOffscreen = NULL;}
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
				/*WS_VISIBLE | */WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
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
		if(m_bWndVisible && m_hWnd) ShowWindow(bVisible,false);
	}

	void CLayerControlUI::SetPos(RECT rc)
	{
		__super::SetPos(rc);
		if(m_pManager){
			if(m_bParentClip) //判断裁剪
			{
				DoParentClip();
			}
			POINT pt = { rc.left, rc.top};
			::ClientToScreen(m_pManager->GetPaintWindow(),&pt);
			::SetWindowPos(m_hWnd, NULL, pt.x, pt.y , rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	void CLayerControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("hasparent")) == 0) 
			SetHasParentWnd(_tcscmp(pstrValue, _T("true")) == 0);
		if (_tcscmp(pstrName, _T("clipparent")) == 0) 
			SetParentClip(_tcscmp(pstrValue, _T("true")) == 0);
		else if (_tcscmp(pstrName, _T("layered")) == 0) 
			SetLayeredWnd(_tcscmp(pstrValue, _T("true")) == 0);
		else if (_tcscmp(pstrName, _T("skinfile")) == 0) SetSkinFile(pstrValue);
		else if (_tcscmp(pstrName, _T("wmonmove")) == 0) SetWMONMOVE(_ttoi(pstrValue));
		else 
			__super::SetAttribute(pstrName, pstrValue);
	}

	void CLayerControlUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if(m_bPaintInit){
			m_bPaintInit = FALSE;
			SetVisible(m_bVisible);
		}
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

	void CLayerControlUI::SetParentClip(BOOL b)
	{
		if (m_bParentClip == b) return;
		m_bParentClip = b;
	}

	void CLayerControlUI::SetWMONMOVE(UINT wm_onmove)
	{
		m_nWMOnMove = wm_onmove;
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
		if( m_hDcOffscreen != NULL ){ ::DeleteDC(m_hDcOffscreen); m_hDcOffscreen = NULL;}
		if( m_hbmpOffscreen != NULL ){ ::DeleteObject(m_hbmpOffscreen); m_hbmpOffscreen = NULL;}
	}

	LRESULT CLayerControlUI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if(uMsg == WM_PAINT)
		{
			BOOL bHandled = FALSE;
			LRESULT lres = OnPaint(wParam, lParam, bHandled);
			if(bHandled) return lres;
		}
		else if(m_nWMOnMove && uMsg == m_nWMOnMove)
		{
			POINT pt = { m_rcItem.left, m_rcItem.top};
			::ClientToScreen(m_pManager->GetPaintWindow(),&pt);
			::SetWindowPos(m_hWnd, NULL, pt.x, pt.y , 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
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

	LRESULT CLayerControlUI::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);
		::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
			rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

		m_PaintManager.Init(m_hWnd);
		m_PaintManager.AddPreMessageFilter(this);

		CDialogBuilder builder;

		switch(GetResourceType())
		{
		case UILIB_ZIP:
			m_PaintManager.SetResourceZip(GetZIPFileName().GetData(), true);
			break;
		case UILIB_ZIPRESOURCE:
			{
				HRSRC hResource = ::FindResource(m_PaintManager.GetResourceDll(), GetResourceID(), _T("ZIPRES"));
				if (hResource == NULL)
					return 0L;

				DWORD dwSize = 0;
				HGLOBAL hGlobal = ::LoadResource(m_PaintManager.GetResourceDll(), hResource);
				if (hGlobal == NULL) 
				{
					::FreeResource(hResource);
					return 0L;
				}

				dwSize = ::SizeofResource(m_PaintManager.GetResourceDll(), hResource);
				if (dwSize == 0)
					return 0L;

				m_lpResourceZIPBuffer = new BYTE[dwSize];
				if (m_lpResourceZIPBuffer != NULL)
				{
					::CopyMemory(m_lpResourceZIPBuffer, (LPBYTE)::LockResource(hGlobal), dwSize);
				}
				::FreeResource(hResource);
				m_PaintManager.SetResourceZip(m_lpResourceZIPBuffer, dwSize);
			}
			break;
		}

		CControlUI* pRoot = NULL;
		if (GetResourceType() == UILIB_RESOURCE)
		{
			CUIString strXml = GetSkinFile().GetData();
			STRINGorID xmlID(strXml);
			pRoot = builder.Create(xmlID, _T("xml"), this, &m_PaintManager);
		}
		else
			pRoot = builder.Create(GetSkinFile().GetData(), (UINT)0, this, &m_PaintManager);

		if (pRoot == NULL)
		{
			pRoot = new CContainerUI;
			pRoot->SetBkColor(0xffffffff);
		}

		m_PaintManager.AttachDialog(pRoot);
		m_PaintManager.AddNotifier(this);
		
		//不知道为什么要加这句在CWindowUI,使用bktrans设置Transparent
		//m_PaintManager.SetBackgroundTransparent(TRUE); 

		InitWindow();
		return 0;
	}


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
						if (m_hbmpOffscreen != NULL) ::DeleteObject(m_hbmpOffscreen);
						m_hDcOffscreen = NULL;
						m_hbmpOffscreen = NULL;
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

	void CLayerControlUI::DoParentClip()
	{
		if(!m_hWnd) return;
		auto pParent = m_pParent;
		CContainerUI* pContainer = NULL;
		int nClipState = CLIP_NON;
		CRect rcItem(m_rcItem),rcAnd, rcParent;
		while(pParent)
		{
			rcParent = pParent->GetPos();
			if(pContainer = static_cast<CContainerUI*>(pParent->GetInterface(DUI_CTR_CONTAINER))){
				CRect rcScrollBarPadding;
				rcParent.Deflate( &pContainer->GetInset());
				auto pScrollBar = pContainer->GetHorizontalScrollBar();
				if(pScrollBar) rcScrollBarPadding.bottom = pScrollBar->GetHeight();
				pScrollBar = pContainer->GetVerticalScrollBar();
				if(pScrollBar) rcScrollBarPadding.right = pScrollBar->GetWidth();
				rcParent.Deflate(&rcScrollBarPadding);
			}
			::IntersectRect(&rcAnd,&rcParent,&rcItem);
			if(rcAnd.IsNull())
			{
				nClipState = CLIP_FULL; //裁剪全部
				break;
			}
			else if(rcAnd.EqualRect(&rcItem))
			{
				nClipState = CLIP_NON; //没有裁剪,继续向父辈查询裁剪状况
			}
			else
			{
				nClipState = CLIP_HALF;
				break;
			}
			pParent = pParent->GetParent();
		}

		if(m_nClipState == nClipState && nClipState != CLIP_HALF) //状态相同不需要进行操作
		{
			return ;
		}

		//DUI__Trace(_T("nClipState %d"), nClipState);

		
		if(CLIP_HALF == nClipState)
		{
			rcAnd.MoveToXY(rcAnd.left-rcItem.left,rcAnd.top-rcItem.top); //还原到原点区域
			//DUI__Trace(_T("rcAnd {%d,%d, %d, %d}"), rcAnd.left,rcAnd.top, rcAnd.right, rcAnd.bottom);
			//DUI__Trace(_T("rcItem {%d,%d, %d, %d}"), rcItem.left,rcItem.top, rcItem.right, rcItem.bottom);
			//rcItem.MoveToXY(0,0);
			//DUI__Trace(_T("rcItem {%d,%d, %d, %d}"), rcItem.left,rcItem.top, rcItem.right, rcItem.bottom);
			HRGN hRgn = ::CreateRectRgn(rcAnd.left,rcAnd.top, rcAnd.right, rcAnd.bottom);
			::SetWindowRgn(m_hWnd, hRgn, TRUE);
			if(!m_bWndVisible)
			{
				m_bWndVisible= TRUE;
				ShowWindow(true,false);
			}
		}
		else if(CLIP_NON == nClipState)
		{
			::SetWindowRgn(m_hWnd, NULL, TRUE);
			if(!m_bWndVisible)
			{
				m_bWndVisible= TRUE;
				ShowWindow(true,false);
			}
		}
		else
		{
			m_bWndVisible = FALSE;
			ShowWindow(false,false);
			HRGN hRgn =  ::CreateRectRgn(0, 0, 0, 0);
			::SetWindowRgn(m_hWnd, hRgn, TRUE);
		}
		m_nClipState = nClipState;
	}


} //namespace DirectUI