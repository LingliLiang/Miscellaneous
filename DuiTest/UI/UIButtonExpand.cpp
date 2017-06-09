#include "stdafx.h"
#include "UIButtonExpand.h"

//#define  DUI_CTR_BUTTONEXPAND                    (_T("ButtonExpand"))
//else if (_tcscmp(pstrClass, DUI_CTR_BUTTONEXPAND) == 0)	  pControl = new CButtonExpandUI; uidlgbuilder.cpp
//#include "Control/UIButtonExpand.h"

//<ButtonExpand  height = "60" width = "60" maxwidth = "200" font = "1" textcolor = "#FFAAAAAA"
//	hottextcolor = "#FF808080" pushedtextcolor = "#FF606060"
//	hotimage = "1.png" normalimage = "3.png" pushedimage = "2.png"
//	exnormalimage = "3.png" exhotimage = "2.png" expushedimage = "1.png"
//	focusedimage = "" disabledimage = "" foreimage = "" hotforeimage = ""
//	flashhotimage = "1.png" flashpushedimage = "3.png"
//	exfocusedimage = "" exdisabledimage = ""
//	expushedforeimage = "" exhotforeimage = "" exforeimage = ""
//	expendrgn = "40,1,1,2,2" bkcolor = "#ff848484"
//	text = "test" / >

namespace DirectUI
{
	CButtonExpandUI::CButtonExpandUI()
		: m_uExpandButtonState(0)
		, m_nFlashTimerID_2(0)
		, m_hExpendRgn(NULL)
		, m_nExpandAnchor(0)
		, m_nNowNormalID_2(BTN_FLASH_NORMAL)
		, m_vecExpendRgn(new std::vector<POINT>)
	{
		
	}

	CButtonExpandUI::~CButtonExpandUI()
	{
		delete m_vecExpendRgn;
		m_vecExpendRgn = NULL;
	}

	LPCTSTR CButtonExpandUI::GetClass() const
	{
		return _T("ButtonExpandUI");
	}

	LPVOID CButtonExpandUI::GetInterface(LPCTSTR pstrName)
	{
		if(_tcscmp(pstrName, DUI_CTR_BUTTONEXPAND) == 0)
			return static_cast<CButtonExpandUI*>(this);

		return CButtonUI::GetInterface(pstrName);
	}

	UINT CButtonExpandUI::GetButtonState() const
	{
		return m_uButtonState;
	}

