#include "StdAfx.h"
#include "UIBaseTouchWnd.h"

namespace DirectUI
{

	LPBYTE CTouchWindowUI::m_lpResourceZIPBuffer = NULL;

	DUI_BEGIN_MESSAGE_MAP(CTouchWindowUI, CNotifyPump)
		DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
	DUI_END_MESSAGE_MAP()

	void CTouchWindowUI::OnFinalMessage(HWND hWnd)
	{
		m_PaintManager.RemovePreMessageFilter(this);
		m_PaintManager.RemoveNotifier(this);
		m_PaintManager.ReapObjects(m_PaintManager.GetRoot());
	}

	LRESULT CTouchWindowUI::ResponseDefaultKeyEvent(WPARAM wParam)
	{
		if (wParam == VK_RETURN)
		{
			return FALSE;
		}
		else if (wParam == VK_ESCAPE)
		{
			return TRUE;
		}

		return FALSE;
	}

	UINT CTouchWindowUI::GetClassStyle() const
	{
		return CS_DBLCLKS;
	}

	UILIB_RESOURCETYPE CTouchWindowUI::GetResourceType() const
	{

	#if defined _DEBUG
		return UILIB_RESOURCE;
	#else
		return UILIB_FILE;
	#endif

	}

	CUIString CTouchWindowUI::GetZIPFileName() const
	{
		return _T("");
	}

	LPCTSTR CTouchWindowUI::GetResourceID() const
	{
		return _T("");
	}

	CControlUI* CTouchWindowUI::CreateControl(LPCTSTR pstrClass)
	{
		return NULL;
	}

