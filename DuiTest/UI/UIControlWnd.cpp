#include "stdafx.h"
#include "UIControlWnd.h"

namespace DirectUI
{

	CControlWndUI::CControlWndUI()
		:m_bHasParent(TRUE)
	{
	}

	CControlWndUI::~CControlWndUI()
	{
	}

	//-----------------------------CControlUI---------------------------------
	void CControlWndUI::DoInit()
	{
		if(m_pManager && m_hWnd == NULL)
		{
			Create(m_bHasParent ? m_pManager->GetPaintWindow() : NULL,
				GetWindowClassName(),
				UI_WNDSTYLE_CHILD,
				UI_WNDSTYLE_CONTAINER);
		}
		__super::DoInit();
	}

	void CControlWndUI::Invalidate()
	{
		::InvalidateRect(m_hWnd,&m_rcItem,TRUE);
		__super::Invalidate();
	}

	void CControlWndUI::SetInternVisible(bool bVisible)
	{
		CControlUI::SetInternVisible(bVisible);
		::ShowWindow(m_hWnd, bVisible); 
	}

	void CControlWndUI::SetPos(RECT rc)
	{
		__super::SetPos(rc);
		::SetWindowPos(m_hWnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	void CControlWndUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("hasparent")) == 0) SetHasParentWnd(_tcscmp(pstrValue, _T("true")) == 0);
		else 
			__super::SetAttribute(pstrName, pstrValue);
	}

	BOOL CControlWndUI::IsHasParentWnd() const
	{
		return m_bEnabled;
	}

	void CControlWndUI::SetHasParentWnd(BOOL b)
	{
		if (m_bHasParent == b) return;
		m_bHasParent = b;
	}

	//------------------------------CWindowWnd--------------------------------

	LPCTSTR CControlWndUI::GetWindowClassName() const
	{
		return _T("ControlWndUI");
	}

	LRESULT CControlWndUI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		Slot_PreHandleMessage.Active(uMsg,wParam,lParam);
		if(uMsg == MSG_HANDLED) return lRes;
		switch (uMsg)
		{
		case WM_CREATE:			lRes = OnCreate( wParam, lParam); break;
		case WM_PAINT:			lRes = OnPaint( wParam, lParam); break;
		case WM_CLOSE:			lRes = OnClose( wParam, lParam); break;
		//case WM_DESTROY:		lRes = OnDestroy( wParam, lParam); break;
		//case WM_NCACTIVATE:		lRes = OnNcActivate( wParam, lParam); break;
		//case WM_NCCALCSIZE:		lRes = OnNcCalcSize( wParam, lParam); break;
		//case WM_NCPAINT:		lRes = OnNcPaint( wParam, lParam); break;
		//case WM_NCHITTEST:		lRes = OnNcHitTest( wParam, lParam); break;
		//case WM_GETMINMAXINFO:	lRes = OnGetMinMaxInfo( wParam, lParam); break;
		//case WM_MOUSEWHEEL:		lRes = OnMouseWheel( wParam, lParam); break;
		//case WM_SIZE:			lRes = OnSize( wParam, lParam); break;
		//case WM_SYSCOMMAND:		lRes = OnSysCommand( wParam, lParam); break;
		case WM_CHAR:			lRes = OnChar( wParam, lParam); break;
		case WM_KEYDOWN:		lRes = OnKeyDown( wParam, lParam); break;
		//case WM_KILLFOCUS:		lRes = OnKillFocus( wParam, lParam); break;
		//case WM_SETFOCUS:		lRes = OnSetFocus( wParam, lParam); break;
		case WM_LBUTTONUP:		lRes = OnLButtonUp( wParam, lParam); break;
		case WM_LBUTTONDOWN:	lRes = OnLButtonDown( wParam, lParam); break;
		case WM_LBUTTONDBLCLK:	lRes = OnLButtonDBClick( wParam, lParam); break;
		case WM_MOUSEMOVE:		lRes = OnMouseMove( wParam, lParam); break;
		//case WM_MOUSEHOVER:	lRes = OnMouseHover( wParam, lParam); break;
		case WM_SETCURSOR:		lRes = OnSetCursor( wParam, lParam); break;
		default:	 break;
		}
		if(lRes) return lRes;
		return __super::HandleMessage(uMsg, wParam, lParam);
	}

	void CControlWndUI::OnFinalMessage(HWND hWnd)
	{
	}

	LRESULT CControlWndUI::OnCreate(WPARAM wParam, LPARAM lParam)
	{
		return 0;
	}

	LRESULT CControlWndUI::OnPaint(WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT ps = {0};
		HDC hDC = BeginPaint(m_hWnd, &ps);
		 // »æÖÆÑ­Ðò£º±³¾°ÑÕÉ«->±³¾°Í¼->×´Ì¬Í¼->ÎÄ±¾->±ß¿ò
		GetClientRect(m_hWnd, &m_rcPaint);

		if (m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0)
		{
			RECT rcItem = m_rcItem;
			rcItem.left = rcItem.top = 0;
			CRenderClip roundClip;
			CRenderClip::GenerateRoundClip(hDC, m_rcPaint,  rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
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
		EndPaint(m_hWnd, &ps);
		return 0;
	}

	LRESULT CControlWndUI::OnLButtonDown(WPARAM wParam, LPARAM lParam)
	{
		POINT pt = {  (LONG)(GET_X_LPARAM(lParam) / 1), (LONG)(GET_Y_LPARAM(lParam) / 1) };
		return 0;
	}

	LRESULT CControlWndUI::OnMouseMove(WPARAM wParam, LPARAM lParam)
	{
		return 0;
	}

	LRESULT CControlWndUI::OnLButtonUp(WPARAM wParam, LPARAM lParam)
	{
		return 0;
	}

	LRESULT CControlWndUI::OnLButtonDBClick(WPARAM wParam, LPARAM lParam)
	{
		return 0;
	}

	LRESULT CControlWndUI::OnKeyDown(WPARAM wParam, LPARAM lParam)
	{
		switch(wParam)
		{
		case VK_UP:
			{
			}
			break;

		case VK_DOWN:
			{
			}
			break;

		case VK_LEFT:
			{
			}
			break;

		case VK_RIGHT:
			{
			}
			break;
		case VK_OEM_4: //[
			if(GetKeyState( VK_CONTROL ) & 0x8000)
			{
			}
			break;
		case VK_OEM_6: //]
			if(GetKeyState( VK_CONTROL ) & 0x8000)
			{
			}
			break;

		case VK_DELETE:
		case VK_BACK:
			break;
		case 'A':
			if(GetKeyState( VK_CONTROL ) & 0x8000)
			{
			}
			break;
		case 'V':
			if(::GetAsyncKeyState(VK_CONTROL))
			{
				if(::OpenClipboard(*this))
				{
					UINT uiFormat = (sizeof(TCHAR) == sizeof(WCHAR))?CF_UNICODETEXT:CF_TEXT;
					HGLOBAL hText = ::GetClipboardData(uiFormat);
					if(hText != NULL)
					{
						LPCTSTR lpStr = (LPCTSTR)::GlobalLock(hText);
						if(lpStr != NULL)
						{
						}
						::GlobalUnlock(hText);
					}
				}
				::CloseClipboard();
			}
			break;
		default:
			break;
		}
		return 0;
	}

	LRESULT CControlWndUI::OnChar(WPARAM wParam, LPARAM lParam)
	{
		switch(wParam)
		{
		case VK_BACK:
			break;

		case 0x1://ctrl+a
		case 0x3://ctrl+c
		case 0x16://ctrl+v
			break;
		default:
			{
				TCHAR ch[2] = {0};
				ch[0] = (TCHAR)wParam;
			}
		}
		return 0;
	}

	LRESULT CControlWndUI::OnSetCursor(WPARAM wParam, LPARAM lParam)
	{
		return 0;
	}

	LRESULT CControlWndUI::OnClose(WPARAM wParam, LPARAM lParam)
	{
		return 0;
	}


}