	void CButtonExpandUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
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
		if (event.Type == UIEVENT_KEYDOWN)
		{
			if (IsKeyboardEnabled()) {
				if (event.chKey == VK_SPACE || event.chKey == VK_RETURN) {
					Activate();
					return;
				}
			}
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
		{
			if( PtInItemRect(event.ptMouse) && IsEnabled() ) {
				m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
				m_pManager->SendNotify(this, DUI_MSGTYPE_BUTTONDOWN, event.wParam, event.lParam);
				Invalidate();
			}
			if (PtInExpendRect(event.ptMouse) && IsEnabled() )
			{
				m_uExpandButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
				if (PtInItemRect(event.ptMouse)) m_uButtonState |= UISTATE_PUSHED;
				else m_uButtonState &= ~UISTATE_PUSHED;
			}
			if((m_uExpandButtonState & UISTATE_CAPTURED) != 0){
				if (PtInExpendRect(event.ptMouse)) m_uExpandButtonState |= UISTATE_PUSHED;
				else m_uExpandButtonState &= ~UISTATE_PUSHED;
			}

			if (!PtInItemRect(event.ptMouse))
				m_uButtonState &= ~UISTATE_HOT;
			else
				m_uButtonState |= UISTATE_HOT;
			if (!PtInExpendRect(event.ptMouse))
				m_uExpandButtonState &= ~UISTATE_HOT;
			else
				m_uExpandButtonState |= UISTATE_HOT;
			Invalidate();
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				if(PtInItemRect(event.ptMouse)) Activate();
				m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
				Invalidate();
			}
			if ((m_uExpandButtonState & UISTATE_CAPTURED) != 0) {
				if (PtInExpendRect(event.ptMouse)) /*Activate()*/Expand();
				m_uExpandButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				if (PtInItemRect(event.ptMouse))
					m_uButtonState |= UISTATE_HOT;
				if (PtInExpendRect(event.ptMouse))
					m_uExpandButtonState |= UISTATE_HOT;
				Invalidate();
			}
			// return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				if (!PtInItemRect(event.ptMouse))
					m_uButtonState &= ~UISTATE_HOT;
				if (!PtInExpendRect(event.ptMouse))
					m_uExpandButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			// return;
		}
		if (event.Type==UIEVENT_TIMER) {
			if (m_nFlashTimerID != 0 && event.wParam == m_nFlashTimerID) {
				OnFlashTimer();
				return;
			}
			if (m_nFlashTimerID_2 != 0 && event.wParam == FLASHTIMERID_2) {
				OnFlashTimer2();
				return;
			}
			else if (event.wParam == EXPENDBUTTON) {
				OnExpand();
				return;
			}
		}
		if (event.Type == UIEVENT_CONTEXTMENU)
		{
			if (IsContextMenuUsed()) {
				m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
			}
			return;
		}
		if (event.Type == UIEVENT_SETCURSOR) {
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IsHandCursor() ? IDC_HAND : IDC_ARROW)));
			return;
		}

		CLabelUI::DoEvent(event);
	}


	void CButtonExpandUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
			m_uExpandButtonState = 0;
		}
	}

	LPCTSTR CButtonExpandUI::GetExpandNormalImage()
	{
		return m_sExpandNormalImage;
	}

	void CButtonExpandUI::SetExpandNormalImage(LPCTSTR pStrImage)
	{
		m_sExpandNormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonExpandUI::GetExpandHotImage()
	{
		return m_sExpandHotImage;
	}

	void CButtonExpandUI::SetExpandHotImage(LPCTSTR pStrImage)
	{
		m_sExpandHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonExpandUI::GetExpandPushedImage()
	{
		return m_sExpandPushedImage;
	}

	void CButtonExpandUI::SetExpandPushedImage(LPCTSTR pStrImage)
	{
		m_sExpandPushedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonExpandUI::GetExpandFocusedImage()
	{
		return m_sExpandFocusedImage;
	}

	void CButtonExpandUI::SetExpandFocusedImage(LPCTSTR pStrImage)
	{
		m_sExpandFocusedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonExpandUI::GetExpandDisabledImage()
	{
		return m_sExpandDisabledImage;
	}

	void CButtonExpandUI::SetExpandDisabledImage(LPCTSTR pStrImage)
	{
		m_sExpandDisabledImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonExpandUI::GetExpandForeImage()
	{
		return m_sExpandForeImage;
	}

	void CButtonExpandUI::SetExpandForeImage( LPCTSTR pStrImage )
	{
		m_sExpandForeImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonExpandUI::GetExpandHotForeImage()
	{
		return m_sExpandHotForeImage;
	}

	void CButtonExpandUI::SetExpandHotForeImage( LPCTSTR pStrImage )
	{
		m_sExpandHotForeImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonExpandUI::GetExpandPushedForeImage()
	{
		return m_sExpandPushedForeImage;
	}

	void CButtonExpandUI::SetExpandPushedForeImage(LPCTSTR pStrImage)
	{
		m_sExpandPushedForeImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonExpandUI::GetFlashHotImage()
	{
		return m_sFlashHotImage;
	}

	void CButtonExpandUI::SetFlashHotImage(LPCTSTR pStrImage)
	{
		m_sFlashHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CButtonExpandUI::GetFlashPushedImage()
	{
		return m_sFlashPushedImage;
	}

	void CButtonExpandUI::SetFlashPushedImage(LPCTSTR pStrImage)
	{
		m_sFlashPushedImage = pStrImage;
		Invalidate();
	}

	void CButtonExpandUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("width")) == 0) m_oldFixed.cx = _ttoi(pstrValue);
		else if (_tcscmp(pstrName, _T("height")) == 0) m_oldFixed.cy = _ttoi(pstrValue);
		if( _tcscmp(pstrName, _T("exnormalimage")) == 0 ) SetExpandNormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("exhotimage")) == 0 ) SetExpandHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("expushedimage")) == 0 ) SetExpandPushedImage(pstrValue);
		else if (_tcscmp(pstrName, _T("flashhotimage")) == 0) SetFlashHotImage(pstrValue);
		else if (_tcscmp(pstrName, _T("flashpushedimage")) == 0) SetFlashPushedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("exfocusedimage")) == 0 ) SetExpandFocusedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("exdisabledimage")) == 0 ) SetExpandDisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("exforeimage")) == 0 ) SetExpandForeImage(pstrValue);
		else if( _tcscmp(pstrName, _T("exhotforeimage")) == 0 ) SetExpandHotForeImage(pstrValue);
		else if (_tcscmp(pstrName, _T("expushedforeimage")) == 0) SetExpandPushedForeImage(pstrValue);
		else if (_tcscmp(pstrName, _T("expendrgn")) == 0)
		{
			LPTSTR pstr = NULL;
			UINT nNum = 0;
			m_vecExpendRgn->clear();
			m_nExpandAnchor = _tcstol(pstrValue, &pstr, 10);
			if (_tcscmp(pstr, _T("")) == 0 || _tcscmp(pstr, _T(" ")) == 0)
			{
				return;
			}
			do {
				POINT pt = { 0 };
				pt.x = _tcstol(pstr + 1, &pstr, 10);
				ASSERT(pstr);
				if (_tcscmp(pstr, _T("")) == 0)
					break;
				pt.y = _tcstol(pstr + 1, &pstr, 10);
				ASSERT(pstr);
				m_vecExpendRgn->push_back(pt);
				if (_tcscmp(pstr, _T("")) == 0)
					break;
			} while (true);
			return;
		}
		else CButtonUI::SetAttribute(pstrName, pstrValue);
	}

	void CButtonExpandUI::PaintText(HDC hDC)
	{
		if (IsFocused()) { 
			m_uButtonState |= UISTATE_FOCUSED; 
			m_uExpandButtonState |= UISTATE_FOCUSED;
		}
		else {
			m_uButtonState &= ~UISTATE_FOCUSED;
			m_uExpandButtonState &= ~UISTATE_FOCUSED;
		}
		if (!IsEnabled()) {
			m_uButtonState |= UISTATE_DISABLED;
			m_uExpandButtonState |= UISTATE_DISABLED;
		}
		else { 
			m_uButtonState &= ~UISTATE_DISABLED;
			m_uExpandButtonState &= ~UISTATE_DISABLED;
		}

		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		if( m_sText.IsEmpty() ) return;
		int nLinks = 0;
		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		DWORD clrColor = IsEnabled()?m_dwTextColor:m_dwDisabledTextColor;

		if( ((m_uButtonState & UISTATE_PUSHED) != 0) && (GetPushedTextColor() != 0) )
			clrColor = GetPushedTextColor();
		else if( ((m_uButtonState & UISTATE_HOT) != 0) && (GetHotTextColor() != 0) )
			clrColor = GetHotTextColor();
		else if( ((m_uButtonState & UISTATE_FOCUSED) != 0) && (GetFocusedTextColor() != 0) )
			clrColor = GetFocusedTextColor();

		if( m_bShowHtml )
			CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, clrColor, \
			NULL, NULL, nLinks, m_uTextStyle);
		else
			CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, clrColor, \
			m_iFont, m_uTextStyle);
	}

	void CButtonExpandUI::PaintStatusImage(HDC hDC)
	{
		if (IsFocused()) {
			m_uButtonState |= UISTATE_FOCUSED;
			m_uExpandButtonState |= UISTATE_FOCUSED;
		}
		else {
			m_uButtonState &= ~UISTATE_FOCUSED;
			m_uExpandButtonState &= ~UISTATE_FOCUSED;
		}
		if (!IsEnabled()) {
			m_uButtonState |= UISTATE_DISABLED;
			m_uExpandButtonState |= UISTATE_DISABLED;
		}
		else {
			m_uButtonState &= ~UISTATE_DISABLED;
			m_uExpandButtonState &= ~UISTATE_DISABLED;
		}

		RECT rcItem = m_rcItem;

		rcItem = GetExpandItemRect();
		if ((m_uExpandButtonState & UISTATE_DISABLED) != 0) {
			if (!m_sExpandDisabledImage.IsEmpty())
			{
				if (!CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, (LPCTSTR)m_sExpandDisabledImage, NULL)) m_sExpandDisabledImage.Empty();
				else goto Label_Expand_ForeImage;
			}
		}
		else if ((m_uExpandButtonState & UISTATE_PUSHED) != 0) {
			if (!m_sExpandPushedImage.IsEmpty()) {
				if (!CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, (LPCTSTR)m_sExpandPushedImage, NULL)) {
					m_sExpandPushedImage.Empty();
				}
				if (!m_sExpandPushedForeImage.IsEmpty())
				{
					if (!CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, (LPCTSTR)m_sExpandPushedForeImage, NULL))
						m_sExpandPushedForeImage.Empty();
					return;
				}
				else goto Label_Expand_ForeImage;
			}
		}
		else if ((m_uExpandButtonState & UISTATE_HOT) != 0) {
			if (!m_sExpandHotImage.IsEmpty()) {
				if (!CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, (LPCTSTR)m_sExpandHotImage, NULL)) {
					m_sExpandHotImage.Empty();
				}
				if (!m_sExpandHotForeImage.IsEmpty()) {
					if (!CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, (LPCTSTR)m_sExpandHotForeImage, NULL))
						m_sExpandHotForeImage.Empty();
					return;
				}
				else goto Label_Expand_ForeImage;
			}
			else if (m_dwHotBkColor != 0) {
				CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwHotBkColor));
				return;
			}
		}
		else if ((m_uExpandButtonState & UISTATE_FOCUSED) != 0) {
			if (!m_sExpandFocusedImage.IsEmpty()) {
				if (!CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, (LPCTSTR)m_sExpandFocusedImage, NULL)) m_sExpandFocusedImage.Empty();
				else goto Label_Expand_ForeImage;
			}
		}

		if (!m_sExpandNormalImage.IsEmpty()) {
			if (!CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, (LPCTSTR)m_sExpandNormalImage, NULL)) m_sExpandNormalImage.Empty();
			else goto Label_Expand_ForeImage;
		}

		if (!m_sExpandForeImage.IsEmpty())
		{

		Label_Expand_ForeImage:
			if (!m_sExpandForeImage.IsEmpty()) {
				if (!CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, (LPCTSTR)m_sExpandForeImage, NULL)) m_sExpandForeImage.Empty();
			}
		}

		rcItem = GetItemRect();
		if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
			if( !m_sDisabledImage.IsEmpty() )
			{
				if( !CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, (LPCTSTR)m_sDisabledImage, NULL)) m_sDisabledImage.Empty();
				else goto Label_ForeImage;
			}
		}
		else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
			if( !m_sPushedImage.IsEmpty() ) {
				if( !CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, (LPCTSTR)m_sPushedImage, NULL)){
					m_sPushedImage.Empty();
				}
				if( !m_sPushedForeImage.IsEmpty() )
				{
					if( !CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, (LPCTSTR)m_sPushedForeImage, NULL))
						m_sPushedForeImage.Empty();
					return;
				}
				else goto Label_ForeImage;
			}
		}
		else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			if( !m_sHotImage.IsEmpty() ) {
				if( !CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, (LPCTSTR)m_sHotImage, NULL)){
					m_sHotImage.Empty();
				}
				if( !m_sHotForeImage.IsEmpty() ) {
					if( !CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, (LPCTSTR)m_sHotForeImage, NULL))
						m_sHotForeImage.Empty();
					return;
				}
				else goto Label_ForeImage;
			}
			else if(m_dwHotBkColor != 0) {
				CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwHotBkColor));
				return;
			}
		}
		else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
			if( !m_sFocusedImage.IsEmpty() ) {
				if( !CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, (LPCTSTR)m_sFocusedImage, NULL)) m_sFocusedImage.Empty();
				else goto Label_ForeImage;
			}
		}

		if( !m_sNormalImage.IsEmpty() ) {
			if( !CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, (LPCTSTR)m_sNormalImage, NULL)) m_sNormalImage.Empty();
			else goto Label_ForeImage;
		}

		if (!m_sForeImage.IsEmpty())
		{

		Label_ForeImage:
			if (!m_sForeImage.IsEmpty()) {
				if (!CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, (LPCTSTR)m_sForeImage, NULL)) m_sForeImage.Empty();
			}
		}
	}

	void CButtonExpandUI::ChangeNormalImageToOther2(BTN_FLASH_IMGS nStatus)
	{
		CUIString strNormal;
		if (nStatus!= BTN_FLASH_NORMAL) {
			if (m_sFirstNormalImage.IsEmpty()) {
				m_sFirstNormalImage=m_sNormalImage;
			}
			switch (nStatus)
			{
			case BTN_FLASH_HOT:
				strNormal=GetHotImage();
				break;
			case BTN_FLASH_PUSHED:
				strNormal=GetPushedImage();
				break;
			}
		} else {
			if (!m_sFirstNormalImage.IsEmpty())
				strNormal=m_sFirstNormalImage;
			else
				strNormal=m_sNormalImage;
		}
		if (strNormal.IsEmpty())
			return;
		SetNormalImage(strNormal);
		m_nNowNormalID_2 =nStatus;
	}

	void CButtonExpandUI::FlashBtn(UINT nCount, int nTime)
	{
		if (m_pManager) {
			m_nFlashTimerID_2 = nCount;
			m_pManager->SetTimer(this, FLASHTIMERID_2,nTime);
		}
	}

	void CButtonExpandUI::OnFlashTimer2()
	{
		int nNow= m_nNowNormalID_2 +1;
		if (nNow>BTN_FLASH_PUSHED)
			nNow= BTN_FLASH_NORMAL;
		if (m_nFlashTimerID_2 > 1)
		{
			m_nFlashTimerID_2--;
		}
		else
		{
			m_nFlashTimerID_2 = 0;
			m_nNowNormalID_2 = BTN_FLASH_NORMAL;
			m_pManager->KillTimer(this, FLASHTIMERID_2);
			ChangeNormalImageToOther(BTN_IMG_NORMAL);
		}
		
		ChangeNormalImageToOther2((BTN_FLASH_IMGS)nNow);
	}

	RECT CButtonExpandUI::GetExpandItemRect()
	{
		CRect rcTemp = m_rcItem;
		UINT rcWidth = rcTemp.GetWidth() - m_oldFixed.cx;
		rcTemp.left = rcTemp.left + rcWidth + m_nExpandAnchor;
		return rcTemp;
	}

	RECT CButtonExpandUI::GetItemRect()
	{
		CRect rcTemp = m_rcItem;
		rcTemp.right = rcTemp.left + m_nExpandAnchor;
		return rcTemp;
	}

	bool CButtonExpandUI::PtInExpendRect(POINT pt)
	{
		//if (m_hExpendRgn == NULL) return false;
		//::PtInRegion(m_hExpendRgn, pt.x, pt.y);
		return ::PtInRect(&GetExpandItemRect(), pt);
	}

	bool CButtonExpandUI::PtInItemRect(POINT pt)
	{
		return ::PtInRect(&GetItemRect(), pt);
	}

	void CButtonExpandUI::Expand()
	{
		if (m_pManager) {
			m_uExpandButtonState = m_uButtonState;
			m_pManager->SetTimer(this, EXPENDBUTTON, 10);
		}	
	}

	void CButtonExpandUI::OnExpand()
	{
		unsigned int width = m_rcItem.right - m_rcItem.left;
		if (width < m_cxyMax.cx)
			width += 24;
		if (width > m_cxyMax.cx)
		{
			width = m_cxyMax.cx;
			m_cxyFixed.cx = width;
			if (m_pManager) {
				m_pManager->KillTimer(this, EXPENDBUTTON);
				m_pManager->NeedUpdate();
			}
		}
		m_cxyFixed.cx = width;
		m_rcItem.right = m_rcItem.left + width;
		SetPos(m_rcItem);
		this->NeedParentUpdate();
	}
}