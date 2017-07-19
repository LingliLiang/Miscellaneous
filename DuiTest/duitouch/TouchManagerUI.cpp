#include "stdafx.h"
#include "TouchManagerUI.h"

#include <Strsafe.h>
namespace DirectUI {

	static UINT MapKeyState()
	{
		UINT uState = 0;
		if( ::GetKeyState(VK_CONTROL) < 0 ) uState |= MK_CONTROL;
		if( ::GetKeyState(VK_RBUTTON) < 0 ) uState |= MK_LBUTTON;
		if( ::GetKeyState(VK_LBUTTON) < 0 ) uState |= MK_RBUTTON;
		if( ::GetKeyState(VK_SHIFT) < 0 ) uState |= MK_SHIFT;
		if( ::GetKeyState(VK_MENU) < 0 ) uState |= MK_ALT;
		return uState;
	}

	typedef struct tagFINDTABINFO
	{
		CControlUI* pFocus;
		CControlUI* pLast;
		bool bForward;
		bool bNextIsIt;
	} FINDTABINFO;

	typedef struct tagFINDSHORTCUT
	{
		TCHAR ch;
		bool bPickNext;
	} FINDSHORTCUT;

	typedef struct tagTIMERINFO
	{
		CControlUI* pSender;
		UINT nLocalID;
		HWND hWnd;
		UINT uWinTimer;
		UINT uElapse;
		bool bKilled;
	} TIMERINFO;


	CTouchManagerUI::CTouchManagerUI(void)
		:m_uniTouchUtils(new TouchUtils)
	{
	}


	CTouchManagerUI::~CTouchManagerUI(void)
	{
	}

	bool CTouchManagerUI::IsMultiTouchMode() const
	{
		return m_bMultiTouch;
	}
	
	void CTouchManagerUI::EnableMultiTouch(bool bEnable)
	{
		ASSERT(::IsWindow(m_hWndPaint));
		m_bMultiTouch = bEnable;
		if(m_bMultiTouch)
		{
			m_uniTouchUtils->SetupTouchHandle(this);
			m_uniTouchUtils->SetupTouchControl(m_hWndPaint);
		}else
		{
			m_uniTouchUtils->SetupTouchHandle(NULL);
			m_uniTouchUtils->DestoryTouchControl(m_hWndPaint);
			m_mapTouchFocus.clear();
		}
	}

	bool CTouchManagerUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
	{
		if (m_hWndPaint == NULL) return false;

		TNotifyUI* pMsg = NULL;
		while (pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify.GetAt(0))) 
		{
			m_aAsyncNotify.Remove(0);
			if (pMsg->pSender)
			{
				if (pMsg->pSender->OnNotify) 
					pMsg->pSender->OnNotify(pMsg);
			}

			for(int j = 0; j < m_aNotifiers.GetSize(); j++) 
			{
				static_cast<INotifyUI*>(m_aNotifiers[j])->Notify(*pMsg);
			}

			delete pMsg;
		}

