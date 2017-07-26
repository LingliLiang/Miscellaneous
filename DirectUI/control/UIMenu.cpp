#include "StdAfx.h"
#include "UIMenu.h"

namespace DirectUI
{
	ContextMenuObserver s_context_menu_observer;

	CMenuUI::CMenuUI()
	{
		m_dwAlignment = eMenuAlignment_Left | eMenuAlignment_Top;
		m_bAlpha = 255;
		m_cColorkey = 255;
		m_pWindow = 0;
		if (GetHeader())
			GetHeader()->SetVisible(false);
	}

	CMenuUI::~CMenuUI()
	{
	}

	void CMenuUI::Clear()
	{

		for(int i  = 0;i  < GetCount();i++)
		{	
			CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(i));
			if(pControl)
			{
				CMenuItemUI* pMenuItem = static_cast<CMenuItemUI*>(pControl->GetInterface(DUI_CTR_MENUITEM));
				if (pMenuItem == NULL)
					continue;

				pControl->OnNotify.Empty();
			}
		}
	}

	LPCTSTR CMenuUI::GetClass() const
	{
		return _T("MenuUI");
	}

	LPVOID CMenuUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_MENU) == 0) 
			return static_cast<CMenuUI*>(this);

		return CListUI::GetInterface(pstrName);
	}

	void CMenuUI::DoEvent(TEventUI& event)
	{
		return __super::DoEvent(event);
	}

	void CMenuUI::SetParent(CMenuWnd *pWnd)
	{
		m_pWindow = pWnd;
	}

	bool CMenuUI::Add(CControlUI* pControl)
	{
		if (!pControl) 
			return false;

		CMenuItemUI* pMenuItem = static_cast<CMenuItemUI*>(pControl->GetInterface(DUI_CTR_MENUITEM));
		if (pMenuItem == NULL)
			return false;
		
		pControl->OnNotify += MakeDelegate(this, &CMenuUI::OnNotify);
		
		for (int i = 0; i < pMenuItem->GetCount(); ++i)
		{
			if (pMenuItem->GetItemAt(i)->GetInterface(DUI_CTR_MENUITEM) != NULL)
			{
				(static_cast<CMenuItemUI*>(pMenuItem->GetItemAt(i)->GetInterface(DUI_CTR_MENUITEM)))->SetInternVisible(false);
			}
		}

		return CListUI::Add(pControl);
	}

	bool CMenuUI::AddAt(CControlUI* pControl, int iIndex)
	{
		if (!pControl) 
			return false;

		CMenuItemUI* pMenuItem = static_cast<CMenuItemUI*>(pControl->GetInterface(DUI_CTR_MENUITEM));
		if (pMenuItem == NULL)
			return false;

		pControl->OnNotify += MakeDelegate(this, &CMenuUI::OnNotify);

		for (int i = 0; i < pMenuItem->GetCount(); ++i)
		{
			if (pMenuItem->GetItemAt(i)->GetInterface(DUI_CTR_MENUITEM) != NULL)
			{
				(static_cast<CMenuItemUI*>(pMenuItem->GetItemAt(i)->GetInterface(DUI_CTR_MENUITEM)))->SetInternVisible(false);
			}
		}

		return CListUI::AddAt(pControl, iIndex);
	}

	int CMenuUI::GetItemIndex(CControlUI* pControl) const
	{
		if (!pControl)
			return false;

		CMenuItemUI* pMenuItem = static_cast<CMenuItemUI*>(pControl->GetInterface(DUI_CTR_MENUITEM));
		if (pMenuItem == NULL)
			return -1;

		return __super::GetItemIndex(pControl);
	}

	bool CMenuUI::SetItemIndex(CControlUI* pControl, int iIndex)
	{
		if (!pControl) 
			return false;

		CMenuItemUI* pMenuItem = static_cast<CMenuItemUI*>(pControl->GetInterface(DUI_CTR_MENUITEM));
		if (pMenuItem == NULL)
			return false;

		return __super::SetItemIndex(pControl, iIndex);
	}

	bool CMenuUI::Remove(CControlUI* pControl)
	{
		if (!pControl) 
			return false;

		CMenuItemUI* pMenuItem = static_cast<CMenuItemUI*>(pControl->GetInterface(DUI_CTR_MENUITEM));
		if (pMenuItem == NULL)
			return false;

		return __super::Remove(pControl);
	}

	bool CMenuUI::OnNotify(void* pNotify)
	{
		TNotifyUI* pMsg = (TNotifyUI*)pNotify;
		if (!pMsg) return false;

		if (pMsg->sType == DUI_MSGTYPE_ITEMCLICK)
		{	
			if (m_pWindow)
				m_pWindow->Notify(pMsg);
		}

		return false;
	}

	SIZE CMenuUI::EstimateSize(SIZE szAvailable)
	{
		int cxFixed = 0;
		int cyFixed = 0;
		for (int it = 0; it < GetCount(); it++) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(it));
			if (!pControl->IsVisible()) 
				continue;

			SIZE sz = pControl->EstimateSize(szAvailable);
			cyFixed += sz.cy;

			if (cxFixed < sz.cx)
				cxFixed = sz.cx;
		}
		
		return CSize(cxFixed, cyFixed);
	}

	void CMenuUI::SetMenuAlign(DWORD dwAlignment)
	{
		m_dwAlignment = dwAlignment;
	}

	DWORD CMenuUI::GetMenuAlign() const
	{
		return m_dwAlignment;
	}

	void CMenuUI::SetMenuAlpha(BYTE ucAlpha)
	{
		m_bAlpha = ucAlpha;
	}

	BYTE CMenuUI::GetMenuAlpha() const
	{
		return m_bAlpha;
	}

	void CMenuUI::SetMenuColorkey(BYTE ucColorkey)
	{
		m_cColorkey = ucColorkey;
	}

	BYTE CMenuUI::GetMenuColorkey() const
	{
		return m_cColorkey;
	}

	void CMenuUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("menualign")) == 0) 
		{
			 if (_tcsstr(pstrValue, _T("lefttop")) != NULL) 
			 {
				 m_dwAlignment = eMenuAlignment_Left | eMenuAlignment_Top;
			 }
			 else if (_tcsstr(pstrValue, _T("righttop")) != NULL) 
			 {
				 m_dwAlignment = eMenuAlignment_Right | eMenuAlignment_Top;
			 }
			 else if(_tcsstr(pstrValue, _T("leftbottom")) != NULL)
			 {
				 m_dwAlignment = eMenuAlignment_Left | eMenuAlignment_Bottom;
			 }
			 else if(_tcsstr(pstrValue, _T("rightbottom")) != NULL)
			 {
				 m_dwAlignment = eMenuAlignment_Right | eMenuAlignment_Bottom;
			 }
		}
		else if (_tcscmp(pstrName, _T("colorkey")) == 0) 
		{
			SetMenuColorkey((BYTE)_ttoi(pstrValue));
		}
		else if (_tcscmp(pstrName, _T("alpha")) == 0) 
		{
			SetMenuAlpha((BYTE)_ttoi(pstrValue));
		}

		CListUI::SetAttribute(pstrName, pstrValue);
	}

	void CMenuUI::SetListItemAutoDestroy(bool bAuto)
	{
		if (m_pList)
			m_pList->SetAutoDestroy(bAuto);
	}
	
	bool CMenuUI::IsListItemAutoDestroy()
	{
		if (m_pList)
			return m_pList->IsAutoDestroy();
		return true;
	}


	/////////////////////////////////////////////////////////////////////////////////////
	// CMenuWnd

	CMenuWnd::CMenuWnd(HWND hParent)
		:m_hParent(hParent),m_pOwner(0),m_pMenu(0),m_xml(_T("")),m_pNotify(0),m_bUserSelected(FALSE)
	{
	}

	BOOL CMenuWnd::UpdateMenuItem(LPCTSTR lpszName, LPCTSTR lpszElement, LPCTSTR lpszInfo)
	{
		if (!lpszName || !lpszElement || !lpszInfo)
			return FALSE;

		CControlUI* pControlUI = m_pm.FindControl(lpszName);
		if (!pControlUI) return FALSE;

		pControlUI->SetAttribute(lpszElement, lpszInfo);
		return TRUE;
	}

	BOOL CMenuWnd::Receive(ContextMenuParam param)
	{
		switch (param.wParam)
		{
		case 1:
			Close();
			break;
		case 2:
			{
				HWND hParent = GetParent(m_hWnd);
				while (hParent)
				{
					if (hParent == param.hWnd)
					{
						Close();
						break;
					}
					hParent = GetParent(hParent);
				}
			}
			break;
		case 3:
			if(param.hWnd == GetHWND())
				SendSelectNotify();
			Close();
			break;
		default:
			break;
		}

		return TRUE;
	}

	void CMenuWnd::Init(INotifyUI *pNotify, CMenuItemUI* pOwner, STRINGorID xml, LPCTSTR pSkinType, POINT point)
	{
		m_pNotify = pNotify;
		m_BasedPoint = point;
		m_pOwner = pOwner;
		m_pMenu = NULL;

		if (pSkinType)
			m_sType = pSkinType;

		m_xml = xml;
		s_context_menu_observer.AddReceiver(this);

		Create((m_pOwner == NULL) ? m_hParent : m_pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP, WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST, CRect());
		// HACK: Don't deselect the parent's caption
		HWND hWndParent = m_hWnd;
		while (::GetParent(hWndParent) != NULL) 
			hWndParent = ::GetParent(hWndParent);

		::ShowWindow(m_hWnd, SW_SHOW);
	#if defined(WIN32) && !defined(UNDER_CE)
		::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
	#endif	
	}

	LPCTSTR CMenuWnd::GetWindowClassName() const
	{
		return _T("MenuWnd");
	}

	void CMenuWnd::OnFinalMessage(HWND hWnd)
	{
		RemoveObserver();
		if (m_pOwner) 
		{
			for (int i = 0; i < m_pOwner->GetCount(); i++)
			{
				if (static_cast<CMenuItemUI*>(m_pOwner->GetItemAt(i)->GetInterface(DUI_CTR_MENUITEM)) != NULL)
				{
					(static_cast<CMenuItemUI*>(m_pOwner->GetItemAt(i)))->SetOwner(m_pOwner->GetParent());
					(static_cast<CMenuItemUI*>(m_pOwner->GetItemAt(i)))->SetVisible(false);
					(static_cast<CMenuItemUI*>(m_pOwner->GetItemAt(i)->GetInterface(DUI_CTR_MENUITEM)))->SetInternVisible(false);
				}
			}
			m_pOwner->m_pWindow = NULL;
			m_pOwner->m_uButtonState &= ~ UISTATE_PUSHED;
			m_pOwner->Invalidate();
		}
		
		delete this;
	}

	void CMenuWnd::Notify(TNotifyUI *msg)
	{
		if (m_bUserSelected && m_pNotify)
			m_pNotify->Notify(*msg);
	}

	LRESULT CMenuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_CREATE) 
		{
			if (m_pOwner) 
			{
				LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
				styleValue &= ~WS_CAPTION;
				::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
				RECT rcClient;
				::GetClientRect(*this, &rcClient);
				::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
					rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

				m_pm.Init(m_hWnd);
				// The trick is to add the items to the new container. Their owner gets
				// reassigned by this operation - which is why it is important to reassign
				// the items back to the righfull owner/manager when the window closes.
				m_pMenu = new CMenuUI();
				m_pm.UseParentResource(m_pOwner->GetManager());
				m_pMenu->SetParent(this);
				m_pMenu->SetManager(&m_pm, NULL, true);

				LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(DUI_CTR_MENU);
				if (pDefaultAttributes) 
					m_pMenu->ApplyAttributeList(pDefaultAttributes);
				
				m_pm.SetTransparent(m_pMenu->GetMenuAlpha());
			
				/*m_pMenu->SetBkColor(0xFFFFFFFF);*/
				/*m_pMenu->SetBorderColor(0xFF85E4FF);*/
				m_pMenu->SetBorderSize(0);
				m_pMenu->SetListItemAutoDestroy(false);
				m_pMenu->EnableScrollBar();

				for (int i = 0; i < m_pOwner->GetCount(); i++) 
				{
					if (m_pOwner->GetItemAt(i)->GetInterface(DUI_CTR_MENUITEM) != NULL)
					{
						(static_cast<CMenuItemUI*>(m_pOwner->GetItemAt(i)))->SetOwner(m_pMenu);
						(static_cast<CMenuItemUI*>(m_pOwner->GetItemAt(i)))->SetNotify(m_pNotify);
						m_pMenu->Add(static_cast<CControlUI*>(m_pOwner->GetItemAt(i)));
					}
				}

				m_pm.AttachDialog(m_pMenu);
				// Position the popup window in absolute space
				RECT rcOwner = m_pOwner->GetPos();
				RECT rc = rcOwner;

				int cxFixed = 0;
				int cyFixed = 0;

	#if defined(WIN32) && !defined(UNDER_CE)
				MONITORINFO oMonitor = {}; 
				oMonitor.cbSize = sizeof(oMonitor);
				::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
				CRect rcWork = oMonitor.rcWork;
	#else
				CDuiRect rcWork;
				GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWork);
	#endif
				SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };

				for (int it = 0; it < m_pOwner->GetCount(); it++) 
				{
					if (m_pOwner->GetItemAt(it)->GetInterface(DUI_CTR_MENUITEM) != NULL)
					{
						CControlUI* pControl = static_cast<CControlUI*>(m_pOwner->GetItemAt(it));
						SIZE sz = pControl->EstimateSize(szAvailable);
						cyFixed += sz.cy;

						if (cxFixed < sz.cx)
							cxFixed = sz.cx;
					}
				}

				cyFixed += 4;
				cxFixed += 4;

				RECT rcWindow;
				GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWindow);

				rc.top = rcOwner.top;
				rc.bottom = rc.top + cyFixed;
				::MapWindowRect(m_pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
				rc.left = rcWindow.right;
				rc.right = rc.left + cxFixed;
				rc.right += 2;

				bool bReachBottom = false;
				bool bReachRight = false;
				LONG chRightAlgin = 0;
				LONG chBottomAlgin = 0;

				RECT rcPreWindow = {0};
				ContextMenuObserver::Iterator<BOOL, ContextMenuParam> iterator(s_context_menu_observer);
				IReceiver<BOOL, ContextMenuParam>* pReceiver = iterator.Next();
				while (pReceiver) 
				{
					CMenuWnd* pContextMenu = dynamic_cast<CMenuWnd*>(pReceiver);
					if (pContextMenu) 
					{
						GetWindowRect(pContextMenu->GetHWND(), &rcPreWindow);

						bReachRight = rcPreWindow.left >= rcWindow.right;
						bReachBottom = rcPreWindow.top >= rcWindow.bottom;
						if (pContextMenu->GetHWND() == m_pOwner->GetManager()->GetPaintWindow() 
							||  bReachBottom || bReachRight)
							break;
					}

					pReceiver = iterator.Next();
				}

				::MapWindowRect(m_pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rcOwner);

				if (bReachBottom)
				{
					rc.bottom = rcWindow.top;
					rc.top = rc.bottom - cyFixed;
				}

				if (bReachRight)
				{
					rc.right = rcWindow.left;
					rc.left = rc.right - cxFixed;
				}

				if (rc.bottom > rcWork.bottom)
				{
					rc.bottom = rcOwner.bottom;
					rc.top = rc.bottom - cyFixed;
				}

				if (rc.right > rcWork.right)
				{
					rc.right = rcWindow.left;
					rc.left = rc.right - cxFixed;

// 					rc.top = rcWindow.bottom;
// 					rc.bottom = rc.top + cyFixed;
				}

				if (rc.top < rcWork.top)
				{
					rc.top = rcOwner.top;
					rc.bottom = rc.top + cyFixed;
				}

				if (rc.left < rcWork.left)
				{
					rc.left = rcWindow.right;
					rc.right = rc.left + cxFixed;
				}

				MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE);
			}
			else 
			{
				m_pm.Init(m_hWnd);

				CDialogBuilder builder;
				CMenuBuilderCallback menuCallback;

				CControlUI* pRoot = builder.Create(m_xml, m_sType.GetData(), &menuCallback, &m_pm);
				m_pm.AttachDialog(pRoot);

	#if defined(WIN32) && !defined(UNDER_CE)
				MONITORINFO oMonitor = {}; 
				oMonitor.cbSize = sizeof(oMonitor);
				::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
				CRect rcWork = oMonitor.rcWork;
	#else
				CDuiRect rcWork;
				GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWork);
	#endif
				SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };
				szAvailable = pRoot->EstimateSize(szAvailable);
				m_pm.SetInitSize(szAvailable.cx, szAvailable.cy);

				m_pMenu = static_cast<CMenuUI*>(pRoot);
				if (!m_pMenu) return false;

				for(int i = 0;i < m_pMenu->GetCount(); i++)
				{
					static_cast<CMenuItemUI *>(m_pMenu->GetItemAt(i))->SetNotify(m_pNotify);
				}

				m_pMenu->SetParent(this);
				DWORD dwAlignment = m_pMenu->GetMenuAlign();
				BYTE bAlpha = m_pMenu->GetMenuAlpha();
				BYTE bColorkey = m_pMenu->GetMenuColorkey();
				::SetLayeredWindowAttributes(m_hWnd, RGB(bColorkey, bColorkey, bColorkey), bAlpha, LWA_COLORKEY|LWA_ALPHA);

				SIZE szInit = m_pm.GetInitSize();
				CRect rc;
				CPoint point = m_BasedPoint;
				rc.left = point.x;
				rc.top = point.y;
				rc.right = rc.left + szInit.cx;
				rc.bottom = rc.top + szInit.cy + 4;

				int nWidth = rc.GetWidth();
				int nHeight = rc.GetHeight();

				if(dwAlignment & eMenuAlignment_Left)
				{
					rc.left = point.x;
					rc.right = rc.left + nWidth;
				}
				if (dwAlignment & eMenuAlignment_Right)
				{
					rc.right = point.x;
					rc.left = rc.right - nWidth;
				}

				if (dwAlignment & eMenuAlignment_Bottom)
				{
					rc.bottom = point.y;
					rc.top = rc.bottom - nHeight;
				}

				SetForegroundWindow(m_hWnd);
				MoveWindow(m_hWnd, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), FALSE);
				SetWindowPos(m_hWnd, HWND_TOPMOST, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), SWP_SHOWWINDOW);
			}

			return 0;
		}
		else if (uMsg == WM_CLOSE) 
		{
			if (m_pOwner)
			{
				m_pMenu->Clear();
				m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
				m_pOwner->SetPos(m_pOwner->GetPos());
				m_pOwner->SetFocus();
			}
		}
		else if (uMsg == WM_RBUTTONDOWN || uMsg == WM_CONTEXTMENU || uMsg == WM_RBUTTONUP || uMsg == WM_RBUTTONDBLCLK)
		{
			return 0L;
		}
		else if (uMsg == WM_KILLFOCUS)
		{
			HWND hFocusWnd = (HWND)wParam;

			BOOL bInMenuWindowList = FALSE;
			ContextMenuParam param;
			param.hWnd = GetHWND();

			ContextMenuObserver::Iterator<BOOL, ContextMenuParam> iterator(s_context_menu_observer);
			IReceiver<BOOL, ContextMenuParam>* pReceiver = iterator.Next();
			while (pReceiver) 
			{
				CMenuWnd* pContextMenu = dynamic_cast<CMenuWnd*>(pReceiver);
				if (pContextMenu != NULL && pContextMenu->GetHWND() ==  hFocusWnd)
				{
					bInMenuWindowList = TRUE;
					break;
				}

				pReceiver = iterator.Next();
			}

			if (!bInMenuWindowList) 
			{
				param.wParam = 1;
				s_context_menu_observer.RBroadcast(param);

				return 0;
			}
		}
		else if (uMsg == WM_KEYDOWN)
		{
			if (wParam == VK_ESCAPE)
			{
				Close();
			} else if (wParam == VK_RETURN) {
				SendSelectNotify();
				Close();
			}
		}

		LRESULT lRes = 0;
		if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) 
			return lRes;

		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	void CMenuWnd::SendSelectNotify()
	{
		if (m_pMenu) {
			int i = m_pMenu->GetCurSel();
			if (i>=0&&i<m_pMenu->GetCount()) {
				CControlUI *pItem=m_pMenu->GetItemAt(i);
				if (pItem) {
					m_bUserSelected=TRUE;
					m_pm.SendNotify(pItem, DUI_MSGTYPE_ITEMCLICK);
					m_bUserSelected=FALSE;
				}
			}
		}
	}

	void CMenuWnd::CalcRect()
	{
		if (m_pOwner) 
		{
			LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
			styleValue &= ~WS_CAPTION;
			::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
			RECT rcClient;
			::GetClientRect(*this, &rcClient);
			::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
				rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

			// Position the popup window in absolute space
				RECT rcOwner = m_pOwner->GetPos();
				RECT rc = rcOwner;

				int cxFixed = 0;
				int cyFixed = 0;

	#if defined(WIN32) && !defined(UNDER_CE)
				MONITORINFO oMonitor = {}; 
				oMonitor.cbSize = sizeof(oMonitor);
				::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
				CRect rcWork = oMonitor.rcWork;
	#else
				CDuiRect rcWork;
				GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWork);
	#endif
				SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };

				for (int it = 0; it < m_pOwner->GetCount(); it++) 
				{
					if (m_pOwner->GetItemAt(it)->GetInterface(DUI_CTR_MENUITEM) != NULL)
					{
						CControlUI* pControl = static_cast<CControlUI*>(m_pOwner->GetItemAt(it));
						SIZE sz = pControl->EstimateSize(szAvailable);
						cyFixed += sz.cy;

						if (cxFixed < sz.cx)
							cxFixed = sz.cx;
					}
				}

				cyFixed += 4;
				cxFixed += 4;

				RECT rcWindow;
				GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWindow);

				rc.top = rcOwner.top;
				rc.bottom = rc.top + cyFixed;
				::MapWindowRect(m_pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
				rc.left = rcWindow.right;
				rc.right = rc.left + cxFixed;
				rc.right += 2;

				bool bReachBottom = false;
				bool bReachRight = false;
				LONG chRightAlgin = 0;
				LONG chBottomAlgin = 0;

				RECT rcPreWindow = {0};
				ContextMenuObserver::Iterator<BOOL, ContextMenuParam> iterator(s_context_menu_observer);
				IReceiver<BOOL, ContextMenuParam>* pReceiver = iterator.Next();
				while (pReceiver) 
				{
					CMenuWnd* pContextMenu = dynamic_cast<CMenuWnd*>(pReceiver);
					if (pContextMenu) 
					{
						GetWindowRect(pContextMenu->GetHWND(), &rcPreWindow);

						bReachRight = rcPreWindow.left >= rcWindow.right;
						bReachBottom = rcPreWindow.top >= rcWindow.bottom;
						if (pContextMenu->GetHWND() == m_pOwner->GetManager()->GetPaintWindow() 
							||  bReachBottom || bReachRight)
							break;
					}

					pReceiver = iterator.Next();
				}

				if (bReachBottom)
				{
					rc.bottom = rcWindow.top;
					rc.top = rc.bottom - cyFixed;
				}

				if (bReachRight)
				{
					rc.right = rcWindow.left;
					rc.left = rc.right - cxFixed;
				}

				if (rc.bottom > rcWork.bottom)
				{
					rc.bottom = rc.top;
					rc.top = rc.bottom - cyFixed;
				}

				if (rc.right > rcWork.right)
				{
					rc.right = rcWindow.left;
					rc.left = rc.right - cxFixed;

					rc.top = rcWindow.bottom;
					rc.bottom = rc.top + cyFixed;
				}

				if (rc.top < rcWork.top)
				{
					rc.top = rcOwner.top;
					rc.bottom = rc.top + cyFixed;
				}

				if (rc.left < rcWork.left)
				{
					rc.left = rcWindow.right;
					rc.right = rc.left + cxFixed;
				}

				MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE);
			}
			else 
			{
				CControlUI* pRoot = m_pm.GetRoot();;
				if (!pRoot)
					return;

	#if defined(WIN32) && !defined(UNDER_CE)
				MONITORINFO oMonitor = {}; 
				oMonitor.cbSize = sizeof(oMonitor);
				::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
				CRect rcWork = oMonitor.rcWork;
	#else
				CDuiRect rcWork;
				GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWork);
	#endif
				SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };
				szAvailable = pRoot->EstimateSize(szAvailable);
				if(pRoot->GetFixedHeight()>0)
				{
					szAvailable.cy = min(szAvailable.cy,pRoot->GetFixedHeight());
				}
				m_pm.SetInitSize(szAvailable.cx, szAvailable.cy);

				m_pMenu = static_cast<CMenuUI*>(pRoot);
				if (!m_pMenu) return;

				m_pMenu->SetParent(this);
				m_pMenu->EnableScrollBar();
				if(m_pMenu->GetHorizontalScrollBar())
				{
					m_pMenu->GetHorizontalScrollBar()->SetAttribute(_T("showemptybuttonrect"),_T("false"));
					m_pMenu->GetHorizontalScrollBar()->SetFixedHeight(1);
				}
				if(m_pMenu->GetVerticalScrollBar())
				{
					m_pMenu->GetVerticalScrollBar()->SetAttribute(_T("showemptybuttonrect"),_T("false"));
					m_pMenu->GetVerticalScrollBar()->SetFixedWidth(1);
				}

				DWORD dwAlignment = m_pMenu->GetMenuAlign();
				BYTE bAlpha = m_pMenu->GetMenuAlpha();
				BYTE bColorkey = m_pMenu->GetMenuColorkey();
				::SetLayeredWindowAttributes(m_hWnd, RGB(bColorkey, bColorkey, bColorkey), bAlpha, LWA_COLORKEY|LWA_ALPHA);

				SIZE szInit = m_pm.GetInitSize();
				CRect rc;
				CPoint point = m_BasedPoint;
				rc.left = point.x;
				rc.top = point.y;
				rc.right = rc.left + szInit.cx;
				rc.bottom = rc.top + szInit.cy + 4;

				int nWidth = rc.GetWidth();
				int nHeight = rc.GetHeight();

				if (dwAlignment & eMenuAlignment_Right)
				{
					rc.right = point.x;
					rc.left = rc.right - nWidth;
				}

				if (dwAlignment & eMenuAlignment_Bottom)
				{
					rc.bottom = point.y;
					rc.top = rc.bottom - nHeight;
				}

				if(oMonitor.rcMonitor.bottom < rc.bottom)
				{
					rc.Offset(0,oMonitor.rcMonitor.bottom - rc.bottom);
				}

				SetForegroundWindow(m_hWnd);
				MoveWindow(m_hWnd, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), FALSE);
				SetWindowPos(m_hWnd, HWND_TOPMOST, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), SWP_SHOWWINDOW);
			}
	}

	bool CMenuWnd::Add(CControlUI* pControl)
	{
		if (m_pMenu)
			return m_pMenu->Add(pControl);
		return false;
	}

	bool CMenuWnd::Remove(int nIdx)
	{
		if (m_pMenu) {
			if (m_pMenu->GetCount()>nIdx) {
				CControlUI *pUI=m_pMenu->GetItemAt(nIdx);
				if (pUI) {
					m_pMenu->Remove(pUI);
				}
			}
			return true;
		}
		return false;
	}

	CControlUI* CMenuWnd::GetAt(int nIdx)
	{
		if (m_pMenu) 
			return m_pMenu->GetItemAt(nIdx);
		return NULL;
	}

	int CMenuWnd::GetCount()
	{
		if (m_pMenu)
		{
			return m_pMenu->GetCount();
		}

		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	// MenuElementUI

	CMenuItemUI::CMenuItemUI()
		:m_pWindow(NULL)
	{
		m_cxyFixed.cy = 25;
		m_bMouseChildEnabled = true;

		SetMouseChildEnabled(false);
		m_pNotify = NULL;
	}

	CMenuItemUI::~CMenuItemUI()
	{
	}

	LPCTSTR CMenuItemUI::GetClass() const
	{
		return _T("MenuItemUI");
	}

	LPVOID CMenuItemUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_MENUITEM) == 0) 
			return static_cast<CMenuItemUI*>(this);   

		return CListContainerElementUI::GetInterface(pstrName);
	}

	void CMenuItemUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName,_T("lefticon")) == 0 )
		{
			SetLeftIcon(pstrValue);
		}
		else if( _tcscmp(pstrName, _T("righticon")) == 0 )
		{
			SetRightIcon(pstrValue);
		}
		
		CListContainerElementUI::SetAttribute(pstrName,pstrValue);
	}

	void CMenuItemUI::SetLeftIcon(LPCTSTR strIcon)
	{
		m_strLeftIcon = strIcon;
	}

	LPCTSTR  CMenuItemUI::GetLeftIcon()
	{
		return m_strLeftIcon;
	}

	void CMenuItemUI::SetRightIcon(LPCTSTR strIcon)
	{
		m_strRightIcon = strIcon;
	}

	LPCTSTR  CMenuItemUI::GetRightIcon()
	{
		return m_strRightIcon;
	}

	void CMenuItemUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem)) return;

		CRect rc = m_rcItem;
		rc.Deflate(&m_rcPadding);

		CMenuItemUI::DrawItemBk(hDC, rc);
		DrawItemIcon(hDC, rc, m_strLeftIcon, true);
		DrawItemIcon(hDC, rc, m_strRightIcon, false);
		DrawItemText(hDC, rc);

		for (int i = 0; i < GetCount(); ++i)
		{
			if (GetItemAt(i)->GetInterface(DUI_CTR_MENUITEM) == NULL)
				GetItemAt(i)->DoPaint(hDC, rcPaint);
		}
	}


	void CMenuItemUI::DrawItemIcon(HDC hdc, const RECT& rcItem, CUIString strIcon, bool isLeft)
	{
		if(strIcon.IsEmpty())
			return;

		DWORD dwMask = 0;
		LPCTSTR lpstrImage = strIcon.GetData();
		TImageInfo*	pImageInfo = CRenderEngine::LoadImage(lpstrImage, (LPCTSTR)NULL, dwMask);
		if(pImageInfo == NULL)
			return;
		
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		CRect rcPaint = rcItem;

		if(isLeft)
		{
			rcPaint.right  = rcPaint.left + pInfo->rcTextPadding.left;
		}
		else
		{
			rcPaint.left = rcPaint.right - pInfo->rcTextPadding.right;
		}


		if(rcPaint.GetWidth() < pImageInfo->nX || rcPaint.GetHeight() < pImageInfo->nY)
			return;

		RECT rcIcon;
		rcIcon.left = rcPaint.left + (rcPaint.GetWidth() - pImageInfo->nX) / 2;
		rcIcon.top  = rcPaint.top +  (rcPaint.GetHeight() - pImageInfo->nY) / 2;
		rcIcon.right = rcIcon.left + pImageInfo->nX;
		rcIcon.bottom = rcIcon.top + pImageInfo->nY;


		CRenderEngine::DrawImageString(hdc, m_pManager, rcIcon, rcPaint, lpstrImage);

		CRenderEngine::FreeImage(pImageInfo); //释放位图句柄和内存
		
	}

	void CMenuItemUI::DrawItemText(HDC hDC, const RECT& rcItem)
	{
		if (m_sText.IsEmpty()) return;

		if (m_pOwner == NULL) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		DWORD iTextColor = pInfo->dwTextColor;

		if ((m_uButtonState & UISTATE_HOT) != 0) 
			iTextColor = pInfo->dwHotTextColor;
		
		if (IsSelected()) 
			iTextColor = pInfo->dwSelectedTextColor;
		
		if (!IsEnabled()) 
			iTextColor = pInfo->dwDisabledTextColor;
		
		int nLinks = 0;
		RECT rcText = rcItem;
		rcText.left += pInfo->rcTextPadding.left;
		rcText.right -= pInfo->rcTextPadding.right;
		rcText.top += pInfo->rcTextPadding.top;
		rcText.bottom -= pInfo->rcTextPadding.bottom;

		if (pInfo->bShowHtml)
			CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_sText, iTextColor, \
			NULL, NULL, nLinks, DT_SINGLELINE | pInfo->uTextStyle);
		else
			CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText, iTextColor, \
			pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);
	}

	SIZE CMenuItemUI::EstimateSize(SIZE szAvailable)
	{
		SIZE cXY = {0};
		for (int it = 0; it < GetCount(); it++) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(it));
			if (!pControl->IsVisible()) continue;

			SIZE sz = pControl->EstimateSize(szAvailable);
			cXY.cy += sz.cy;

			if (cXY.cx < sz.cx)
				cXY.cx = sz.cx;
		}

		if (cXY.cy == 0)
		{
			TListInfoUI* pInfo = m_pOwner->GetListInfo();
			DWORD iTextColor = pInfo->dwTextColor;

			if ((m_uButtonState & UISTATE_HOT) != 0)
				iTextColor = pInfo->dwHotTextColor;

			if (IsSelected()) 
				iTextColor = pInfo->dwSelectedTextColor;
			
			if (!IsEnabled()) 
				iTextColor = pInfo->dwDisabledTextColor;

			RECT rcText = { 0, 0, MAX(szAvailable.cx, m_cxyFixed.cx), 9999 };
			rcText.left += pInfo->rcTextPadding.left;
			rcText.right -= pInfo->rcTextPadding.right;

			if (pInfo->bShowHtml) 
			{   
				int nLinks = 0;
				CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, iTextColor, NULL, NULL, nLinks, DT_CALCRECT | pInfo->uTextStyle);
			}
			else
			{
				CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, iTextColor, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle);
			}

			cXY.cx = rcText.right - rcText.left + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right + 20;
			cXY.cy = rcText.bottom - rcText.top + pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
		}

		if (m_cxyFixed.cy != 0) 
			cXY.cy = m_cxyFixed.cy;

		return cXY;
	}

	void CMenuItemUI::DoEvent(TEventUI& event)
	{
		if (event.Type == UIEVENT_MOUSEENTER)
		{
			CListContainerElementUI::DoEvent(event);

			if (m_pWindow) return;
			bool hasSubMenu = false;

			for (int i = 0; i < GetCount(); ++i)
			{
				if (GetItemAt(i)->GetInterface(DUI_CTR_MENUITEM) != NULL)
				{
					(static_cast<CMenuItemUI*>(GetItemAt(i)->GetInterface(DUI_CTR_MENUITEM)))->SetVisible(true);
					(static_cast<CMenuItemUI*>(GetItemAt(i)->GetInterface(DUI_CTR_MENUITEM)))->SetInternVisible(true);

					hasSubMenu = true;
				}
			}

			if (hasSubMenu)
			{
				m_pOwner->SelectItem(GetIndex(), true);
				CreateMenuWnd();
			}
			else
			{
				ContextMenuParam param;
				param.hWnd = m_pManager->GetPaintWindow();
				param.wParam = 2;
				s_context_menu_observer.RBroadcast(param);
				m_pOwner->SelectItem(GetIndex(), true);
			}
			return;
		}

		if (event.Type == UIEVENT_BUTTONDOWN)
		{
			if (IsEnabled())
			{
				CListContainerElementUI::DoEvent(event);

				if (m_pWindow) return;
				bool hasSubMenu = false;

				for (int i = 0; i < GetCount(); ++i)
				{
					if (GetItemAt(i)->GetInterface(DUI_CTR_MENUITEM) != NULL) 
					{
						(static_cast<CMenuItemUI*>(GetItemAt(i)->GetInterface(DUI_CTR_MENUITEM)))->SetVisible(true);
						(static_cast<CMenuItemUI*>(GetItemAt(i)->GetInterface(DUI_CTR_MENUITEM)))->SetInternVisible(true);

						hasSubMenu = true;
					}
				}

				if(hasSubMenu)
				{
					CreateMenuWnd();
				}
				else
				{
					ContextMenuParam param;
					param.hWnd = m_pManager->GetPaintWindow();
					param.wParam = 3;
					s_context_menu_observer.RBroadcast(param);
				}
			}

			return;
		}

		CListContainerElementUI::DoEvent(event);
	}

	bool CMenuItemUI::Activate()
	{
		if (CListContainerElementUI::Activate() && m_bSelected)
		{
			if (m_pWindow) return true;
			bool hasSubMenu = false;

			for (int i = 0; i < GetCount(); ++i)
			{
				if (GetItemAt(i)->GetInterface(DUI_CTR_MENUITEM) != NULL)
				{
					(static_cast<CMenuItemUI*>(GetItemAt(i)->GetInterface(DUI_CTR_MENUITEM)))->SetVisible(true);
					(static_cast<CMenuItemUI*>(GetItemAt(i)->GetInterface(DUI_CTR_MENUITEM)))->SetInternVisible(true);

					hasSubMenu = true;
				}
			}

			if (hasSubMenu)
			{
				CreateMenuWnd();
			}
			else
			{
				ContextMenuParam param;
				param.hWnd = m_pManager->GetPaintWindow();
				param.wParam = 3;
				s_context_menu_observer.RBroadcast(param);
			}

			return true;
		}

		return false;
	}

	CMenuWnd* CMenuItemUI::GetMenuWnd()
	{
		return m_pWindow;
	}

	void CMenuItemUI::CreateMenuWnd()
	{
		if (m_pWindow) return;
		
		m_pWindow = new CMenuWnd(m_pManager->GetPaintWindow());
		ASSERT(m_pWindow);

		ContextMenuParam param;
		param.hWnd = m_pManager->GetPaintWindow();
		param.wParam = 2;
		s_context_menu_observer.RBroadcast(param);

	
		m_pWindow->Init(m_pNotify, static_cast<CMenuItemUI*>(this), _T(""), _T(""), CPoint());
	}

	void CMenuItemUI::SetNotify(INotifyUI * pNotify)
	{
		m_pNotify = pNotify;
	}

}
