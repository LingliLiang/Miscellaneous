#include "StdAfx.h"
#include "UIScrollView.h"

const TCHAR* DUI_CTR_SCROLLVIEW = _T("ScrollView");
namespace DirectUI {

	CScrollViewUI::CScrollViewUI() : m_iCurSel(-1), m_uButtonState(0)
	{
		m_bMouseWheeling = FALSE;
		m_bMouseWheelSelect = true;

		::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));

		m_ListInfo.nColumns = 0;
		m_ListInfo.nFont = -1;
		m_ListInfo.uTextStyle = DT_VCENTER;
		m_ListInfo.dwTextColor = 0xFF000000;
		m_ListInfo.dwBkColor = 0;
		m_ListInfo.bAlternateBk = false;
		m_ListInfo.dwSelectedTextColor = 0xFF000000;
		m_ListInfo.dwSelectedBkColor = 0x00000000;
		m_ListInfo.dwHotTextColor = 0xFF000000;
		m_ListInfo.dwHotBkColor = 0x00000000;
		m_ListInfo.dwDisabledTextColor = 0xFFCCCCCC;
		m_ListInfo.dwDisabledBkColor = 0x00000000;
		m_ListInfo.dwLineColor = 0;
		m_ListInfo.bShowHtml = false;
		m_ListInfo.bMultiExpandable = false;
		::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
		::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
		memset(&m_oldPoint, 0, sizeof(POINT));
		memset(&m_szItem, 0, sizeof(SIZE));
		m_moveOffset = 0;
		m_ptYCorrect = 0;
	}


	LPCTSTR CScrollViewUI::GetClass() const
	{
		return _T("ScrollViewUI");
	}

	LPVOID CScrollViewUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_SCROLLVIEW) == 0 ) return static_cast<CScrollViewUI*>(this);
		if( _tcscmp(pstrName, _T("IListOwner")) == 0 ) return static_cast<IListOwnerUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	UINT CScrollViewUI::GetControlFlags() const
	{
		return 0;
	}

	void CScrollViewUI::DoInit()
	{
	}

	int CScrollViewUI::GetCurSel() const
	{
		return m_iCurSel;
	}

	bool CScrollViewUI::SelectItem(int nIndex, bool bTakeFocus)
	{
		if (nIndex == m_iCurSel) return true;
		int iOldSel = m_iCurSel;
		if (m_iCurSel >= 0) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
			if (!pControl) return false;
			IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if (pListItem != NULL) pListItem->Select(false);
			m_iCurSel = -1;
		}

		if (nIndex < 0) return false;
		if (m_items.GetSize() == 0) return false;
		if (nIndex >= m_items.GetSize()) nIndex = m_items.GetSize() - 1;

		CControlUI* pControl = static_cast<CControlUI*>(m_items[nIndex]);
		if (!pControl/* || !pControl->IsVisible() */|| !pControl->IsEnabled()) return false;
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if (pListItem == NULL) return false;

		m_iCurSel = nIndex;
		if (bTakeFocus) pControl->SetFocus();
		pListItem->Select(true);
		if (m_pManager != NULL) m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMSELECT, m_iCurSel, iOldSel);
		Invalidate();

		return true;
	}

	bool CScrollViewUI::SetItemIndex(CControlUI* pControl, int nIndex)
	{
		int iOrginIndex = GetItemIndex(pControl);
		if( iOrginIndex == -1 ) return false;
		if( iOrginIndex == nIndex ) return true;

		IListItemUI* pSelectedListItem = NULL;
		if( m_iCurSel >= 0 ) pSelectedListItem = 
			static_cast<IListItemUI*>(GetItemAt(m_iCurSel)->GetInterface(_T("ListItem")));

		if( !CContainerUI::SetItemIndex(pControl, nIndex) ) return false;
		int iMinIndex = min(iOrginIndex, nIndex);
		int iMaxIndex = max(iOrginIndex, nIndex);
		for(int i = iMinIndex; i < iMaxIndex + 1; ++i) 
		{
			CControlUI* p = GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) 
			{
				pListItem->SetIndex(i);
			}
		}

		if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->GetIndex();
		return true;
	}

	bool CScrollViewUI::Add(CControlUI* pControl)
	{
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) 
		{
			pListItem->SetOwner(this);
			pListItem->SetIndex(m_items.GetSize());
		}
		return CContainerUI::Add(pControl);
	}

	bool CScrollViewUI::AddAt(CControlUI* pControl, int nIndex)
	{
		if (!CContainerUI::AddAt(pControl, nIndex)) return false;

		// The list items should know about us
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) 
		{
			pListItem->SetOwner(this);
			pListItem->SetIndex(nIndex);
		}

		for(int i = nIndex + 1; i < GetCount(); ++i) 
		{
			CControlUI* p = GetItemAt(i);
			pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if ( pListItem != NULL ) 
				pListItem->SetIndex(i);
		}
		if( m_iCurSel >= nIndex ) m_iCurSel += 1;
		return true;
	}

	bool CScrollViewUI::Remove(CControlUI* pControl)
	{
		int nIndex = GetItemIndex(pControl);
		if (nIndex == -1) return false;

		if (!CContainerUI::RemoveAt(nIndex)) return false;

		for(int i = nIndex; i < GetCount(); ++i) 
		{
			CControlUI* p = GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) 
				pListItem->SetIndex(i);
		}

		if( nIndex == m_iCurSel && m_iCurSel >= 0 )
		{
			int iSel = m_iCurSel;
			m_iCurSel = -1;
			SelectItem(FindSelectable(iSel, false));
		}
		else if( nIndex < m_iCurSel ) m_iCurSel -= 1;
		return true;
	}

	bool CScrollViewUI::RemoveAt(int nIndex)
	{
		if (!CContainerUI::RemoveAt(nIndex)) return false;

		for(int i = nIndex; i < GetCount(); ++i) 
		{
			CControlUI* p = GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) pListItem->SetIndex(i);
		}

		if( nIndex == m_iCurSel && m_iCurSel >= 0 ) 
		{
			int iSel = m_iCurSel;
			m_iCurSel = -1;
			SelectItem(FindSelectable(iSel, false));
		}
		else if( nIndex < m_iCurSel ) m_iCurSel -= 1;
		return true;
	}

	void CScrollViewUI::RemoveAll()
	{
		m_iCurSel = -1;
		CContainerUI::RemoveAll();
	}

	SIZE CScrollViewUI::EstimateSize(SIZE szAvailable)
	{
		return CControlUI::EstimateSize(szAvailable);
	}

	bool CScrollViewUI::Activate()
	{
		if( !CControlUI::Activate() ) return false;
		Invalidate();
		return true;
	}

	CUIString CScrollViewUI::GetText() const
	{
		if (m_iCurSel < 0) return _T("");
		CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
		return pControl->GetText();
	}

	void CScrollViewUI::SetEnabled(bool bEnable)
	{
		CContainerUI::SetEnabled(bEnable);
		if( !IsEnabled() ) m_uButtonState = 0;
	}

	RECT CScrollViewUI::GetTextPadding() const
	{
		return m_rcTextPadding;
	}

	void CScrollViewUI::SetTextPadding(RECT rc)
	{
		m_rcTextPadding = rc;
		Invalidate();
	}

	LPCTSTR CScrollViewUI::GetNormalImage() const
	{
		return m_sNormalImage;
	}

	void CScrollViewUI::SetNormalImage(LPCTSTR pStrImage)
	{
		m_sNormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollViewUI::GetHotImage() const
	{
		return m_sHotImage;
	}

	void CScrollViewUI::SetHotImage(LPCTSTR pStrImage)
	{
		m_sHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollViewUI::GetPushedImage() const
	{
		return m_sPushedImage;
	}

	void CScrollViewUI::SetPushedImage(LPCTSTR pStrImage)
	{
		m_sPushedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollViewUI::GetFocusedImage() const
	{
		return m_sFocusedImage;
	}

	void CScrollViewUI::SetFocusedImage(LPCTSTR pStrImage)
	{
		m_sFocusedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollViewUI::GetDisabledImage() const
	{
		return m_sDisabledImage;
	}

	void CScrollViewUI::SetDisabledImage(LPCTSTR pStrImage)
	{
		m_sDisabledImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollViewUI::GetForeImage() const
	{
		return m_sForeImage;
	}

	void CScrollViewUI::SetForeImage(LPCTSTR pStrImage)
	{
		m_sForeImage = pStrImage;
		Invalidate();
	}

	TListInfoUI* CScrollViewUI::GetListInfo()
	{
		return &m_ListInfo;
	}

	void CScrollViewUI::SetItemFont(int index)
	{
		m_ListInfo.nFont = index;
		Invalidate();
	}

	void CScrollViewUI::SetItemTextStyle(UINT uStyle)
	{
		m_ListInfo.uTextStyle = uStyle;
		Invalidate();
	}

	RECT CScrollViewUI::GetItemTextPadding() const
	{
		return m_ListInfo.rcTextPadding;
	}

	void CScrollViewUI::SetItemTextPadding(RECT rc)
	{
		m_ListInfo.rcTextPadding = rc;
		Invalidate();
	}

	void CScrollViewUI::SetItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwTextColor = dwTextColor;
		Invalidate();
	}

	void CScrollViewUI::SetItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwBkColor = dwBkColor;
	}

	void CScrollViewUI::SetItemBkImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sBkImage = pStrImage;
	}

	DWORD CScrollViewUI::GetItemTextColor() const
	{
		return m_ListInfo.dwTextColor;
	}

	DWORD CScrollViewUI::GetItemBkColor() const
	{
		return m_ListInfo.dwBkColor;
	}

	LPCTSTR CScrollViewUI::GetItemBkImage() const
	{
		return m_ListInfo.sBkImage;
	}

	bool CScrollViewUI::IsAlternateBk() const
	{
		return m_ListInfo.bAlternateBk;
	}

	void CScrollViewUI::SetAlternateBk(bool bAlternateBk)
	{
		m_ListInfo.bAlternateBk = bAlternateBk;
	}

	void CScrollViewUI::SetSelectedItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwSelectedTextColor = dwTextColor;
	}

	void CScrollViewUI::SetSelectedItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwSelectedBkColor = dwBkColor;
	}

	void CScrollViewUI::SetSelectedItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sSelectedImage = pStrImage;
	}

	DWORD CScrollViewUI::GetSelectedItemTextColor() const
	{
		return m_ListInfo.dwSelectedTextColor;
	}

	DWORD CScrollViewUI::GetSelectedItemBkColor() const
	{
		return m_ListInfo.dwSelectedBkColor;
	}

	LPCTSTR CScrollViewUI::GetSelectedItemImage() const
	{
		return m_ListInfo.sSelectedImage;
	}

	void CScrollViewUI::SetHotItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwHotTextColor = dwTextColor;
	}

	void CScrollViewUI::SetHotItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwHotBkColor = dwBkColor;
	}

	void CScrollViewUI::SetHotItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sHotImage = pStrImage;
	}

	DWORD CScrollViewUI::GetHotItemTextColor() const
	{
		return m_ListInfo.dwHotTextColor;
	}

	DWORD CScrollViewUI::GetHotItemBkColor() const
	{
		return m_ListInfo.dwHotBkColor;
	}

	LPCTSTR CScrollViewUI::GetHotItemImage() const
	{
		return m_ListInfo.sHotImage;
	}

	void CScrollViewUI::SetDisabledItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwDisabledTextColor = dwTextColor;
	}

	void CScrollViewUI::SetDisabledItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwDisabledBkColor = dwBkColor;
	}

	void CScrollViewUI::SetDisabledItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sDisabledImage = pStrImage;
	}

	DWORD CScrollViewUI::GetDisabledItemTextColor() const
	{
		return m_ListInfo.dwDisabledTextColor;
	}

	DWORD CScrollViewUI::GetDisabledItemBkColor() const
	{
		return m_ListInfo.dwDisabledBkColor;
	}

	LPCTSTR CScrollViewUI::GetDisabledItemImage() const
	{
		return m_ListInfo.sDisabledImage;
	}

	DWORD CScrollViewUI::GetItemLineColor() const
	{
		return m_ListInfo.dwLineColor;
	}

	void CScrollViewUI::SetItemLineColor(DWORD dwLineColor)
	{
		m_ListInfo.dwLineColor = dwLineColor;
	}

	bool CScrollViewUI::IsItemShowHtml()
	{
		return m_ListInfo.bShowHtml;
	}

	void CScrollViewUI::SetItemShowHtml(bool bShowHtml)
	{
		if( m_ListInfo.bShowHtml == bShowHtml ) return;

		m_ListInfo.bShowHtml = bShowHtml;
		Invalidate();
	}

	SIZE CScrollViewUI::GetItemSize()
	{
		return m_szItem;
	}

	void CScrollViewUI::SetItemSize(SIZE size)
	{
		m_szItem = size;
	}

	void CScrollViewUI::SetPos(RECT rc)
	{
		// Put all elements out of sight
		RECT rcNull = { 0 };
		for( int i = 0; i < m_items.GetSize(); i++ ) static_cast<CControlUI*>(m_items[i])->SetPos(rcNull);
		// Position this control
		CControlUI::SetPos(rc);
	}

	void CScrollViewUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("textpadding")) == 0) 
		{
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetTextPadding(rcTextPadding);
		}
		else if (_tcscmp(pstrName, _T("normalimage")) == 0) SetNormalImage(pstrValue);
		else if (_tcscmp(pstrName, _T("hotimage")) == 0) SetHotImage(pstrValue);
		else if (_tcscmp(pstrName, _T("pushedimage")) == 0) SetPushedImage(pstrValue);
		else if (_tcscmp(pstrName, _T("focusedimage")) == 0) SetFocusedImage(pstrValue);
		else if (_tcscmp(pstrName, _T("disabledimage")) == 0) SetDisabledImage(pstrValue);
		else if (_tcscmp(pstrName, _T("foreimage")) == 0) SetForeImage(pstrValue);
		else if (_tcscmp(pstrName, _T("itemfont")) == 0) m_ListInfo.nFont = _ttoi(pstrValue);
		else if (_tcscmp(pstrName, _T("itemalign")) == 0)
		{
			if (_tcsstr(pstrValue, _T("left")) != NULL)
			{
				m_ListInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
				m_ListInfo.uTextStyle |= DT_LEFT;
			}
			if (_tcsstr(pstrValue, _T("center")) != NULL) 
			{
				m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT );
				m_ListInfo.uTextStyle |= DT_CENTER;
			}
			if (_tcsstr(pstrValue, _T("right")) != NULL)
			{
				m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				m_ListInfo.uTextStyle |= DT_RIGHT;
			}
			if (_tcsstr(pstrValue, _T("top")) != NULL) 
			{
				m_ListInfo.uTextStyle &= ~(DT_BOTTOM | DT_VCENTER | DT_VCENTER);
				m_ListInfo.uTextStyle |= (DT_TOP | DT_SINGLELINE);
			}
			if (_tcsstr(pstrValue, _T("vcenter")) != NULL) 
			{
				m_ListInfo.uTextStyle &= ~(DT_TOP | DT_BOTTOM );			
				m_ListInfo.uTextStyle |= (DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			if (_tcsstr(pstrValue, _T("bottom")) != NULL) 
			{
				m_ListInfo.uTextStyle &= ~(DT_TOP | DT_VCENTER | DT_VCENTER);
				m_ListInfo.uTextStyle |= (DT_BOTTOM | DT_SINGLELINE);
			}
			if (_tcsstr(pstrValue, _T("vleft")) != NULL) 
			{
				m_ListInfo.uTextStyle &= ~(DT_TOP | DT_BOTTOM |DT_CENTER );			
				m_ListInfo.uTextStyle |= (DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			}
			if (_tcsstr(pstrValue, _T("vright")) != NULL) 
			{
				m_ListInfo.uTextStyle &= ~(DT_TOP | DT_BOTTOM |DT_CENTER );			
				m_ListInfo.uTextStyle |= (DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
			}
		}
		else if (_tcscmp(pstrName, _T("itemtextpadding")) == 0)
		{
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetItemTextPadding(rcTextPadding);
		}
		else if (_tcscmp(pstrName, _T("itemtextcolor")) == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetItemTextColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("itembkcolor")) == 0)
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetItemBkColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("itembkimage")) == 0) SetItemBkImage(pstrValue);
		else if (_tcscmp(pstrName, _T("itemaltbk")) == 0) SetAlternateBk(_tcscmp(pstrValue, _T("true")) == 0);
		else if (_tcscmp(pstrName, _T("itemselectedtextcolor")) == 0) 
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedItemTextColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("itemselectedbkcolor")) == 0)
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedItemBkColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("itemselectedimage")) == 0) SetSelectedItemImage(pstrValue);
		else if (_tcscmp(pstrName, _T("itemhottextcolor")) == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetHotItemTextColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("itemhotbkcolor")) == 0)
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetHotItemBkColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("itemhotimage")) == 0) SetHotItemImage(pstrValue);
		else if (_tcscmp(pstrName, _T("itemdisabledtextcolor")) == 0) 
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetDisabledItemTextColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("itemdisabledbkcolor")) == 0) 
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetDisabledItemBkColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("itemdisabledimage")) == 0) SetDisabledItemImage(pstrValue);
		else if (_tcscmp(pstrName, _T("itemlinecolor")) == 0) 
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetItemLineColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("mousewheelselect")) == 0) SetMouseWheelSelect(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("itemendellipsis")) == 0 )
		{
			if( _tcscmp(pstrValue, _T("true")) == 0 ) m_ListInfo.uTextStyle |= DT_END_ELLIPSIS;
			else m_ListInfo.uTextStyle &= ~DT_END_ELLIPSIS;
		}
		else if (_tcscmp(pstrName, _T("itemsize")) == 0) 
		{
			SIZE sz = { 0 };
			LPTSTR pstr = NULL;
			sz.cx= _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			sz.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			SetItemSize(sz);
		}

		else CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	void CScrollViewUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem)) return;

		// ����ѭ�򣺱�����ɫ->����ͼ->״̬ͼ->�ı�->�߿�
		if (m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0)
		{
			CRenderClip roundClip;
			CRenderClip::GenerateRoundClip(hDC, m_rcPaint, m_rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
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
			if (!m_sForeImage.IsEmpty()) 
			{
				if (!DrawImage(hDC, (LPCTSTR)m_sForeImage)) m_sForeImage.Empty();
				else return;
			}
			PaintBorder(hDC);
		}
	}

	void CScrollViewUI::SetMouseWheeling(BOOL b)
	{
		m_bMouseWheeling = b;
	}

	BOOL CScrollViewUI::IsMouseWheeling()
	{
		return m_bMouseWheeling;
	}

	void CScrollViewUI::SetMouseWheelSelect(bool isMouseWheelSelect)
	{
		m_bMouseWheelSelect = isMouseWheelSelect;
	}

	bool CScrollViewUI::GetMouseWheelSelect()
	{
		return m_bMouseWheelSelect;
	}

	void CScrollViewUI::PaintStatusImage(HDC hDC)
	{
		if (IsFocused()) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if (!IsEnabled()) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;

		if ((m_uButtonState & UISTATE_DISABLED) != 0) 
		{
			if (!m_sDisabledImage.IsEmpty())
			{
				if (!DrawImage(hDC, (LPCTSTR)m_sDisabledImage)) m_sDisabledImage.Empty();
				else return;
			}
		}
		else if ((m_uButtonState & UISTATE_PUSHED) != 0)
		{
			if (!m_sPushedImage.IsEmpty()) 
			{
				if (!DrawImage(hDC, (LPCTSTR)m_sPushedImage)) m_sPushedImage.Empty();
				else return;
			}
		}
		else if ((m_uButtonState & UISTATE_HOT) != 0) 
		{
			if (!m_sHotImage.IsEmpty()) 
			{
				if (!DrawImage(hDC, (LPCTSTR)m_sHotImage)) m_sHotImage.Empty();
				else return;
			}
		}
		else if ((m_uButtonState & UISTATE_FOCUSED) != 0) 
		{
			if (!m_sFocusedImage.IsEmpty()) 
			{
				if (!DrawImage(hDC, (LPCTSTR)m_sFocusedImage)) m_sFocusedImage.Empty();
				else return;
			}
		}

		if (!m_sNormalImage.IsEmpty()) 
		{
			if (!DrawImage(hDC, (LPCTSTR)m_sNormalImage)) m_sNormalImage.Empty();
		}
	}

	void CScrollViewUI::PaintText(HDC hDC)
	{
		CRenderClip Clip;
		CRenderClip::GenerateClip(hDC, GetClipPaintRc(), Clip);
		if (m_iCurSel >= 0)
		{
			auto Item = [&](int index)->CControlUI*
			{
				return static_cast<CControlUI*>(m_items[index]);
			};
			auto PaintCtl = [&](CControlUI* pControl,int line)->void
			{
				RECT rcOldPos = pControl->GetPos();
				CRect rcPos(m_rcItem);
				rcPos.top = line;
				rcPos.bottom = rcPos.top + pControl->GetFixedHeight();
				pControl->SetManager(NULL,pControl->GetParent(),false); // ���manager��ֹSetPos�ػ�
				pControl->SetPos(rcPos);
				pControl->SetManager(this->GetManager(),pControl->GetParent(),false); // ����manager
				pControl->DoPaint(hDC, rcPos);
				pControl->SetManager(NULL,pControl->GetParent(),false); // ���manager��ֹSetPos�ػ�
				pControl->SetPos(rcOldPos);
				pControl->SetManager(this->GetManager(),pControl->GetParent(),false); // ����manager
			};
			//paint select above
			int lineY = GetMoveLineY();
			int indexOther = 0;
			int lineOther = 0;
			indexOther = GetPreIndex(m_iCurSel);
			lineOther = lineY;
			while (true)
			{
				CControlUI* pControl = Item(indexOther);
				if (lineOther <= m_rcItem.top)
				{
					break;
				}
				lineOther -= pControl->GetFixedHeight();
				PaintCtl(pControl, lineOther);
				indexOther = GetPreIndex(indexOther);

			}
			//paint select item
			{
				CControlUI* pControl = Item(m_iCurSel);
				PaintCtl(pControl, lineY);
				lineY += pControl->GetFixedHeight();
			}
			//paint select above
			indexOther = GetNextIndex(m_iCurSel);
			lineOther = lineY;
			while (true)
			{
				CControlUI* pControl = Item(indexOther);
				if (lineOther >= m_rcItem.bottom)
				{
					break;
				}
				PaintCtl(pControl, lineOther);
				lineOther += pControl->GetFixedHeight();
				indexOther = GetNextIndex(indexOther);
			}
		}
	}

	void CScrollViewUI::DoEvent(TEventUI& event)
	{
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
			if (m_pParent != NULL) m_pParent->DoEvent(event);
			else CContainerUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_SETFOCUS)
		{
			Invalidate();
		}
		if (event.Type == UIEVENT_KILLFOCUS)
		{
			Invalidate();
		}
		if (event.Type == UIEVENT_BUTTONDOWN)
		{
			if (IsEnabled())
			{
				Activate();
				m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
				m_oldPoint = event.ptMouse;
				m_moveOffset = 0;
				m_ptYCorrect = 0;
			}
			return;
		}
		if (event.Type == UIEVENT_BUTTONUP)
		{
			if ((m_uButtonState & UISTATE_CAPTURED) != 0)
			{
				m_moveOffset = 0;
				m_ptYCorrect = 0;
				memset(&m_oldPoint, 0, sizeof(POINT));
				m_uButtonState &= ~UISTATE_CAPTURED;
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_MOUSEMOVE)
		{
			if (m_iCurSel < 0) return;
			if (m_oldPoint.x != 0 && 0 != m_oldPoint.y)
			{
				IListItemUI* pListItem = NULL;
				auto Item = [&](int index)->CControlUI*
				{
					return static_cast<CControlUI*>(m_items[index]);
				};
				auto ItemList = [&](int index)->IListItemUI*
				{
					return static_cast<IListItemUI*>(static_cast<CControlUI*>(m_items[index])->GetInterface(_T("ListItem")));;
				};
				int offsety = event.ptMouse.y - m_oldPoint.y; //��Ϊ����,��Ϊ����
				m_moveOffset = offsety + m_ptYCorrect;
				//DUI__Trace(L"offsety  -- %d --- %d --- %d",offsety,m_moveOffset , m_ptYCorrect);
				if (offsety > 0)
				{
					int pre = GetPreIndex(m_iCurSel);
					long pretop = GetMoveLineY() - Item(pre)->GetFixedHeight();
					long line = GetSelectLineY(pre);
					if(line <= pretop) //�л�ѡ����
					{
						//DUI__Trace(L"UP Hiting -- %d === %d",line,pretop);
						pListItem = ItemList(m_iCurSel);
						if(pListItem){
							pListItem->Select(false,false);
						}
						pListItem = ItemList(pre);
						if(pListItem){
							pListItem->Select();
							m_ptYCorrect -=Item(pre)->GetFixedHeight();
							m_moveOffset = 0;
							if (m_pManager != NULL) m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMSELECT, pre, m_iCurSel);
							m_iCurSel = pre;
						}
					}
				}
				else
				{
					int next = GetNextIndex(m_iCurSel);
					long nexttop = GetMoveLineY() + Item(m_iCurSel)->GetFixedHeight();
					long line = GetSelectLineY(next);
					if(line >= nexttop) //�л�ѡ����
					{
						//DUI__Trace(L"Down Hiting -- %d === %d",line,nexttop);
						pListItem = ItemList(m_iCurSel);
						if(pListItem){
							pListItem->Select(false,false);
						}
						pListItem = ItemList(next);
						if(pListItem){
							pListItem->Select();
							m_ptYCorrect +=Item(next)->GetFixedHeight();
							m_moveOffset = 0;
							if (m_pManager != NULL) m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMSELECT, next, m_iCurSel);
							m_iCurSel = next;
						}
					}
				}
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_KEYDOWN)
		{
			switch (event.chKey) {
			case VK_F4:
				Activate();
				return;
			case VK_UP:
				SelectItem(FindSelectable(m_iCurSel - 1, false));
				return;
			case VK_DOWN:
				SelectItem(FindSelectable(m_iCurSel + 1, true));
				return;
			case VK_PRIOR:
				SelectItem(FindSelectable(m_iCurSel - 1, false));
				return;
			case VK_NEXT:
				SelectItem(FindSelectable(m_iCurSel + 1, true));
				return;
			case VK_HOME:
				SelectItem(FindSelectable(0, false));
				return;
			case VK_END:
				SelectItem(FindSelectable(GetCount() - 1, true));
				return;
			}
		}
		if (event.Type == UIEVENT_SCROLLWHEEL)
		{
			if (!IsEnabled())
				return;
			if (!m_bMouseWheelSelect)
				return;
			bool bDownward = LOWORD(event.wParam) == SB_LINEDOWN;
			SetMouseWheeling(TRUE);
			SelectItem(FindSelectable(m_iCurSel + (bDownward ? 1 : -1), bDownward));
			SetMouseWheeling(FALSE);
			return;
		}
		if (event.Type == UIEVENT_CONTEXTMENU)
		{
			return;
		}
		if (event.Type == UIEVENT_MOUSEENTER)
		{
			if (::PtInRect(&m_rcItem, event.ptMouse))
			{
				if ((m_uButtonState & UISTATE_HOT) == 0)
					m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_MOUSELEAVE)
		{
			if ((m_uButtonState & UISTATE_HOT) != 0) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}

		CControlUI::DoEvent(event);
	}

	RECT CScrollViewUI::GetClipPaintRc()
	{
		RECT rcText = m_rcItem;
		rcText.left += m_rcTextPadding.left;
		rcText.right -= m_rcTextPadding.right;
		rcText.top += m_rcTextPadding.top;
		rcText.bottom -= m_rcTextPadding.bottom;
		return rcText;
	}

	long CScrollViewUI::GetSelectLineY(int index)
	{
		CControlUI* p = GetItemAt(index);
		if (p)
		{
			long y = m_rcItem.bottom - m_rcItem.top;
			long height = p->GetFixedHeight();
			if (y > 0)
			{
				if (y > height)
				{
					return m_rcItem.top + ((y - height) >> 1);
				}
				else
				{
					return m_rcItem.top - ((height - y) >> 1);
				}
			}

		}
		return -1;
	}

	long CScrollViewUI::GetMoveLineY()
	{
		if (m_moveOffset != 0)
		{
			return GetSelectLineY(m_iCurSel) + m_moveOffset;
		}
		return GetSelectLineY(m_iCurSel);
	}

	int CScrollViewUI::GetPreIndex(int index)
	{
		if (index < 0) return -1;
		int size = m_items.GetSize();
		if (size == 0) return -1;
		if (index == 0) return size - 1;
		return index - 1;
	}

	int CScrollViewUI::GetNextIndex(int index)
	{
		if (index < 0) return -1;
		int size = m_items.GetSize();
		if (size == 0) return -1;
		if (index == size - 1) return 0;
		return index + 1;
	}


} // namespace DirectUI
