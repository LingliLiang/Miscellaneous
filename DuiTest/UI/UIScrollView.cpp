#include "StdAfx.h"
#include "UIScrollView.h"
namespace DirectUI {

CScrollViewUI::CScrollViewUI() : , m_iCurSel(-1), m_uButtonState(0)
{
    m_szDropBox = CSize(0, 150);
    ::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));

    m_ListInfo.nColumns = 0;
    m_ListInfo.nFont = -1;
    m_ListInfo.uTextStyle = DT_VCENTER;
    m_ListInfo.dwTextColor = 0xFF000000;
    m_ListInfo.dwBkColor = 0;
    m_ListInfo.bAlternateBk = false;
    m_ListInfo.dwSelectedTextColor = 0xFF000000;
    m_ListInfo.dwSelectedBkColor = 0xFFC1E3FF;
    m_ListInfo.dwHotTextColor = 0xFF000000;
    m_ListInfo.dwHotBkColor = 0xFFE9F5FF;
    m_ListInfo.dwDisabledTextColor = 0xFFCCCCCC;
    m_ListInfo.dwDisabledBkColor = 0xFFFFFFFF;
    m_ListInfo.dwLineColor = 0;
    m_ListInfo.bShowHtml = false;
    m_ListInfo.bMultiExpandable = false;
    ::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
    ::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));

	m_bMouseWheeling=FALSE;
	m_bMouseWheelSelect = true;
}

LPCTSTR CScrollViewUI::GetClass() const
{
    return _T("ScrollViewUI");
}

LPVOID CScrollViewUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, DUI_CTR_COMBO) == 0 ) return static_cast<CScrollViewUI*>(this);
    if( _tcscmp(pstrName, _T("IListOwner")) == 0 ) return static_cast<IListOwnerUI*>(this);
    return CContainerUI::GetInterface(pstrName);
}

UINT CScrollViewUI::GetControlFlags() const
{
    return UIFLAG_TABSTOP;
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
    if (!IsMouseWheeling()) ;
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

bool CScrollViewUI::SelectRange(int nIndex, bool bTakeFocus /* = false */)
{
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

void CScrollViewUI::MoveSelectedItemToHotItem()
{
	return;
}

void CScrollViewUI::SetHotItemIndex(int nIndex)
{
	return;
}

void CScrollViewUI::SetMouseState(bool bDown)
{
	return;
}

void CScrollViewUI::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else CContainerUI::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_SETFOCUS ) 
    {
        Invalidate();
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
        Invalidate();
    }
    if( event.Type == UIEVENT_BUTTONDOWN )
    {
        if( IsEnabled() ) 
		{
            Activate();
            m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
        }
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP )
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) 
		{
            m_uButtonState &= ~ UISTATE_CAPTURED;
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE )
    {
        return;
    }
    if( event.Type == UIEVENT_KEYDOWN )
    {
        switch( event.chKey ) {
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
    if( event.Type == UIEVENT_SCROLLWHEEL )
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
    if( event.Type == UIEVENT_CONTEXTMENU )
    {
        return;
    }
    if( event.Type == UIEVENT_MOUSEENTER )
    {
        if (::PtInRect(&m_rcItem, event.ptMouse ))
		{
            if( (m_uButtonState & UISTATE_HOT) == 0  )
                m_uButtonState |= UISTATE_HOT;
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSELEAVE )
    {
        if( (m_uButtonState & UISTATE_HOT) != 0 ) {
            m_uButtonState &= ~UISTATE_HOT;
            Invalidate();
        }
        return;
    }

    CControlUI::DoEvent(event);
}

SIZE CScrollViewUI::EstimateSize(SIZE szAvailable)
{
    if( m_cxyFixed.cy == 0 ) return CSize(m_cxyFixed.cx, m_pManager->GetDefaultFontInfo()->tm.tmHeight + 12);
    return CControlUI::EstimateSize(szAvailable);
}

bool CScrollViewUI::Activate()
{
    if( !CControlUI::Activate() ) return false;
    if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_DROPDOWN);
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

CUIString CScrollViewUI::GetDropBoxAttributeList()
{
    return m_sDropBoxAttributes;
}

void CScrollViewUI::SetDropBoxAttributeList(LPCTSTR pstrList)
{
    m_sDropBoxAttributes = pstrList;
}

SIZE CScrollViewUI::GetDropBoxSize() const
{
    return m_szDropBox;
}

void CScrollViewUI::SetDropBoxSize(SIZE szDropBox)
{
    m_szDropBox = szDropBox;
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
    else if (_tcscmp(pstrName, _T("dropbox")) == 0) SetDropBoxAttributeList(pstrValue);
	else if (_tcscmp(pstrName, _T("dropboxsize")) == 0)
	{
		SIZE szDropBoxSize = { 0 };
		LPTSTR pstr = NULL;
		szDropBoxSize.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		szDropBoxSize.cy = _tcstol(pstr + 1, &pstr, 10);   ASSERT(pstr);    
		SetDropBoxSize(szDropBoxSize);
	}
    else if (_tcscmp(pstrName, _T("itemfont")) == 0) m_ListInfo.nFont = _ttoi(pstrValue);
    else if (_tcscmp(pstrName, _T("itemalign")) == 0)
	{
        if (_tcsstr(pstrValue, _T("left")) != NULL) 
		{
            m_ListInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
            m_ListInfo.uTextStyle |= DT_LEFT;
        }
        if( _tcsstr(pstrValue, _T("center")) != NULL ) 
		{
            m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
            m_ListInfo.uTextStyle |= DT_CENTER;
        }
        if( _tcsstr(pstrValue, _T("right")) != NULL )
		{
            m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);
            m_ListInfo.uTextStyle |= DT_RIGHT;
        }
    }
    if (_tcscmp(pstrName, _T("itemtextpadding")) == 0)
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
    else CContainerUI::SetAttribute(pstrName, pstrValue);
}

void CScrollViewUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    CControlUI::DoPaint(hDC, rcPaint);
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
        else return;
    }
}

void CScrollViewUI::PaintText(HDC hDC)
{
    RECT rcText = m_rcItem;
    rcText.left += m_rcTextPadding.left;
    rcText.right -= m_rcTextPadding.right;
    rcText.top += m_rcTextPadding.top;
    rcText.bottom -= m_rcTextPadding.bottom;
	
    if (m_iCurSel >= 0) 
	{
        CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
        IListItemUI* pElement = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
        if (pElement != NULL) 
		{
            pElement->DrawItemText(hDC, rcText);
        }
        else 
		{
            RECT rcOldPos = pControl->GetPos();
            pControl->SetPos(rcText);
            pControl->DoPaint(hDC, rcText);
            pControl->SetPos(rcOldPos);
        }
    }
}

void CScrollViewUI::SetMouseWheeling(BOOL b)
{
	m_bMouseWheeling=b;
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

} // namespace DirectUI