		// Cycle through listeners
		for (int i = 0; i < m_aMessageFilters.GetSize(); i++) 
		{
			bool bHandled = false;
			LRESULT lResult = static_cast<IMessageFilterUI*>(m_aMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
			if (bHandled) 
			{
				lRes = lResult;
				return true;
			}
		}
		BOOL bTouchHandle = 0;
		lRes = m_uniTouchUtils->TouchWndProc(m_hWndPaint,uMsg,wParam,lParam,bTouchHandle);
		if(bTouchHandle)
		{
			return true;
		}
		// Custom handling of events
		switch(uMsg) 
		{
		case WM_APP + 1:
			{
				for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) 
					delete static_cast<CControlUI*>(m_aDelayedCleanup[i]);
				m_aDelayedCleanup.Empty();
			}
			break;
		case WM_CLOSE:
			{
				// Make sure all matching "closing" events are sent
				TEventUI event = { 0 };
				event.ptMouse = m_ptLastMousePos;
				event.dwTimestamp = ::GetTickCount();
				if( m_pEventHover != NULL ) {
					event.Type = UIEVENT_MOUSELEAVE;
					event.pSender = m_pEventHover;
					m_pEventHover->Event(event);
				}
				if( m_pEventClick != NULL ) {
					event.Type = UIEVENT_BUTTONUP;
					event.pSender = m_pEventClick;
					m_pEventClick->Event(event);
				}

				SetFocus(NULL);

				// Hmmph, the usual Windows tricks to avoid
				// focus loss...
				HWND hwndParent = GetWindowOwner(m_hWndPaint);
				if( hwndParent != NULL ) ::SetFocus(hwndParent);
			}
			break;
		case WM_ERASEBKGND:
			{
				// We'll do the painting here...
				lRes = 1;
			}
			return true;
		case WM_PAINT:
			{
				// Should we paint?
				RECT rcPaint = { 0 };
				if (!::GetUpdateRect(m_hWndPaint, &rcPaint, FALSE)) return true;
				if (m_pRoot == NULL) 
				{
					PAINTSTRUCT ps = { 0 };
					::BeginPaint(m_hWndPaint, &ps);
					::EndPaint(m_hWndPaint, &ps);
					return true;
				}            
				// Do we need to resize anything?
				// This is the time where we layout the controls on the form.
				// We delay this even from the WM_SIZE messages since resizing can be
				// a very expensize operation.
				if (m_bUpdateNeeded) 
				{
					m_bUpdateNeeded = false;
					RECT rcClient = { 0 };
					::GetClientRect(m_hWndPaint, &rcClient);
					if (!::IsRectEmpty(&rcClient))
					{
						if (m_pRoot->IsUpdateNeeded()) 
						{
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
							SendNotify(m_pRoot, DUI_MSGTYPE_WINDOWINIT,  0, 0, false);
						}
					}
				}
				// Set focus to first control?
				if( m_bFocusNeeded ) {
					SetNextTabControl();
				}
				//
				// Render screen
				//

				// Prepare offscreen bitmap?
				if( m_bOffscreenPaint && m_hbmpOffscreen == NULL )
				{
					RECT rcClient = { 0 };
					::GetClientRect(m_hWndPaint, &rcClient);
					m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
					m_hbmpOffscreen = ::CreateCompatibleBitmap(m_hDcPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
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
					if (m_bAlphaBackground) 
					{
						if (m_hbmpBackground == NULL) 
						{
							RECT rcClient = { 0 };
							::GetClientRect(m_hWndPaint, &rcClient);
							m_hDcBackground = ::CreateCompatibleDC(m_hDcPaint);;
							m_hbmpBackground = ::CreateCompatibleBitmap(m_hDcPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
							ASSERT(m_hDcBackground);
							ASSERT(m_hbmpBackground);
							::SelectObject(m_hDcBackground, m_hbmpBackground);
							::BitBlt(m_hDcBackground, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
								ps.rcPaint.bottom - ps.rcPaint.top, ps.hdc, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
						}
						else
							::SelectObject(m_hDcBackground, m_hbmpBackground);
						::BitBlt(m_hDcOffscreen, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
							ps.rcPaint.bottom - ps.rcPaint.top, m_hDcBackground, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
					}
					m_pRoot->DoPaint(m_hDcOffscreen, ps.rcPaint);
					for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
						CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_aPostPaintControls[i]);
						pPostPaintControl->DoPostPaint(m_hDcOffscreen, ps.rcPaint);
					}
					::RestoreDC(m_hDcOffscreen, iSaveDC);

					::BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
						ps.rcPaint.bottom - ps.rcPaint.top, m_hDcOffscreen, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

					::SelectObject(m_hDcOffscreen, hOldBitmap);
				}
				else
				{
					// A standard paint job
					int iSaveDC = ::SaveDC(ps.hdc);
					m_pRoot->DoPaint(ps.hdc, ps.rcPaint);
					::RestoreDC(ps.hdc, iSaveDC);
				}
				// All Done!
				::EndPaint(m_hWndPaint, &ps);
			}
			// If any of the painting requested a resize again, we'll need
			// to invalidate the entire window once more.
			if( m_bUpdateNeeded ) {
				::InvalidateRect(m_hWndPaint, NULL, FALSE);
			}
			return true;
		case WM_PRINTCLIENT:
			{
				RECT rcClient;
				::GetClientRect(m_hWndPaint, &rcClient);
				HDC hDC = (HDC) wParam;
				int save = ::SaveDC(hDC);
				m_pRoot->DoPaint(hDC, rcClient);
				// Check for traversing children. The crux is that WM_PRINT will assume
				// that the DC is positioned at frame coordinates and will paint the child
				// control at the wrong position. We'll simulate the entire thing instead.
				if( (lParam & PRF_CHILDREN) != 0 ) {
					HWND hWndChild = ::GetWindow(m_hWndPaint, GW_CHILD);
					while( hWndChild != NULL ) {
						RECT rcPos = { 0 };
						::GetWindowRect(hWndChild, &rcPos);
						::MapWindowPoints(HWND_DESKTOP, m_hWndPaint, reinterpret_cast<LPPOINT>(&rcPos), 2);
						::SetWindowOrgEx(hDC, -rcPos.left, -rcPos.top, NULL);
						// NOTE: We use WM_PRINT here rather than the expected WM_PRINTCLIENT
						//       since the latter will not print the nonclient correctly for
						//       EDIT controls.
						::SendMessage(hWndChild, WM_PRINT, wParam, lParam | PRF_NONCLIENT);
						hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
					}
				}
				::RestoreDC(hDC, save);
			}
			break;
		case WM_GETMINMAXINFO:
			{
				LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
				if( m_szMinWindow.cx > 0 ) lpMMI->ptMinTrackSize.x = m_szMinWindow.cx;
				if( m_szMinWindow.cy > 0 ) lpMMI->ptMinTrackSize.y = m_szMinWindow.cy;
				if( m_szMaxWindow.cx > 0 ) lpMMI->ptMaxTrackSize.x = m_szMaxWindow.cx;
				if( m_szMaxWindow.cy > 0 ) lpMMI->ptMaxTrackSize.y = m_szMaxWindow.cy;
			}
			break;
		case WM_SIZE:
			{
				if( m_pFocus != NULL ) 
				{
					TEventUI event = { 0 };
					event.Type = UIEVENT_WINDOWSIZE;
					event.pSender = m_pFocus;
					event.dwTimestamp = ::GetTickCount();
					m_pFocus->Event(event);
				}

				if( m_pRoot != NULL ) m_pRoot->NeedUpdate();
			}
			return true;
		case WM_TIMER:
			{
				for (int i = 0; i < m_aTimers.GetSize(); i++) 
				{
					const TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
					if (pTimer->hWnd == m_hWndPaint && pTimer->uWinTimer == LOWORD(wParam) && pTimer->bKilled == false)
					{
						TEventUI event = { 0 };
						event.Type = UIEVENT_TIMER;
						event.pSender = pTimer->pSender;
						event.wParam = pTimer->nLocalID;
						event.dwTimestamp = ::GetTickCount();
						pTimer->pSender->Event(event);
						break;
					}
				}
			}
			break;
		case WM_MOUSEHOVER:
			{
				m_bMouseTracking = false;
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				CControlUI* pHover = FindControl(pt);
				if (!pHover) break;
				// Generate mouse hover event
				if (m_pEventHover) 
				{
					TEventUI event = { 0 };
					event.ptMouse = pt;
					event.Type = UIEVENT_MOUSEHOVER;
					event.pSender = m_pEventHover;
					event.dwTimestamp = ::GetTickCount();
					m_pEventHover->Event(event);
				}
				// Create tooltip information
				CUIString sToolTip = pHover->GetToolTip();
				if (sToolTip.IsEmpty()) return true;
				::ZeroMemory(&m_ToolTip, sizeof(TOOLINFO));
				m_ToolTip.cbSize = sizeof(TOOLINFO);
				m_ToolTip.uFlags = TTF_IDISHWND;
				m_ToolTip.hwnd = m_hWndPaint;
				m_ToolTip.uId = (UINT_PTR) m_hWndPaint;
				m_ToolTip.hinst = m_hInstance;
				m_ToolTip.lpszText = const_cast<LPTSTR>( (LPCTSTR) sToolTip );
				m_ToolTip.rect = pHover->GetPos();
				if (!m_hwndTooltip) 
				{
					m_hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndPaint, NULL, m_hInstance, NULL);
					::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) &m_ToolTip);
				}
				::SendMessage(m_hwndTooltip,TTM_SETMAXTIPWIDTH,0, pHover->GetToolTipWidth());
				::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM) &m_ToolTip);
				::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &m_ToolTip);
			}
			return true;
		case WM_MOUSELEAVE:
			{
				if (m_hwndTooltip) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
				if (m_bMouseTracking) ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) -1);
				m_bMouseTracking = false;
			}
			break;
		case WM_MOUSEMOVE:
			{
				// Start tracking this entire window again...
				if (!m_bMouseTracking)
				{
					TRACKMOUSEEVENT tme = { 0 };
					tme.cbSize = sizeof(TRACKMOUSEEVENT);
					tme.dwFlags = TME_HOVER | TME_LEAVE;
					tme.hwndTrack = m_hWndPaint;
					tme.dwHoverTime = m_hwndTooltip == NULL ? 400UL : (DWORD) ::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
					_TrackMouseEvent(&tme);
					m_bMouseTracking = true;
				}
				// Generate the appropriate mouse messages
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_ptLastMousePos = pt;
				CControlUI* pNewHover = FindControl(pt);
				if( pNewHover != NULL && pNewHover->GetManager() != this ) break;
				TEventUI event = { 0 };
				event.ptMouse = pt;
				event.dwTimestamp = ::GetTickCount();
				if( pNewHover != m_pEventHover && m_pEventHover != NULL ) 
				{
					event.Type = UIEVENT_MOUSELEAVE;
					event.pSender = m_pEventHover;
					m_pEventHover->Event(event);
					m_pEventHover = NULL;
					if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
				}
				if( pNewHover != m_pEventHover && pNewHover != NULL ) 
				{
					event.Type = UIEVENT_MOUSEENTER;
					event.pSender = pNewHover;
					pNewHover->Event(event);
					m_pEventHover = pNewHover;
				}
				if( m_pEventClick != NULL )
				{
					event.Type = UIEVENT_MOUSEMOVE;
					event.pSender = m_pEventClick;
					m_pEventClick->Event(event);
				}
				else if( pNewHover != NULL ) 
				{
					event.Type = UIEVENT_MOUSEMOVE;
					event.pSender = pNewHover;
					pNewHover->Event(event);
				}
			}
			break;
		case WM_LBUTTONDOWN:
			{
				::OutputDebugStringA("WM_LBUTTONDOWN\n");
				// We alway set focus back to our app (this helps
				// when Win32 child windows are placed on the dialog
				// and we need to remove them on focus change).
				::SetFocus(m_hWndPaint);
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_ptLastMousePos = pt;
				CControlUI* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				m_pEventClick = pControl;
				pControl->SetFocus();
				SetCapture();
				TEventUI event = { 0 };
				event.Type = UIEVENT_BUTTONDOWN;
				event.pSender = pControl;
				event.wParam = wParam;
				event.lParam = lParam;
				event.ptMouse = pt;
				event.wKeyState = (WORD)wParam;
				event.dwTimestamp = ::GetTickCount();
				pControl->Event(event);
				// No need to burden user with 3D animations
			}
			break;
		case WM_LBUTTONDBLCLK:
			{
				::SetFocus(m_hWndPaint);
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_ptLastMousePos = pt;
				CControlUI* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				SetCapture();
				TEventUI event = { 0 };
				event.Type = UIEVENT_DBLCLICK;
				event.pSender = pControl;
				event.ptMouse = pt;
				event.wKeyState = (WORD)wParam;
				event.dwTimestamp = ::GetTickCount();
				pControl->Event(event);
				m_pEventClick = pControl;
			}
			break;
		case WM_LBUTTONUP:
			{
				::OutputDebugStringA("WM_LBUTTONUP\n");
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_ptLastMousePos = pt;
				if( m_pEventClick == NULL ) break;
				ReleaseCapture();
				TEventUI event = { 0 };
				event.Type = UIEVENT_BUTTONUP;
				event.pSender = m_pEventClick;
				event.wParam = wParam;
				event.lParam = lParam;
				event.ptMouse = pt;
				event.wKeyState = (WORD)wParam;
				event.dwTimestamp = ::GetTickCount();
				m_pEventClick->Event(event);
				m_pEventClick = NULL;
			}
			break;
		case WM_RBUTTONDOWN:
			{
				::SetFocus(m_hWndPaint);
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				m_ptLastMousePos = pt;
				CControlUI* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				pControl->SetFocus();
				SetCapture();
				TEventUI event = { 0 };
				event.Type = UIEVENT_RBUTTONDOWN;
				event.pSender = pControl;
				event.wParam = wParam;
				event.lParam = lParam;
				event.ptMouse = pt;
				event.wKeyState = (WORD)wParam;
				event.dwTimestamp = ::GetTickCount();
				pControl->Event(event);
				m_pEventClick = pControl;
			}
			break;
		case WM_CONTEXTMENU:
			{
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				::ScreenToClient(m_hWndPaint, &pt);
				m_ptLastMousePos = pt;
				if( m_pEventClick == NULL ) break;
				ReleaseCapture();
				TEventUI event = { 0 };
				event.Type = UIEVENT_CONTEXTMENU;
				event.pSender = m_pEventClick;
				event.ptMouse = pt;
				event.wKeyState = (WORD)wParam;
				event.lParam = (LPARAM)m_pEventClick;
				event.dwTimestamp = ::GetTickCount();
				m_pEventClick->Event(event);
				m_pEventClick = NULL;
			}
			break;
		case WM_MOUSEWHEEL:
			{
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				::ScreenToClient(m_hWndPaint, &pt);
				m_ptLastMousePos = pt;
				CControlUI* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( pControl->GetManager() != this ) break;
				int zDelta = (int) (short) HIWORD(wParam);
				TEventUI event = { 0 };
				event.Type = UIEVENT_SCROLLWHEEL;
				event.pSender = pControl;
				event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
				event.lParam = lParam;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				pControl->Event(event);

				// Let's make sure that the scroll item below the cursor is the same as before...
				::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) MAKELPARAM(m_ptLastMousePos.x, m_ptLastMousePos.y));
			}
			break;
		case WM_CHAR:
			{
				if( m_pFocus == NULL ) break;
				TEventUI event = { 0 };
				event.Type = UIEVENT_CHAR;
				event.chKey = (TCHAR)wParam;
				event.ptMouse = m_ptLastMousePos;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				m_pFocus->Event(event);
			}
			break;
		case WM_KEYDOWN:
			{
				if( m_pFocus == NULL ) break;
				TEventUI event = { 0 };
				event.Type = UIEVENT_KEYDOWN;
				event.chKey = (TCHAR)wParam;
				event.ptMouse = m_ptLastMousePos;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				event.lParam=lParam;
				event.wParam=wParam;
				m_pFocus->Event(event);
				m_pEventKey = m_pFocus;
			}
			break;
		case WM_KEYUP:
			{
				if( m_pEventKey == NULL ) break;
				TEventUI event = { 0 };
				event.Type = UIEVENT_KEYUP;
				event.chKey = (TCHAR)wParam;
				event.ptMouse = m_ptLastMousePos;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				event.lParam=lParam;
				event.wParam=wParam;
				m_pEventKey->Event(event);
				m_pEventKey = NULL;
			}
			break;
		case WM_SETCURSOR:
			{
				if( LOWORD(lParam) != HTCLIENT ) break;
				if( m_bMouseCapture ) return true;

				POINT pt = { 0 };
				::GetCursorPos(&pt);
				::ScreenToClient(m_hWndPaint, &pt);
				CControlUI* pControl = FindControl(pt);
				if( pControl == NULL ) break;
				if( (pControl->GetControlFlags() & UIFLAG_SETCURSOR) == 0 ) break;
				TEventUI event = { 0 };
				event.Type = UIEVENT_SETCURSOR;
				event.wParam = wParam;
				event.lParam = lParam;
				event.ptMouse = pt;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				pControl->Event(event);
			}
			return true;
		case WM_NOTIFY:
			{
				LPNMHDR lpNMHDR = (LPNMHDR) lParam;
				if( lpNMHDR != NULL ) lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
				return true;
			}
			break;
		case WM_COMMAND:
			{
				if( lParam == 0 ) break;
				HWND hWndChild = (HWND) lParam;
				lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
				return true;
			}
			break;
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORSTATIC:
			{
				// Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
				// Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the WM_CTLCOLORSTATIC message.
				if( lParam == 0 ) break;
				HWND hWndChild = (HWND) lParam;
				lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
				return true;
			}
			break;
		default:
			break;
		}

		pMsg = NULL;
		while (pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify.GetAt(0))) 
		{
			m_aAsyncNotify.Remove(0);
			if (pMsg->pSender)
			{
				if (pMsg->pSender->OnNotify) 
					pMsg->pSender->OnNotify(pMsg);
			}

			for (int j = 0; j < m_aNotifiers.GetSize(); j++) 
			{
				static_cast<INotifyUI*>(m_aNotifiers[j])->Notify(*pMsg);
			}

			delete pMsg;
		}

		return false;
	}

	bool CTouchManagerUI::HandleTouchInput(__in POINT ptScreen, PTOUCHINPUT pIn)
	{
		BOOL bPrimary = pIn->dwFlags & TOUCHEVENTF_PRIMARY; // 主触控点,系统鼠标位置将紧跟主触控点
		auto FireEvent=[&](int type,WPARAM wParam,LPARAM lParam)->void
		{
			TEventUI event = { 0 };
			event.Type = type;
			event.pSender = m_mapTouchFocus[pIn->dwID].pControl;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = ptScreen;
			event.wKeyState = bPrimary;
			event.dwTimestamp = ::GetTickCount();
			if(event.pSender && event.pSender ->GetManager() == this)
			{
				DUI__Trace(_T("name %s, value: %x"),event.pSender->GetName().GetData(),event.pSender);
				event.pSender->Event(event);
			}
		};
		auto touchIter = m_mapTouchFocus.find(pIn->dwID);
		if(touchIter == m_mapTouchFocus.end())
		{
			::ScreenToClient(m_hWndPaint, &ptScreen);
			m_mapTouchFocus[pIn->dwID].pControl = FindControl(ptScreen);
		}
		if(m_mapTouchFocus[pIn->dwID].oldTouchPt.x == ptScreen.x && ptScreen.y == m_mapTouchFocus[pIn->dwID].oldTouchPt.y)
		{
			//Same point
			return false;
		}

		{
			//获取设备信息
			RID_DEVICE_INFO info;
			ZeroMemory(&info, sizeof(RID_DEVICE_INFO));
			info.cbSize = sizeof(RID_DEVICE_INFO);
			UINT size = 0;
			if (GetRawInputDeviceInfo(pIn->hSource, RIDI_DEVICEINFO, &info, &size)){
			}else{
				DWORD err = GetLastError();
			}
		}

		//条件判断的下面消息互斥
		if(pIn->dwFlags & TOUCHEVENTF_DOWN)
		{
			::OutputDebugStringA("UIEVENT_TAP_DOWN\n");
			FireEvent(UIEVENT_TAP_DOWN,0,0);
		}
		else if(pIn->dwFlags & TOUCHEVENTF_UP)
		{
			::OutputDebugStringA("UIEVENT_TAP_UP\n");
			FireEvent(UIEVENT_TAP_UP,0,0);
			m_mapTouchFocus.erase(pIn->dwID);
		}
		else if(pIn->dwFlags & TOUCHEVENTF_MOVE)
		{
			::OutputDebugStringA("UIEVENT_TAP_MOVE\n");
			FireEvent(UIEVENT_TAP_MOVE,0,0);
		}

		if(pIn->dwFlags & TOUCHEVENTF_PALM)//触控事件来自用户的手掌
		{
		}

		return true;
	}

	bool CTouchManagerUI::HandleGestureInput(__in POINT ptScreen, PTOUCHINPUT pIn)
	{
		return true;
	}

}