	LRESULT CTouchWindowUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/)
	{
		if (uMsg == WM_KEYDOWN)
		{
			switch (wParam)
			{
			case VK_RETURN:
			case VK_ESCAPE:
				return ResponseDefaultKeyEvent(wParam);
			default:
				break;
			}
		}

		return FALSE;
	}

	LRESULT CTouchWindowUI::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		m_PaintManager.EnableMultiTouch(false);
		bHandled = FALSE;
		return 0;
	}

	LRESULT CTouchWindowUI::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT CTouchWindowUI::OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (::IsIconic(*this))
			bHandled = FALSE;

		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT CTouchWindowUI::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LPRECT pRect=NULL;

		if (wParam == TRUE)
		{
			LPNCCALCSIZE_PARAMS pParam = (LPNCCALCSIZE_PARAMS)lParam;
			pRect = &pParam->rgrc[0];
		}
		else
		{
			pRect=(LPRECT)lParam;
		}

		if (::IsZoomed(m_hWnd))   // 最大化时，计算当前显示器最适合宽高度
		{
			MONITORINFO oMonitor = {};
			oMonitor.cbSize = sizeof(oMonitor);
			::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
			CRect rcWork = oMonitor.rcWork;
			CRect rcMonitor = oMonitor.rcMonitor;
			rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

			pRect->right = pRect->left + rcWork.GetWidth();
			pRect->bottom = pRect->top + rcWork.GetHeight();
			return WVR_REDRAW;
		}

		return 0;
	}

	LRESULT CTouchWindowUI::OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0;
	}

	LRESULT CTouchWindowUI::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);
	
		if( !::IsZoomed(*this) )
		{
			RECT rcSizeBox = m_PaintManager.GetSizeBox();
			if( pt.y < rcClient.top + rcSizeBox.top )
			{
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
				return HTTOP;
			}
			else if( pt.y > rcClient.bottom - rcSizeBox.bottom )
			{
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
				return HTBOTTOM;
			}

			if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
		}

		RECT rcCaption = m_PaintManager.GetCaptionRect();
		if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
		{
				CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(pt));
				if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
					_tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
					_tcsicmp(pControl->GetClass(), _T("TextUI")) != 0 )
					return HTCAPTION;
		}

		return HTCLIENT;
	}

	LRESULT CTouchWindowUI::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;

		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
		CRect rcWork = oMonitor.rcWork;
		CRect rcMonitor = oMonitor.rcMonitor;
		rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

		// 计算最大化时，正确的原点坐标
		lpMMI->ptMaxPosition.x	= rcWork.left;
		lpMMI->ptMaxPosition.y	= rcWork.top;

		lpMMI->ptMaxTrackSize.x = rcWork.GetWidth();
		lpMMI->ptMaxTrackSize.y = rcWork.GetHeight();

		lpMMI->ptMinTrackSize.x = m_PaintManager.GetMinInfo().cx;
		lpMMI->ptMinTrackSize.y = m_PaintManager.GetMinInfo().cy;

		bHandled = FALSE;
		return 0;
	}

	LRESULT CTouchWindowUI::OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT CTouchWindowUI::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT CTouchWindowUI::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = m_PaintManager.GetRoundCorner();
		if(!::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0))
		{
			CRect rcWnd;
			::GetWindowRect(*this, &rcWnd);
			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(*this, hRgn, TRUE);
			::DeleteObject(hRgn);
		}
		bHandled=FALSE;
		return 0;
	}

	LRESULT CTouchWindowUI::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT CTouchWindowUI::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (wParam == SC_CLOSE)
		{
			bHandled = TRUE;
			SendMessage(WM_CLOSE);
			return 0;
		}

		BOOL bZoomed = ::IsZoomed(*this);
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		if (::IsZoomed(*this) != bZoomed)
		{
		}

		return lRes;
	}

	LRESULT CTouchWindowUI::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
		if (!GetSkinFolder().IsEmpty())
		{
			CUIString strResourcePath = m_PaintManager.GetInstancePath();
			strResourcePath += GetSkinFolder().GetData();
			m_PaintManager.SetResourcePath(strResourcePath.GetData());
		}

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

		ASSERT(pRoot);
		if (pRoot == NULL)
		{
			CUIString strXml = GetSkinFile().GetData();
			CUIString str;
			str.Format(_T("Skin %s error"), strXml.GetData());			
			MessageBox(NULL,str, _T("提示"), MB_OK|MB_ICONERROR);
			exit(1);
			return 0;
		}

		m_PaintManager.AttachDialog(pRoot);
		m_PaintManager.AddNotifier(this);
		m_PaintManager.SetBackgroundTransparent(TRUE);
		m_PaintManager.EnableMultiTouch(true);

		InitWindow();
		return 0;
	}

	LRESULT CTouchWindowUI::OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT CTouchWindowUI::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT CTouchWindowUI::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT CTouchWindowUI::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT CTouchWindowUI::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT CTouchWindowUI::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT CTouchWindowUI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch (uMsg)
		{
		case WM_CREATE:			lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:			lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:		lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
		case WM_NCACTIVATE:		lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:		lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:		lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:		lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO:	lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
		case WM_MOUSEWHEEL:		lRes = OnMouseWheel(uMsg, wParam, lParam, bHandled); break;
		case WM_SIZE:			lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_CHAR:			lRes = OnChar(uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:		lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		case WM_KEYDOWN:		lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
		case WM_KILLFOCUS:		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
		case WM_SETFOCUS:		lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
		case WM_LBUTTONUP:		lRes = OnLButtonUp(uMsg, wParam, lParam, bHandled); break;
		case WM_LBUTTONDOWN:	lRes = OnLButtonDown(uMsg, wParam, lParam, bHandled); break;
		case WM_MOUSEMOVE:		lRes = OnMouseMove(uMsg, wParam, lParam, bHandled); break;
		case WM_MOUSEHOVER:	lRes = OnMouseHover(uMsg, wParam, lParam, bHandled); break;
		default:				bHandled = FALSE; break;
		}
		if (bHandled) return lRes;

		lRes = HandleCustomMessage(uMsg, wParam, lParam, bHandled);
		if (bHandled) return lRes;
		bHandled=ProcessWindowMessage(GetHWND(), uMsg, wParam,lParam,lRes, 0);
		if (bHandled) return lRes;
		
		if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes))
			return lRes;

		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	LRESULT CTouchWindowUI::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LONG CTouchWindowUI::GetStyle()
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;

		return styleValue;
	}

	void CTouchWindowUI::OnClick(TNotifyUI& msg)
	{
		CUIString sCtrlName = msg.pSender->GetName();
		if (sCtrlName == _T("closebtn") )
		{
			Close();
			return; 
		}
		else if(sCtrlName == _T("minbtn"))
		{ 
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); 
			return; 
		}
		else if (sCtrlName == _T("maxbtn"))
		{ 
			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); 
			return; 
		}
		else if (sCtrlName == _T("restorebtn"))
		{ 
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); 
			return; 
		}
		return;
	}

	void CTouchWindowUI::Notify(TNotifyUI& msg)
	{
		return CNotifyPump::NotifyPump(msg);
	}

	CControlUI* CTouchWindowUI::GetControl(LPCTSTR strControl)
	{
		CUIString szControl(strControl);
		if (szControl.IsEmpty()) return NULL;

		CControlUI* pControlUI = m_PaintManager.FindControl(szControl);
		if (!pControlUI) return NULL;
		return pControlUI;
	}
}
