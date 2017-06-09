#include "stdafx.h"
#include "DockPanel.h"
#include <GdiPlus.h>
#pragma comment (lib,"GdiPlus.lib")

CUIString GetCommonArrt(CControlUI* pControl)
{
	CUIString strArrt;
	CUIString strValue;
	if(pControl)
	{
		strValue.Format(_T("name=\"%s\" "),pControl->GetName().GetData());
		strArrt.Append(strValue);
		CRect rcTemp = pControl->GetPadding();
		strValue.Format(_T("padding=\"%d,%d,%d,%d\" "),rcTemp.left ,rcTemp.top,rcTemp.right,rcTemp.bottom);
		strArrt.Append(strValue);
		strValue.Format(_T("bkcolor=\"%ld\" "),pControl->GetBkColor());
		strArrt.Append(strValue);
		strValue.Format(_T("bkcolor2=\"%ld\" "),pControl->GetBkColor2());
		strArrt.Append(strValue);
		strValue.Format(_T("bkcolor3=\"%ld\" "),pControl->GetBkColor3());
		strArrt.Append(strValue);
		strValue.Format(_T("nativebkcolor=\"%ld\" "),pControl->GetNativeBkColor());
		strArrt.Append(strValue);
		strValue.Format(_T("bordercolor=\"%ld\" "),pControl->GetBorderColor());
		strArrt.Append(strValue);
		strValue.Format(_T("focusbordercolor=\"%ld\" "),pControl->GetFocusBorderColor());
		strArrt.Append(strValue);
		strValue.Format(_T("bordersize=\"%d\" "),pControl->GetBorderSize());
		strArrt.Append(strValue);
		strValue.Format(_T("borderround=\"%d,%d\" "),pControl->GetBorderRound().cx,pControl->GetBorderRound().cy);
		strArrt.Append(strValue);
		strValue.Format(_T("borderstyle=\"%d\" "),pControl->GetBorderStyle());
		strArrt.Append(strValue);
		strValue.Format(_T("bkimage=\"%s\" "),pControl->GetBkImage());
		strArrt.Append(strValue);
		strValue.Format(_T("text=\"%s\" "),pControl->GetText().GetData());
		strArrt.Append(strValue);
		strValue.Format(_T("tooltip=\"%s\" "),pControl->GetToolTip().GetData());
		strArrt.Append(strValue);
		strValue.Format(_T("userdata=\"%s\" "),pControl->GetUserData().GetData());
		strArrt.Append(strValue);
		strValue.Format(_T("enabled=\"%s\" "),pControl->IsEnabled()?_T("true"):_T("false"));
		strArrt.Append(strValue);
		strValue.Format(_T("mouse=\"%s\" "),pControl->IsMouseEnabled()?_T("true"):_T("false"));
		strArrt.Append(strValue);
		strValue.Format(_T("visible=\"%s\" "),pControl->IsVisible()?_T("true"):_T("false"));
		strArrt.Append(strValue);
		strValue.Format(_T("float=\"%s\" "),pControl->IsFloat()?_T("true"):_T("false"));
		strArrt.Append(strValue);
		if(pControl->IsFloat())
		{
			rcTemp = pControl->GetPos();
			strValue.Format(_T("pos=\"%d,%d,%d,%d\" "),rcTemp.left ,rcTemp.top,rcTemp.right,rcTemp.bottom);
			strArrt.Append(strValue);
		}
		TCHAR scut = pControl->GetShortcut();
		if(scut != _T('\0'))
		{
			strValue.Format(_T("shortcut=\"%c\" "),scut);
		}
		strArrt.Append(strValue);
		strValue.Format(_T("menu=\"%s\" "),pControl->IsContextMenuUsed()?_T("true"):_T("false"));
		strArrt.Append(strValue);
		strValue.Format(_T("keyboard=\"%s\" "),pControl->IsKeyboardEnabled()?_T("true"):_T("false"));
		strArrt.Append(strValue);
		strValue.Format(_T("virtualwnd=\"%s\" "),pControl->GetVirtualWnd().GetData());
		strArrt.Append(strValue);
	}

	return strArrt;
}

CUIString GetLayoutArrt(CContainerUI* pLayout)
{
	CUIString strArrt;
	CUIString strValue;
	CRect rcTemp;
	if(pLayout)
	{
		rcTemp = pLayout->GetInset();
		strValue.Format(_T("mousechild=\"%s\" "),pLayout->IsMouseChildEnabled()?_T("true"):_T("false"));
		strArrt.Append(strValue);
		strValue.Format(_T("inset=\"%d,%d,%d,%d\" "),rcTemp.left ,rcTemp.top,rcTemp.right,rcTemp.bottom);
		strArrt.Append(strValue);
		strValue.Format(_T("childpadding=\"%d\" "),pLayout->GetChildPadding());
		strArrt.Append(strValue);


		if(pLayout->GetClass() == _T("HorizontalLayoutUI"))
		{
			strValue.Format(_T("sepwidth=\"%s\" "),static_cast<CHorizontalLayoutUI*>(pLayout)->GetSepWidth());
			strArrt.Append(strValue);
			strValue.Format(_T("sepimm=\"%s\" "),static_cast<CHorizontalLayoutUI*>(pLayout)->IsSepImmMode()?_T("true"):_T("false"));
			strArrt.Append(strValue);
		}
		else if(pLayout->GetClass() == _T("VerticalLayoutUI"))
		{
			strValue.Format(_T("sepheight=\"%s\" "),static_cast<CVerticalLayoutUI*>(pLayout)->GetSepHeight());
			strArrt.Append(strValue);
			strValue.Format(_T("sepimm=\"%s\" "),static_cast<CVerticalLayoutUI*>(pLayout)->IsSepImmMode()?_T("true"):_T("false"));
			strArrt.Append(strValue);
		}
	}
	return strArrt;
}

/***************************CDockManager***************************************/

bool CDockManager::m_bDocking = false;
std::vector<CDockPanelUI*> CDockManager::m_vPanels;
HWND CDockManager::m_hDockWin = (HWND)INVALID_HANDLE_VALUE;
CDockPanelUI*  CDockManager::m_pDockingPanel = NULL;
CDockerUI* CDockManager::m_pDocker = NULL;
CDockManager::CDockManager()
{
}

CDockManager::~CDockManager()
{
}

void CDockManager::AddDockPanelToWatch(CDockPanelUI* panel)
{
	m_vPanels.push_back(panel);
}

void CDockManager::RemoveDockPanel(CDockPanelUI* panel)
{
	std::vector<CDockPanelUI*>::iterator  it = m_vPanels.begin();
	while(it != m_vPanels.end())
	{
		if(*it == panel)
		{
			m_vPanels.erase(it);
			break;
		}
		it++;
	}
}

void CDockManager::Docking(HWND hWin,CDockerUI* pDocker)
{
	m_bDocking = true;
	m_hDockWin = hWin;
	m_pDockingPanel = NULL;
	m_pDocker = pDocker;
	HANDLE hThread = ::CreateThread(
		NULL, // default security attributes
		0, // use default stack size 
		DockingThread, // thread function 
		&m_bDocking, // argument to thread function 
		0, // use default creation flags 
		NULL); // returns the thread identifier 
	::CloseHandle(hThread);
}

bool CDockManager::IsDocking()
{
	return m_bDocking;
}

void CDockManager::StopDocking()
{
	m_bDocking = false;
	m_hDockWin = NULL;
}

CDockPanelUI* CDockManager::GetDockingPanel()
{
	return m_pDockingPanel;
}

void  CDockManager::ClearDockingPanel()
{
	m_pDockingPanel = NULL;
}

DWORD CDockManager::DockingThread(LPVOID lpbDocking)
{
	bool* bDocking = (bool*)lpbDocking;
	while (*bDocking)
	{
		CPoint ptMouse;
		::GetCursorPos(&ptMouse);
		CRect dockArea;
		for (size_t i = 0; i < m_vPanels.size(); i++)
		{
			dockArea = m_vPanels[i]->GetDockingArea();
			if(::PtInRect(&dockArea,ptMouse))
			{
				//when group is same value,then cant dock to panel
				if(m_vPanels[i]->GetUniGroup().Compare(m_pDocker->GetDockGroup()) == 0)
				{
					m_pDockingPanel =  m_vPanels[i];
					m_pDockingPanel->AcceptDocking(true);
				}
			}
			else
			{
				m_pDockingPanel = NULL;
				m_vPanels[i]->AcceptDocking(false);
			}
		}
		::Sleep(40);
	}
	//::OutputDebugStringA("EndDockingThread\n");
	m_pDocker = NULL;
	return 0;
}

void  CDockManager::SendInMessage()
{
	if(::IsWindow(m_hDockWin))
		::SendMessage(m_hDockWin,WM_INACCEPT,0,0);
}

void  CDockManager::SendLeaveMessage()
{
	if(::IsWindow(m_hDockWin))
		::SendMessage(m_hDockWin,WM_LEAVEACCEPT,0,0);
}

/***************************CDockTitleTextUI***************************************/
CDockTitleTextUI::CDockTitleTextUI()
	:m_dwSelectedTextColor(0)
	, m_dwHotTextColor(0)
	, m_dwPushedTextColor(0)
	, m_dwFocusedTextColor(0)
{
	m_bVerticalWestText = true;
	m_nEstimaLength = 0;
	m_uTextStyle |= DT_SINGLELINE | DT_VCENTER | DT_CENTER;
	m_nTextState = 0;
}

SIZE CDockTitleTextUI::EstimateSize(SIZE szAvailable)
{
	//full size
	return CControlUI::EstimateSize(szAvailable);
}

void CDockTitleTextUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcscmp(pstrName, _T("verwesttext")) == 0)
		SetVerticalWestText(_tcscmp(pstrValue, _T("true")) == 0);
	else if (_tcscmp(pstrName, _T("align")) == 0)
	{
		return;
	}
	else if (_tcscmp(pstrName, _T("selectedtextcolor")) == 0) {
		if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetSelectedTextColor(clrColor);
	}
	else if (_tcscmp(pstrName, _T("hottextcolor")) == 0)
	{
		if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetHotTextColor(clrColor);
	}
	else if (_tcscmp(pstrName, _T("pushedtextcolor")) == 0)
	{
		if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetPushedTextColor(clrColor);
	}
	else if (_tcscmp(pstrName, _T("focusedtextcolor")) == 0)
	{
		if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetFocusedTextColor(clrColor);
	}
	else
		CLabelUI::SetAttribute(pstrName, pstrValue);
}

/**
** if it's Vertical Text ,then turn to west or east
**/
void CDockTitleTextUI::SetVerticalWestText(bool west)
{
	m_bVerticalWestText = west;
}


void CDockTitleTextUI::SetSelectedTextColor(DWORD dwTextColor)
{
	m_dwSelectedTextColor = dwTextColor;
}

DWORD CDockTitleTextUI::GetSelectedTextColor()
{
	if (m_dwSelectedTextColor == 0) m_dwSelectedTextColor = m_pManager->GetDefaultFontColor();
	return m_dwSelectedTextColor;
}

void CDockTitleTextUI::SetHotTextColor(DWORD dwColor)
{
	m_dwHotTextColor = dwColor;
}

DWORD CDockTitleTextUI::GetHotTextColor() const
{
	return m_dwHotTextColor;
}

void CDockTitleTextUI::SetPushedTextColor(DWORD dwColor)
{
	m_dwPushedTextColor = dwColor;
}

DWORD CDockTitleTextUI::GetPushedTextColor() const
{
	return m_dwPushedTextColor;
}

void CDockTitleTextUI::SetFocusedTextColor(DWORD dwColor)
{
	m_dwFocusedTextColor = dwColor;
}

DWORD CDockTitleTextUI::GetFocusedTextColor() const
{
	return m_dwFocusedTextColor;
}

void CDockTitleTextUI::SetTextState(UINT state)
{
	m_nTextState = state;
}

void CDockTitleTextUI::ParseTextState(DWORD &dwTextColor)
{
	switch(m_nTextState)
	{
	case UISTATE_FOCUSED:
		if(m_dwFocusedTextColor)
			dwTextColor = m_dwFocusedTextColor;
		break;
	case UISTATE_SELECTED:
		if(m_dwSelectedTextColor)
			dwTextColor = m_dwSelectedTextColor;
		break;
	case UISTATE_HOT:
		if(m_dwHotTextColor)
			dwTextColor = m_dwHotTextColor;
		break;
	case UISTATE_PUSHED:
		if(m_dwPushedTextColor)
			dwTextColor = m_dwPushedTextColor;
		break;
	case UISTATE_DISABLED:
		dwTextColor = m_dwDisabledTextColor;
		break;
	default: //0 Normal
		break;
	}
}

void CDockTitleTextUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem)) return;
	// 绘制循序：背景颜色->背景图->状态图->文本->边框
	//PaintBkColor(hDC);
	PaintText(hDC);
}

void CDockTitleTextUI::PaintText(HDC hDC)
{
	if (m_dwTextColor == 0)
		m_dwTextColor = m_pManager->GetDefaultFontColor();
	if (m_dwDisabledTextColor == 0)
		m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();
	DWORD oldTextColor = m_dwTextColor;
	ParseTextState(m_dwTextColor);
	RECT rc = m_rcItem;
	rc.left += m_rcTextPadding.left;
	rc.right -= m_rcTextPadding.right;
	rc.top += m_rcTextPadding.top;
	rc.bottom -= m_rcTextPadding.bottom;

	if (m_sText.IsEmpty()) return;
	int nLinks = 0;
	if (m_bShowHtml)
		CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, m_dwTextColor, \
		NULL, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
	else
		DrawItemText(hDC, m_pManager, rc, m_sText, m_dwTextColor, m_iFont, m_uTextStyle);
	m_dwTextColor = oldTextColor;
}

void CDockTitleTextUI::DrawItemText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, int iFont, UINT uStyle)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	if (pstrText == NULL || pManager == NULL) return;
	CRect rcItem = this->GetPos();
	HFONT hFont = NULL;
	HFONT hOldFont = NULL;
	UINT oldAlign = GetTextAlign(hDC);
	::SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
	bool end_ellipsis = (uStyle & DT_END_ELLIPSIS) == 0 ? false : true;


	if (rcItem.GetWidth() > rcItem.GetHeight()) //draw Horizontal Text
	{
		hOldFont = (HFONT)::SelectObject(hDC, pManager->GetFont(iFont));
		::DrawText(hDC, pstrText, -1, &rc, uStyle | DT_NOPREFIX);
	}
	else //draw Vertical Text
	{
		CUIString strText = pstrText;
		SetTextAlign(hDC, TA_CENTER);
		LOGFONT LogFont = { 0 };
		::GetObject(pManager->GetFont(iFont), sizeof(LOGFONT), &LogFont);


		CSize textSize;
		GetTextExtentPoint32(hDC, strText.GetData(), strText.GetLength(), &textSize);
		if (m_bVerticalWestText)
		{
			LogFont.lfEscapement = LogFont.lfOrientation = 900;
		}
		else
		{
			;
			LogFont.lfEscapement = LogFont.lfOrientation = 2700;
			textSize.cy = -textSize.cy;
		}
		hFont = CreateFontIndirect(&LogFont);
		hOldFont = (HFONT)::SelectObject(hDC, hFont);
		if (end_ellipsis)
		{

			SIZE szText = textSize;
			if (rcItem.GetHeight() < textSize.cx)
			{
				strText.Append(_T("…"));
				do {
					strText = strText.Left(strText.GetLength() - 3);
					strText.Append(_T("…"));
					GetTextExtentPoint32(hDC, strText.GetData(), strText.GetLength(), &szText);
				} while (rcItem.GetHeight() < szText.cx);
			}
		}
		ExtTextOut(hDC, rcItem.left + (rcItem.GetWidth() - textSize.cy) / 2, rcItem.top + rcItem.GetHeight() / 2,
			0, &rc, strText.GetData(), strText.GetLength(), NULL);

	}

	::SelectObject(hDC, hOldFont);
	SetTextAlign(hDC, oldAlign);
	if (hFont)
		::DeleteObject(hFont);
}

UINT CDockTitleTextUI::GetEstimaLength()
{
	if (m_pManager == NULL) return 0;
	HFONT hOldFont = NULL;
	HDC hDC = m_pManager->GetPaintDC();//::GetDC(NULL);
	hOldFont = (HFONT)::SelectObject(hDC, m_pManager->GetFont(m_iFont));
	CSize textSize;
	GetTextExtentPoint32(hDC,m_sText.GetData(), m_sText.GetLength(), &textSize);
	m_nEstimaLength = textSize.cx + 4 + m_rcTextPadding.right + m_rcTextPadding.left + m_rcPadding.right + m_rcPadding.left;
	::SelectObject(hDC, hOldFont);
	//::ReleaseDC(NULL,hDC);
	return m_nEstimaLength;
}

/***************************CDockTitleItemUI***************************************/


CDockTitleItemUI::CDockTitleItemUI()
	:m_bSelected(false)
	, m_uButtonState(0)
	, m_dwHotBkColor(0)
	, m_nNowNormalID(BTN_IMG_NORMAL)
{
	m_bMoveable = true;
	m_bFloatItem = true;
	m_pText = NULL;
	m_pIcon = NULL;
	m_pClose = NULL;
	m_pDockItemOwer = NULL;
	m_pIcon = new CLabelUI;
	m_pIcon->SetBkColor(0xff121212);
	m_pIcon->SetFixedWidth(16);
	m_pIcon->SetFixedHeight(16);
	m_pIcon->SetVisible(false);
	m_pClose = new CButtonUI;
	m_pClose->SetFixedWidth(30);
	m_pClose->SetFixedHeight(30);
	m_pClose->SetVisible(false);
	this->Add(m_pIcon);
	this->Add(m_pClose);
}

CDockTitleItemUI::~CDockTitleItemUI()
{
	if( !m_sGroupName.IsEmpty() && m_pManager ) 
		m_pManager->RemoveOptionGroup(m_sGroupName, this);
}
//Attributes
LPCTSTR CDockTitleItemUI::GetGroup() const
{
	return m_sGroupName;
}

void CDockTitleItemUI::SetGroup(LPCTSTR pStrGroupName)
{
	CUIString sGroupName = m_sGroupName;
	if (pStrGroupName == NULL) {
		if (m_sGroupName.IsEmpty()) return;
		m_sGroupName.Empty();
	}
	else {
		if (m_sGroupName == pStrGroupName) return;
		if (!m_sGroupName.IsEmpty() && m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
		m_sGroupName = pStrGroupName;
	}

	if (!m_sGroupName.IsEmpty()) {
		if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
	}
	else {
		if (m_pManager) m_pManager->RemoveOptionGroup(sGroupName, this);
	}

	Selected(m_bSelected);
}

bool CDockTitleItemUI::IsSelected() const
{
	return m_bSelected;
}

void CDockTitleItemUI::Selected(bool bSelected)
{
	if (!m_pDockItemOwer || m_bSelected == bSelected) return;
	m_bSelected = bSelected;
	if (m_bSelected)
	{
		m_uButtonState |= UISTATE_SELECTED;
	}
	else
	{
		m_uButtonState &= ~UISTATE_SELECTED;
	}

	if (m_pManager != NULL) {
		if (!m_sGroupName.IsEmpty()) {
			if (m_bSelected) {
				CStdPtrArray* aOptionGroup = m_pManager->GetOptionGroup(m_sGroupName);
				for (int i = 0; i < aOptionGroup->GetSize(); i++) {
					CDockTitleItemUI* pControl = static_cast<CDockTitleItemUI*>(aOptionGroup->GetAt(i));
					if (pControl != this) {
						pControl->Selected(false);
					}
				}
				if(m_pDockItemOwer)
					m_pDockItemOwer->SelectTitle(this);
				m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
			}
		}
		else {
			m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
		}
	}

	Invalidate();
}

void CDockTitleItemUI::SetSelectedBkColor(DWORD dwBkColor)
{
	m_dwSelectedBkColor = dwBkColor;
}

DWORD CDockTitleItemUI::GetSelectBkColor()
{
	return m_dwSelectedBkColor;
}

void CDockTitleItemUI::SetHotBkColor(DWORD dwColor)
{
	m_dwHotBkColor = dwColor;
}

DWORD CDockTitleItemUI::GetHotBkColor() const
{
	return m_dwHotBkColor;
}

//images 

LPCTSTR CDockTitleItemUI::GetSelectedImage()
{
	return m_sSelectedImage;
}

void CDockTitleItemUI::SetSelectedImage(LPCTSTR pStrImage)
{
	m_sSelectedImage = pStrImage;
	Invalidate();
}

LPCTSTR CDockTitleItemUI::GetSelectedHotImage()
{
	return m_sSelectedHotImage;
}

void CDockTitleItemUI::SetSelectedHotImage(LPCTSTR pStrImage)
{
	m_sSelectedHotImage = pStrImage;
	Invalidate();
}

LPCTSTR CDockTitleItemUI::GetSelectedPushedImage()
{
	return m_sSelectedPushedImage;
}

void CDockTitleItemUI::SetSelectedPushedImage(LPCTSTR pStrImage)
{
	m_sSelectedPushedImage = pStrImage;
	Invalidate();
}

LPCTSTR CDockTitleItemUI::GetForeImage()
{
	return m_sForeImage;
}

void CDockTitleItemUI::SetForeImage(LPCTSTR pStrImage)
{
	m_sForeImage = pStrImage;
	Invalidate();
}

LPCTSTR CDockTitleItemUI::GetHotForeImage()
{
	return m_sHotForeImage;
}

void CDockTitleItemUI::SetHotForeImage(LPCTSTR pStrImage)
{
	m_sHotForeImage = pStrImage;
	Invalidate();
}

void CDockTitleItemUI::SetForePushedImage(LPCTSTR pStrImage)
{
	m_sPushedForeImage = pStrImage;
	Invalidate();
}

LPCTSTR CDockTitleItemUI::GetForePushedImage()
{
	return m_sPushedForeImage;
}

LPCTSTR CDockTitleItemUI::GetNormalImage()
{
	return m_sNormalImage;
}

void CDockTitleItemUI::SetNormalImage(LPCTSTR pStrImage)
{
	m_sNormalImage = pStrImage;
	Invalidate();
}

LPCTSTR CDockTitleItemUI::GetHotImage()
{
	return m_sHotImage;
}

void CDockTitleItemUI::SetHotImage(LPCTSTR pStrImage)
{
	m_sHotImage = pStrImage;
	Invalidate();
}

LPCTSTR CDockTitleItemUI::GetPushedImage()
{
	return m_sPushedImage;
}

void CDockTitleItemUI::SetPushedImage(LPCTSTR pStrImage)
{
	m_sPushedImage = pStrImage;
	Invalidate();
}

LPCTSTR CDockTitleItemUI::GetFocusedImage()
{
	return m_sFocusedImage;
}

void CDockTitleItemUI::SetFocusedImage(LPCTSTR pStrImage)
{
	m_sFocusedImage = pStrImage;
	Invalidate();
}

LPCTSTR CDockTitleItemUI::GetDisabledImage()
{
	return m_sDisabledImage;
}

void CDockTitleItemUI::SetDisabledImage(LPCTSTR pStrImage)
{
	m_sDisabledImage = pStrImage;
	Invalidate();
}

bool CDockTitleItemUI::GetMoveable()
{
	return m_bMoveable;
}

void CDockTitleItemUI::SetMoveable(bool moveable)
{
	m_bMoveable = moveable;
}

bool CDockTitleItemUI::GetFloatItem()
{
	return m_bFloatItem;
}

void CDockTitleItemUI::SetFloatItem(bool floatItem)
{
	m_bFloatItem = floatItem;
}

//event
void CDockTitleItemUI::DoEvent(TEventUI& event)
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
	if (event.Type == UIEVENT_KEYDOWN)
	{
		if (IsKeyboardEnabled()) {
			if (event.chKey == VK_SPACE || event.chKey == VK_RETURN) {
				Activate();
				return;
			}
		}
	}
	if (event.Type == UIEVENT_BUTTONDOWN)
	{
		if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled()) {
			m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
			Activate();
			Invalidate();
		}
		return;
	}
	if (event.Type == UIEVENT_DBLCLICK)
	{
		if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled()) {
			if(m_bFloatItem && m_pDockItemOwer)
				m_pDockItemOwer->FloatWindow();
			Invalidate();
		}
		return;
	}
	if (event.Type == UIEVENT_MOUSEMOVE)
	{
		if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
			if (::PtInRect(&m_rcItem, event.ptMouse)) m_uButtonState |= UISTATE_PUSHED;
			else m_uButtonState &= ~UISTATE_PUSHED;
			Invalidate();
		}
		return;
	}
	if (event.Type == UIEVENT_BUTTONUP)
	{
		if (m_pDockItemOwer && m_pDockItemOwer->GetMove())
			m_pDockItemOwer->SetMove(false);
		if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
			if (::PtInRect(&m_rcItem, event.ptMouse)) NULL;
			m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
			Invalidate();
		}
		return;
	}
	if (event.Type == UIEVENT_CONTEXTMENU)
	{
		if (IsContextMenuUsed()) {
			m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
		}
		return;
	}
	if (event.Type == UIEVENT_MOUSEENTER)
	{
		if (m_pDockItemOwer && m_pDockItemOwer->GetMove())
		{

		}
		else
		{
			if (IsEnabled()) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
		}
		// return;
	}
	if (event.Type == UIEVENT_MOUSELEAVE)
	{
		if (IsEnabled()) {
			m_uButtonState &= ~UISTATE_HOT;
			Invalidate();
			if (m_pDockItemOwer && (m_uButtonState & UISTATE_PUSHED) != 0) {
				CRect rcLayout = m_pDockItemOwer->GetItemsLayout()->GetPos();
				rcLayout.Deflate(&(m_pDockItemOwer->GetItemsLayout()->GetInset()));
				if (::PtInRect(&rcLayout, event.ptMouse))
				{
					if(m_bMoveable){
						int nChildPadding = m_pDockItemOwer->GetItemsLayout()->GetChildPadding();
						if (m_pDockItemOwer->IsHorizontal())
						{
							if (event.ptMouse.x <= m_rcItem.left - nChildPadding)
							{
								m_pDockItemOwer->SetMove(true);
								m_pDockItemOwer->MoveTitleItem(true);
							}
							if (event.ptMouse.x >= m_rcItem.right + nChildPadding)
							{
								m_pDockItemOwer->SetMove(true);
								m_pDockItemOwer->MoveTitleItem(false);
							}
						}
						else
						{
							if (event.ptMouse.y <= m_rcItem.top)
							{
								m_pDockItemOwer->SetMove(true);
								m_pDockItemOwer->MoveTitleItem(true);

							}
							if (event.ptMouse.y >= m_rcItem.bottom)
							{
								m_pDockItemOwer->SetMove(true);
								m_pDockItemOwer->MoveTitleItem(false);

							}
						}
					}
				}
				else
				{
					if (m_bFloatItem)
						m_pDockItemOwer->FloatWindow(CDockTitleUI::DRAG_FLOAT);
				}
			}
		}
		// return;
	}
	if (event.Type == UIEVENT_SETCURSOR) {
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
		return;
	}

	CContainerUI::DoEvent(event);
}

//paints
void CDockTitleItemUI::PaintStatusImage(HDC hDC)
{
	//m_uButtonState &= ~UISTATE_PUSHED;
	if ((m_uButtonState & UISTATE_PUSHED) != 0 && IsSelected() && !m_sSelectedPushedImage.IsEmpty()) {
		if(m_pText) m_pText->SetTextState(UISTATE_PUSHED);
		if (!DrawImage(hDC, (LPCTSTR)m_sSelectedPushedImage))
			m_sSelectedPushedImage.Empty();
		else goto Label_ForeImage;

	}
	else if ((m_uButtonState & UISTATE_HOT) != 0 && IsSelected() && !m_sSelectedHotImage.IsEmpty()) {
		if(m_pText) m_pText->SetTextState(UISTATE_HOT);
		if (!DrawImage(hDC, (LPCTSTR)m_sSelectedHotImage))
			m_sSelectedHotImage.Empty();
		else goto Label_ForeImage;

	}
	else if ((m_uButtonState & UISTATE_SELECTED) != 0) {
		if(m_pText) m_pText->SetTextState(UISTATE_SELECTED);
		if (!m_sSelectedImage.IsEmpty()) {
			if (!DrawImage(hDC, (LPCTSTR)m_sSelectedImage)) m_sSelectedImage.Empty();
			else goto Label_ForeImage;
		}
		else if (m_dwSelectedBkColor != 0) {
			CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelectedBkColor));
			goto Label_ForeImage;
		}
	}

	if (IsFocused()) m_uButtonState |= UISTATE_FOCUSED;
	else m_uButtonState &= ~UISTATE_FOCUSED;
	if (!IsEnabled()) m_uButtonState |= UISTATE_DISABLED;
	else m_uButtonState &= ~UISTATE_DISABLED;

	if ((m_uButtonState & UISTATE_DISABLED) != 0) {
		if(m_pText) m_pText->SetTextState(UISTATE_DISABLED);
		if (!m_sDisabledImage.IsEmpty())
		{
			if (!DrawImage(hDC, (LPCTSTR)m_sDisabledImage)) m_sDisabledImage.Empty();
			else goto Label_ForeImage;
		}
	}
	else if ((m_uButtonState & UISTATE_PUSHED) != 0) {
		if(m_pText) m_pText->SetTextState(UISTATE_PUSHED);
		if (!m_sPushedImage.IsEmpty()) {
			if (!DrawImage(hDC, (LPCTSTR)m_sPushedImage)) {
				m_sPushedImage.Empty();
			}
			if (!m_sPushedForeImage.IsEmpty())
			{
				if (!DrawImage(hDC, (LPCTSTR)m_sPushedForeImage))
					m_sPushedForeImage.Empty();
				goto Label_ForeImage;
			}
			else goto Label_ForeImage;
		}
	}
	else if ((m_uButtonState & UISTATE_HOT) != 0) {
		if(m_pText) m_pText->SetTextState(UISTATE_HOT);
		if (!m_sHotImage.IsEmpty()) {
			if (!DrawImage(hDC, (LPCTSTR)m_sHotImage)) {
				m_sHotImage.Empty();
			}
			if (!m_sHotForeImage.IsEmpty()) {
				if (!DrawImage(hDC, (LPCTSTR)m_sHotForeImage))
					m_sHotForeImage.Empty();
				goto Label_ForeImage;
			}
			else goto Label_ForeImage;
		}
		else if (m_dwHotBkColor != 0) {
			CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwHotBkColor));
			goto Label_ForeImage;
		}
	}
	else if ((m_uButtonState & UISTATE_FOCUSED) != 0) {
		if(m_pText) m_pText->SetTextState(UISTATE_FOCUSED);
		if (!m_sFocusedImage.IsEmpty()) {
			if (!DrawImage(hDC, (LPCTSTR)m_sFocusedImage)) m_sFocusedImage.Empty();
			else goto Label_ForeImage;
		}
	}
	else
	{
		if(m_pText) m_pText->SetTextState(0);
	}

	if (!m_sNormalImage.IsEmpty()) {
		if (!DrawImage(hDC, (LPCTSTR)m_sNormalImage)) m_sNormalImage.Empty();
		else goto Label_ForeImage;
	}

	if (!m_sForeImage.IsEmpty())
		goto Label_ForeImage;

	return;

Label_ForeImage:
	if ((m_uButtonState & UISTATE_HOT) != 0)
	{
		if ((m_uButtonState & UISTATE_PUSHED) != 0)
		{
			if (!m_sPushedForeImage.IsEmpty()) {
				if (!DrawImage(hDC, (LPCTSTR)m_sPushedForeImage))
					m_sPushedForeImage.Empty();
				return;
			}
		}
		if (!m_sHotForeImage.IsEmpty()) {
			if (!DrawImage(hDC, (LPCTSTR)m_sHotForeImage))
			{
				m_sHotForeImage.Empty();
			}
			return;
		}
	}

	if (!m_sForeImage.IsEmpty()) {
		if (!DrawImage(hDC, (LPCTSTR)m_sForeImage)) m_sForeImage.Empty();
	}
}

bool CDockTitleItemUI::DrawImage(HDC hDC, LPCTSTR pStrImage, LPCTSTR pStrModify)
{
	return CRenderEngine::DrawImageString(hDC, m_pManager, m_rcItem, m_rcPaint, pStrImage, pStrModify);
}

void CDockTitleItemUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
{
	CContainerUI::SetManager(pManager, pParent, bInit);
	if (bInit && !m_sGroupName.IsEmpty()) {
		if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
	}
}

bool CDockTitleItemUI::Activate()
{
	if (!CContainerUI::Activate()) return false;
	if (!m_sGroupName.IsEmpty()) Selected(true);
	else Selected(!m_bSelected);

	return true;
}

void CDockTitleItemUI::SetEnabled(bool bEnable)
{
	CContainerUI::SetEnabled(bEnable);
	if (!IsEnabled()) {
		if (m_bSelected) m_uButtonState = UISTATE_SELECTED;
		else m_uButtonState = 0;
	}
}

void CDockTitleItemUI::ChangeNormalImageToOther(BTN_IMGS nStatus)
{
	CUIString strNormal;
	if (nStatus != BTN_IMG_NORMAL) {
		if (m_sFirstNormalImage.IsEmpty()) {
			m_sFirstNormalImage = m_sNormalImage;
		}
		switch (nStatus)
		{
		case BTN_IMG_HOT:
			strNormal = GetHotImage();
			m_pText->SetTextState(UISTATE_HOT);
			break;
		case BTN_IMG_PUSHED:
			strNormal = GetPushedImage();
			m_pText->SetTextState(UISTATE_PUSHED);
			break;
		case BTN_IMG_DISABLE:
			strNormal = GetDisabledImage();
			m_pText->SetTextState(UISTATE_DISABLED);
			break;
		case BTN_IMG_FOCUS:
			strNormal = GetFocusedImage();
			m_pText->SetTextState(UISTATE_FOCUSED);
			break;
		case BTN_IMG_FORCE:
			strNormal = GetForeImage();
			break;
		case BTN_IMG_HOTFORCE:
			strNormal = GetHotForeImage();
			break;
		}
	}
	else {
		if (!m_sFirstNormalImage.IsEmpty())
			strNormal = m_sFirstNormalImage;
		else
			strNormal = m_sNormalImage;
		m_pText->SetTextState(0);
	}
	if (strNormal.IsEmpty())
		return;
	SetNormalImage(strNormal);
	m_nNowNormalID = nStatus;
}

void CDockTitleItemUI::SetPos(RECT rc)
{
	CControlUI::SetPos(rc);
	if (m_items.IsEmpty()) return;
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;

	if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
	if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

	if (m_items.GetSize() == 0) {
		ProcessScrollBar(rc, 0, 0);
		return;
	}

	// Determine the minimum size
	SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
	bool verticalLayout = szAvailable.cy > szAvailable.cx;
	if (verticalLayout)
	{
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
			szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();
	}
	else
	{
	}
	int nAdjustables = 0;
	int cyFixed = 0;
	int cxFixed = 0;
	int nEstimateNum = 0;

	for (int it1 = 0; it1 < m_items.GetSize(); it1++) {
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
		if (!pControl->IsVisible()) continue;
		if (pControl->IsFloat()) continue;
		SIZE sz = pControl->EstimateSize(szAvailable);
		if (verticalLayout)
		{
			if (sz.cy == 0) {
				nAdjustables++;
			}
			else {
				if (sz.cy < pControl->GetMinHeight()) sz.cy = pControl->GetMinHeight();
				if (sz.cy > pControl->GetMaxHeight()) sz.cy = pControl->GetMaxHeight();
			}
			cyFixed += sz.cy + pControl->GetPadding().top + pControl->GetPadding().bottom;
		}
		else
		{
			if (sz.cx == 0) {
				nAdjustables++;
			}
			else {
				if (sz.cx < pControl->GetMinWidth()) sz.cx = pControl->GetMinWidth();
				if (sz.cx > pControl->GetMaxWidth()) sz.cx = pControl->GetMaxWidth();
			}
			cxFixed += sz.cx + pControl->GetPadding().left + pControl->GetPadding().right;
		}
		nEstimateNum++;
	}
	cyFixed += (nEstimateNum - 1) * m_iChildPadding;
	cxFixed += (nEstimateNum - 1) * m_iChildPadding;

	// Place elements
	int cyNeeded = 0;
	int cyExpand = 0;
	int cxExpand = 0;
	int cxNeeded = 0;
	if (nAdjustables > 0)
	{
		cyExpand = max(0, (szAvailable.cy - cyFixed) / nAdjustables);
		cxExpand = max(0, (szAvailable.cx - cxFixed) / nAdjustables);
	}
	// Position the elements
	SIZE szRemaining = szAvailable;
	int iPosY = rc.top;
	if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
		iPosY -= m_pVerticalScrollBar->GetScrollPos();
	}
	int iPosX = rc.left;
	if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
		iPosX -= m_pHorizontalScrollBar->GetScrollPos();
	}
	int iAdjustable = 0;
	int cyFixedRemaining = cyFixed;
	int cxFixedRemaining = cxFixed;
	for (int it2 = 0; it2 < m_items.GetSize(); it2++) {
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
		if (!pControl->IsVisible()) continue;
		if (pControl->IsFloat()) {
			SetFloatPos(it2);
			continue;
		}

		RECT rcPadding = pControl->GetPadding();
		if (verticalLayout)
		{
			szRemaining.cy -= rcPadding.top;
			SIZE sz = pControl->EstimateSize(szRemaining);
			if (sz.cy == 0) {
				iAdjustable++;
				sz.cy = cyExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if (iAdjustable == nAdjustables) {
					sz.cy = max(0, szRemaining.cy - rcPadding.bottom - cyFixedRemaining);
				}
				if (sz.cy < pControl->GetMinHeight()) sz.cy = pControl->GetMinHeight();
				if (sz.cy > pControl->GetMaxHeight()) sz.cy = pControl->GetMaxHeight();
			}
			else {
				if (sz.cy < pControl->GetMinHeight()) sz.cy = pControl->GetMinHeight();
				if (sz.cy > pControl->GetMaxHeight()) sz.cy = pControl->GetMaxHeight();
				cyFixedRemaining -= sz.cy;
			}

			sz.cx = pControl->GetFixedWidth();
			if (sz.cx == 0) sz.cx = szAvailable.cx - rcPadding.left - rcPadding.right;
			if (sz.cx < 0) sz.cx = 0;
			if (sz.cx < pControl->GetMinWidth()) sz.cx = pControl->GetMinWidth();
			if (sz.cx > pControl->GetMaxWidth()) sz.cx = pControl->GetMaxWidth();

			RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top + rcPadding.bottom };
			pControl->SetPos(rcCtrl);

			iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
			cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
			szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
		}
		else
		{
			szRemaining.cx -= rcPadding.left;
			SIZE sz = pControl->EstimateSize(szRemaining);
			if (sz.cx == 0) {
				iAdjustable++;
				sz.cx = cxExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if (iAdjustable == nAdjustables) {
					sz.cx = max(0, szRemaining.cx - rcPadding.right - cxFixedRemaining);
				}
				if (sz.cx < pControl->GetMinWidth()) sz.cx = pControl->GetMinWidth();
				if (sz.cx > pControl->GetMaxWidth()) sz.cx = pControl->GetMaxWidth();
			}
			else {
				if (sz.cx < pControl->GetMinWidth()) sz.cx = pControl->GetMinWidth();
				if (sz.cx > pControl->GetMaxWidth()) sz.cx = pControl->GetMaxWidth();

				cxFixedRemaining -= sz.cx;
			}

			sz.cy = pControl->GetFixedHeight();
			if (sz.cy == 0) sz.cy = rc.bottom - rc.top - rcPadding.top - rcPadding.bottom;
			if (sz.cy < 0) sz.cy = 0;
			if (sz.cy < pControl->GetMinHeight()) sz.cy = pControl->GetMinHeight();
			if (sz.cy > pControl->GetMaxHeight()) sz.cy = pControl->GetMaxHeight();

			RECT rcCtrl = { iPosX + rcPadding.left, rc.top + rcPadding.top, iPosX + sz.cx + rcPadding.left + rcPadding.right, rc.top + rcPadding.top + sz.cy };
			pControl->SetPos(rcCtrl);
			iPosX += sz.cx + m_iChildPadding + rcPadding.left + rcPadding.right;
			cxNeeded += sz.cx + rcPadding.left + rcPadding.right;
			szRemaining.cx -= sz.cx + m_iChildPadding + rcPadding.right;
		}


	}
	cyNeeded += (nEstimateNum - 1) * m_iChildPadding;
	cxNeeded += (nEstimateNum - 1) * m_iChildPadding;
	// Process the scrollbar
	if (verticalLayout)
		ProcessScrollBar(rc, 0, cyNeeded);
	else
		ProcessScrollBar(rc, cxNeeded, 0);
}

void CDockTitleItemUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	CUIString strName = pstrName;
	if (strName.Left(4) == _T("item"))
	{
		strName = strName.Right(strName.GetLength() - 4);
		if(m_pText)
			m_pText->SetAttribute(strName.GetData(), pstrValue);
		return;
	}
	else if (strName.Left(4) == _T("icon"))
	{
		strName = strName.Right(strName.GetLength() - 4);
		if(m_pIcon)
			m_pIcon->SetAttribute(strName.GetData(), pstrValue);
		return;
	}
	else if (strName.Left(3) == _T("cls"))
	{
		strName = strName.Right(strName.GetLength() - 3);
		if(m_pClose)
			m_pClose->SetAttribute(strName.GetData(), pstrValue);
		return;
	}
	else if (_tcscmp(pstrName, _T("moveable")) == 0) SetMoveable(_tcscmp(pstrValue, _T("true")) == 0);
	else if (_tcscmp(pstrName, _T("floatout")) == 0) SetFloatItem(_tcscmp(pstrValue, _T("true")) == 0);
	////////////////////////////////////
	else if (_tcscmp(pstrName, _T("group")) == 0) SetGroup(pstrValue);
	else if (_tcscmp(pstrName, _T("selected")) == 0) Selected(_tcscmp(pstrValue, _T("true")) == 0);
	else if (_tcscmp(pstrName, _T("selectedbkcolor")) == 0) {
		if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetSelectedBkColor(clrColor);
	}
	else if (_tcscmp(pstrName, _T("hotbkcolor")) == 0)
	{
		if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetHotBkColor(clrColor);
	}
	else if (_tcscmp(pstrName, _T("normalimage")) == 0) SetNormalImage(pstrValue);
	else if (_tcscmp(pstrName, _T("selectedimage")) == 0) SetSelectedImage(pstrValue);
	else if (_tcscmp(pstrName, _T("selectedhotimage")) == 0) SetSelectedHotImage(pstrValue);
	else if (_tcscmp(pstrName, _T("selectedpushedimage")) == 0) SetSelectedPushedImage(pstrValue);
	else if (_tcscmp(pstrName, _T("hotimage")) == 0) SetHotImage(pstrValue);
	else if (_tcscmp(pstrName, _T("pushedimage")) == 0) SetPushedImage(pstrValue);
	else if (_tcscmp(pstrName, _T("focusedimage")) == 0) SetFocusedImage(pstrValue);
	else if (_tcscmp(pstrName, _T("disabledimage")) == 0) SetDisabledImage(pstrValue);
	else if (_tcscmp(pstrName, _T("foreimage")) == 0) SetForeImage(pstrValue);
	else if (_tcscmp(pstrName, _T("hotforeimage")) == 0) SetHotForeImage(pstrValue);
	else if (_tcscmp(pstrName, _T("pushedforeimage")) == 0) SetForePushedImage(pstrValue);
	else if (_tcscmp(pstrName, _T("listItemtattr")) == 0)
	{
		m_strListItemAttr.Empty();
		m_strListItemAttr = pstrValue;
	}

	CContainerUI::SetAttribute(pstrName, pstrValue);
}

void CDockTitleItemUI::SetOwer(CDockTitleUI* pOwer)
{
	m_pDockItemOwer = pOwer;
}

void CDockTitleItemUI::SetText(LPCTSTR pstrText)
{ 
	if (m_sText == pstrText) return;
	m_sText = pstrText;
	if(m_pText == NULL)
	{
		m_pText = new CDockTitleTextUI;
		LPCTSTR attrList = m_pManager->GetDefaultAttributeList(DUI_CTR_DOCKTITLETEXT);
		m_pText->ApplyAttributeList(attrList);
		m_pText->SetText(pstrText);
		m_pText->SetAttribute(_T("endellipsis"), _T("true"));
		this->AddAt(m_pText,1);
		//this->AddAt(m_pClose,2);
	}
	else
	{
		m_pText->SetText(pstrText);
	}
	Invalidate();
}

void CDockTitleItemUI::CalculateSize()
{
	int length = 0;
	int nItem = 0;
	if (m_pDockItemOwer->IsHorizontal())
	{
		if (m_pIcon && m_pIcon->IsVisible()){
			length += m_pIcon->GetFixedWidth();
			nItem++;
		}
		if (m_pText && m_pText->IsVisible()){
			length += m_pText->GetEstimaLength();
			nItem++;
		}
		if ( m_pClose && m_pClose->IsVisible()){
			length += m_pClose->GetFixedWidth();
			nItem++;
		}
		this->SetFixedWidth(length);
		if(this->GetCount() > nItem)
		{
			for (int i = nItem; i < GetCount(); i++)
			{
				length += (int)GetItemAt(i)->GetFixedWidth();
			}
		}
	}
	else
	{
		if (m_pIcon && m_pIcon->IsVisible()){
			length += m_pIcon->GetFixedHeight();
			nItem++;
		}
		if (m_pText && m_pText->IsVisible()){
			length += m_pText->GetEstimaLength();
			nItem++;
		}
		if ( m_pClose && m_pClose->IsVisible()){
			length += m_pClose->GetFixedHeight();
			nItem++;
		}
		this->SetFixedHeight(length);
		this->Invalidate();
		if (this->GetCount() > nItem)
		{
			for (int i = nItem; i < GetCount(); i++)
			{
				length += (int)GetItemAt(i)->GetFixedHeight();
			}
		}
	}

}


/***************************CDockTitleUI***************************************/

/////////////////list pop wnd///////////////
void CTitleListWnd::Init(CDockTitleUI* pOwner)
{
	m_pOwner = pOwner;
	m_pLayout = NULL;
	m_bScrollBar = false;

	// Position the popup window in absolute space
	SIZE szDrop = m_pOwner->GetListViewSize();
	RECT rcOwner = m_pOwner->m_pListItemOpt->GetPos();
	RECT rc = rcOwner;
	rc.top = rc.bottom - m_pOwner->GetInset().bottom;		// 父窗口left、bottom位置作为弹出窗口起点
	rc.bottom = rc.top + szDrop.cy;	// 计算弹出窗口高度
	if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;	// 计算弹出窗口宽度
	SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
	int cyFixed = 0;
	int cxFixed = 0;
	for( int it = 0; it < pOwner->m_pItemLayout->GetCount(); it++ ) 
	{
		CDockTitleItemUI* pTitleItem = static_cast<CDockTitleItemUI*>(m_pOwner->m_pItemLayout->GetItemAt(it)->GetInterface(_T("DockTitleItem")));			
		if(pTitleItem == NULL || !pTitleItem->IsVisible()) continue;
		SIZE sz = pTitleItem->EstimateSize(szAvailable);
		cyFixed += sz.cy;
		cxFixed = max(sz.cx, cxFixed);
	}
	cyFixed += 2; // CVerticalLayoutUI 默认的Inset 调整
	cxFixed += 1;// 滚动条默认的调整
	rc.bottom = rc.top + min(cyFixed, szDrop.cy);
	if( szDrop.cx <= 0 )
		rc.right = rc.left + cxFixed;
	else
		cxFixed = szDrop.cx;

	::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);

	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CRect rcWork = oMonitor.rcWork;
	if (rc.bottom > rcWork.bottom) 
	{
		rc.left = rcOwner.left;
		rc.right = rc.left + cxFixed;
		rc.top = rcOwner.top - min(cyFixed, szDrop.cy);
		rc.bottom = rcOwner.top;
		::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
	}
	if (rc.right > rcWork.right) 
	{
		rc.left -= cxFixed - (rcOwner.right - rcOwner.left);
		rc.right -= cxFixed - (rcOwner.right - rcOwner.left);
	}

	Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW, rc);
	// HACK: Don't deselect the parent's caption
	HWND hWndParent = m_hWnd;
	while (::GetParent(hWndParent) != NULL)
		hWndParent = ::GetParent(hWndParent);

	::ShowWindow(m_hWnd, SW_SHOW);
	::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

LPCTSTR CTitleListWnd::GetWindowClassName() const
{
	return _T("TitleListWnd");
}

void CTitleListWnd::OnFinalMessage(HWND hWnd)
{
	m_pOwner->m_pListWindow = NULL;
	m_pOwner->m_pListItemOpt->Selected(false);
	m_pOwner->Invalidate();
	delete this;
}

LRESULT CTitleListWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if( uMsg == WM_CREATE ) 
	{
		CUIString strOpGroup;
		strOpGroup.Format(_T("%x"),m_hWnd);
		m_pm.Init(m_hWnd);
		// The trick is to add the items to the new container. Their owner gets
		// reassigned by this operation - which is why it is important to reassign
		// the items back to the righfull owner/manager when the window closes.
		m_pLayout = new CVerticalLayoutUI;
		m_pm.UseParentResource(m_pOwner->GetManager());
		m_pLayout->SetManager(&m_pm, NULL, true);
		//LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("VerticalLayout"));
		// if( pDefaultAttributes ) 
		//    m_pLayout->ApplyAttributeList(pDefaultAttributes);

		m_pLayout->SetInset(CRect(1, 1, 1, 1));
		m_pLayout->SetBkColor(0xFF222222);
		m_pLayout->SetBorderColor(0xFF000000);
		m_pLayout->SetAutoDestroy(false);
		m_pLayout->EnableScrollBar();
		for( int i = 0; i < m_pOwner->m_pItemLayout->GetCount(); i++ ) {
			CDockTitleItemUI* pTitleItem = static_cast<CDockTitleItemUI*>(m_pOwner->m_pItemLayout->GetItemAt(i)->GetInterface(_T("DockTitleItem")));
			if(pTitleItem != NULL && pTitleItem->IsVisible())
			{
				COptionUI* pOp = new COptionUI;
				pOp->SetGroup(strOpGroup.GetData());
				pOp->SetText(pTitleItem->GetText());
				pOp->SetTag(i);
				pOp->ApplyAttributeList(pTitleItem->GetListItemAttr().GetData());
				m_pLayout->Add(pOp);
			}
		}
		m_pm.AttachDialog(m_pLayout);
		m_pLayout->GetVerticalScrollBar()->SetFixedWidth(1);
		return 0;
	}
	else if( uMsg == WM_CLOSE ) 
	{
		for( int i = 0; i < m_pLayout->GetCount(); i++ ) {
			COptionUI* pOp = static_cast<COptionUI*>(m_pLayout->GetItemAt(i));
			if(pOp->IsSelected())
			{
				CDockTitleItemUI* pTitleItem = static_cast<CDockTitleItemUI*>(m_pOwner->m_pItemLayout->GetItemAt((int)pOp->GetTag())->GetInterface(_T("DockTitleItem")));
				if(pTitleItem)
					pTitleItem->Selected(true);
				break;
			}
		}
		m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
		m_pOwner->SetPos(m_pOwner->GetPos());
		m_pOwner->SetFocus();
	}
	else if( uMsg == WM_LBUTTONUP ) 
	{
		if(m_bScrollBar)
			m_bScrollBar = false;
		else
			PostMessage(WM_KILLFOCUS);
	}
	else if( uMsg == WM_LBUTTONDOWN )
	{
		POINT pt = { 0 };
		::GetCursorPos(&pt);
		::ScreenToClient(m_pm.GetPaintWindow(), &pt);
		CControlUI* pControl = m_pm.FindControl(pt);
		if( pControl && _tcscmp(pControl->GetClass(), _T("ScrollBarUI")) != 0 )
			m_bScrollBar = false;
		else
			m_bScrollBar = true;
	}
	else if( uMsg == WM_KEYDOWN ) 
	{
		switch( wParam ) {
		case VK_ESCAPE:
		case VK_RETURN:
			PostMessage(WM_KILLFOCUS);
			break;
		default:
			return 0;
		}
	}
	else if( uMsg == WM_MOUSEWHEEL ) 
	{
		int zDelta = (int) (short) HIWORD(wParam);
		zDelta < 0 ? /*SB_LINEDOWN*/Scroll(0, 8): /*SB_LINEUP*/ Scroll(0, -8);
		return 0;
	}
	else if( uMsg == WM_KILLFOCUS ) 
	{
		if( m_hWnd != (HWND) wParam ) PostMessage(WM_CLOSE);
	}

	LRESULT lRes = 0;
	if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void CTitleListWnd::Scroll(int dx, int dy)
{
	if (dx == 0 && dy == 0) return;
	SIZE sz = m_pLayout->GetScrollPos();
	m_pLayout->SetScrollPos(CSize(sz.cx + dx, sz.cy + dy));
}

#if(_WIN32_WINNT >= 0x0501)
UINT CTitleListWnd::GetClassStyle() const
{
	return __super::GetClassStyle() | CS_DROPSHADOW;
}
#endif
/////////////////////////end list wnd////////////////////////

CDockTitleUI::CDockTitleUI()
{
	m_nTitle = 40;
	m_pItemLayout = NULL;
	m_pTitleOwer = NULL;
	m_pSelected = NULL;
	m_pMiniBtn = NULL;
	m_pListItemOpt = NULL;
	m_pListWindow = NULL;
	m_szView.cx = 0;
	m_szView.cy = 140;
	m_bCalcsize = false;
	m_bMoveItem = false;
	m_sUniGroupName.Format(_T("%x"),this);
	SetLayoutMethod(Horizontal_Method);
}

void CDockTitleUI::SetLayoutMethod(LayoutMethod method)
{
	std::vector<CControlUI*> vItems;
	std::vector<std::pair<int,int>> vItemsWH;
	CControlUI* pControl = NULL;
	CUIString strItemLayoutAttr = GetCommonArrt(m_pItemLayout);
	strItemLayoutAttr.Append(GetLayoutArrt(m_pItemLayout));
	if (m_pItemLayout)
	{
		while (m_pItemLayout->GetCount())
		{
			pControl = m_pItemLayout->GetItemAt(0);
			vItemsWH.push_back(std::make_pair(pControl->GetFixedWidth(),pControl->GetFixedHeight()));
			//clear width,heiht
			pControl->SetFixedWidth(0);
			pControl->SetFixedHeight(0);
			vItems.push_back(pControl);

			m_pItemLayout->RemoveNotDestroy(pControl);
		}
		this->SetDelayedDestroy(false);
		this->Remove(m_pItemLayout);
		this->SetDelayedDestroy(true);
		m_pItemLayout = NULL;
	}
	this->RemoveAll();
	if (method == Vertical_Method)
	{
		this->SetFixedWidth(m_nTitle);
		m_pItemLayout = new CVerticalLayoutUI;
		this->SetFixedHeight(0);
	}
	else
	{
		this->SetFixedHeight(m_nTitle);
		m_pItemLayout = new CHorizontalLayoutUI;
		this->SetFixedWidth(0);
	}
	this->Add(m_pItemLayout);
	m_pItemLayout->ApplyAttributeList(strItemLayoutAttr.GetData());
	m_pItemLayout->EnableScrollBar(method == Vertical_Method,method != Vertical_Method);
	if(_tcscmp(m_pItemLayout->GetClass() ,_T("VerticalLayoutUI"))== 0)
	{
		CScrollBarUI* pBar = m_pItemLayout->GetVerticalScrollBar();
		if(pBar)
		{
			pBar->SetFixedWidth(0); //隐藏滚动条
		}
	}
	else
	{
		CScrollBarUI* pBar = m_pItemLayout->GetHorizontalScrollBar();
		if(pBar)
		{
			pBar->SetFixedHeight(0); //隐藏滚动条
		}
	}
	m_pMiniBtn = new CButtonUI;
	m_pMiniBtn->SetFixedHeight(m_nTitle);
	m_pMiniBtn->SetFixedWidth(m_nTitle);
	m_pMiniBtn->SetVisible(false);
	m_pMiniBtn->ApplyAttributeList(m_strMiniAttr.GetData());
	m_pMiniBtn->OnEvent += MakeDelegate(this,&CDockTitleUI::DoItemEvent);
	m_pListItemOpt = new COptionUI;
	m_pListItemOpt->SetFixedHeight(m_nTitle);
	m_pListItemOpt->SetFixedWidth(m_nTitle);
	m_pListItemOpt->SetVisible(false);
	m_pListItemOpt->ApplyAttributeList(m_strListOptAttr.GetData());
	m_pListItemOpt->OnEvent += MakeDelegate(this,&CDockTitleUI::DoItemEvent);
	this->Add(m_pMiniBtn);
	this->Add(m_pListItemOpt);

	for (int index = 0; index < vItems.size();index++)
	{
		m_pItemLayout->Add(vItems[index]);
		//restone height/ width
		vItems[index]->SetFixedHeight(vItemsWH[index].second);
		vItems[index]->SetFixedWidth(vItemsWH[index].first);
	}
	if(vItems.size() > 0)
	{
		SelectTitle((CDockTitleItemUI*)vItems[0]);
		((CDockTitleItemUI*)vItems[0])->Selected(true);
	}
	this->SetCalculateSize(m_bCalcsize);
	this->SetTitleLength(m_nTitle);
	this->Invalidate();
}

void CDockTitleUI::SetTitleLength(int length)
{
	if (length >= 0)
		m_nTitle = length;
	else
		return;
	if (m_pItemLayout)
	{
		CUIString strClass = m_pItemLayout->GetClass();
		if (strClass == _T("HorizontalLayoutUI"))
			this->SetFixedHeight(m_nTitle);
		else
			this->SetFixedWidth(m_nTitle);
	}
}

int CDockTitleUI::GetTitleItemIndex()
{
	return m_pItemLayout->GetItemIndex(m_pSelected);
}

int CDockTitleUI::GetTitleItemIndex(CControlUI* pControl)
{
	return m_pItemLayout->GetItemIndex(pControl);
}

void CDockTitleUI::SetPos(RECT rc)
{
	CControlUI::SetPos(rc);
	if (m_items.IsEmpty()) return;
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;

	if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
	if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

	if (m_items.GetSize() == 0) {
		ProcessScrollBar(rc, 0, 0);
		return;
	}

	// Determine the minimum size
	SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
	bool verticalLayout = szAvailable.cy > szAvailable.cx;
	if (verticalLayout)
	{
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
			szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();
	}
	else
	{
	}
	int nAdjustables = 0;
	int cyFixed = 0;
	int cxFixed = 0;
	int nEstimateNum = 0;

	for (int it1 = 0; it1 < m_items.GetSize(); it1++) {
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
		if (!pControl->IsVisible()) continue;
		if (pControl->IsFloat()) continue;
		SIZE sz = pControl->EstimateSize(szAvailable);
		if (verticalLayout)
		{
			if (sz.cy == 0) {
				nAdjustables++;
			}
			else {
				if (sz.cy < pControl->GetMinHeight()) sz.cy = pControl->GetMinHeight();
				if (sz.cy > pControl->GetMaxHeight()) sz.cy = pControl->GetMaxHeight();
			}
			cyFixed += sz.cy + pControl->GetPadding().top + pControl->GetPadding().bottom;
		}
		else
		{
			if (sz.cx == 0) {
				nAdjustables++;
			}
			else {
				if (sz.cx < pControl->GetMinWidth()) sz.cx = pControl->GetMinWidth();
				if (sz.cx > pControl->GetMaxWidth()) sz.cx = pControl->GetMaxWidth();
			}
			cxFixed += sz.cx + pControl->GetPadding().left + pControl->GetPadding().right;
		}
		nEstimateNum++;
	}
	cyFixed += (nEstimateNum - 1) * m_iChildPadding;
	cxFixed += (nEstimateNum - 1) * m_iChildPadding;

	// Place elements
	int cyNeeded = 0;
	int cyExpand = 0;
	int cxExpand = 0;
	int cxNeeded = 0;
	if (nAdjustables > 0)
	{
		cyExpand = max(0, (szAvailable.cy - cyFixed) / nAdjustables);
		cxExpand = max(0, (szAvailable.cx - cxFixed) / nAdjustables);
	}
	// Position the elements
	SIZE szRemaining = szAvailable;
	int iPosY = rc.top;
	if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
		iPosY -= m_pVerticalScrollBar->GetScrollPos();
	}
	int iPosX = rc.left;
	if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
		iPosX -= m_pHorizontalScrollBar->GetScrollPos();
	}
	int iAdjustable = 0;
	int cyFixedRemaining = cyFixed;
	int cxFixedRemaining = cxFixed;
	for (int it2 = 0; it2 < m_items.GetSize(); it2++) {
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
		if (!pControl->IsVisible()) continue;
		if (pControl->IsFloat()) {
			SetFloatPos(it2);
			continue;
		}

		RECT rcPadding = pControl->GetPadding();
		if (verticalLayout)
		{
			szRemaining.cy -= rcPadding.top;
			SIZE sz = pControl->EstimateSize(szRemaining);
			if (sz.cy == 0) {
				iAdjustable++;
				sz.cy = cyExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if (iAdjustable == nAdjustables) {
					sz.cy = max(0, szRemaining.cy - rcPadding.bottom - cyFixedRemaining);
				}
				if (sz.cy < pControl->GetMinHeight()) sz.cy = pControl->GetMinHeight();
				if (sz.cy > pControl->GetMaxHeight()) sz.cy = pControl->GetMaxHeight();
			}
			else {
				if (sz.cy < pControl->GetMinHeight()) sz.cy = pControl->GetMinHeight();
				if (sz.cy > pControl->GetMaxHeight()) sz.cy = pControl->GetMaxHeight();
				cyFixedRemaining -= sz.cy;
			}

			sz.cx = pControl->GetFixedWidth();
			if (sz.cx == 0) sz.cx = szAvailable.cx - rcPadding.left - rcPadding.right;
			if (sz.cx < 0) sz.cx = 0;
			if (sz.cx < pControl->GetMinWidth()) sz.cx = pControl->GetMinWidth();
			if (sz.cx > pControl->GetMaxWidth()) sz.cx = pControl->GetMaxWidth();

			RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top + rcPadding.bottom };
			pControl->SetPos(rcCtrl);

			iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
			cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
			szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
		}
		else
		{
			szRemaining.cx -= rcPadding.left;
			SIZE sz = pControl->EstimateSize(szRemaining);
			if (sz.cx == 0) {
				iAdjustable++;
				sz.cx = cxExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if (iAdjustable == nAdjustables) {
					sz.cx = max(0, szRemaining.cx - rcPadding.right - cxFixedRemaining);
				}
				if (sz.cx < pControl->GetMinWidth()) sz.cx = pControl->GetMinWidth();
				if (sz.cx > pControl->GetMaxWidth()) sz.cx = pControl->GetMaxWidth();
			}
			else {
				if (sz.cx < pControl->GetMinWidth()) sz.cx = pControl->GetMinWidth();
				if (sz.cx > pControl->GetMaxWidth()) sz.cx = pControl->GetMaxWidth();

				cxFixedRemaining -= sz.cx;
			}

			sz.cy = pControl->GetFixedHeight();
			if (sz.cy == 0) sz.cy = rc.bottom - rc.top - rcPadding.top - rcPadding.bottom;
			if (sz.cy < 0) sz.cy = 0;
			if (sz.cy < pControl->GetMinHeight()) sz.cy = pControl->GetMinHeight();
			if (sz.cy > pControl->GetMaxHeight()) sz.cy = pControl->GetMaxHeight();

			RECT rcCtrl = { iPosX + rcPadding.left, rc.top + rcPadding.top, iPosX + sz.cx + rcPadding.left + rcPadding.right, rc.top + rcPadding.top + sz.cy };
			pControl->SetPos(rcCtrl);
			iPosX += sz.cx + m_iChildPadding + rcPadding.left + rcPadding.right;
			cxNeeded += sz.cx + rcPadding.left + rcPadding.right;
			szRemaining.cx -= sz.cx + m_iChildPadding + rcPadding.right;
		}


	}
	cyNeeded += (nEstimateNum - 1) * m_iChildPadding;
	cxNeeded += (nEstimateNum - 1) * m_iChildPadding;
	// Process the scrollbar
	if (verticalLayout)
		ProcessScrollBar(rc, 0, cyNeeded);
	else
		ProcessScrollBar(rc, cxNeeded, 0);
}

void CDockTitleUI::DoEvent(TEventUI& event_)
{
	if (event_.Type == UIEVENT_SCROLLWHEEL)
	{
		switch (LOWORD(event_.wParam))
		{
		case SB_LINEUP:
			//::OutputDebugStringA("SB_LINEUP\n");
			break;
		case SB_LINEDOWN:
			//::OutputDebugStringA("SB_LINEDOWN\n");
			break;
		}
	}
	CContainerUI::DoEvent(event_);
}

bool CDockTitleUI::DoItemEvent(void* pevent_)
{
	TEventUI event_ = *((TEventUI*)pevent_);
	if (event_.Type == UIEVENT_BUTTONDOWN)
	{
		if(event_.pSender == m_pMiniBtn && m_pTitleOwer)
		{
			m_pTitleOwer->SetMini(!m_pTitleOwer->GetMini());
		}
		else if(event_.pSender == m_pListItemOpt)
		{
			m_pListWindow = new CTitleListWnd;
			m_pListWindow->Init(this);
		}
	}
	return true;
}

void CDockTitleUI::FloatWindow(FloatMethod method)
{
	if (method == DBLCLICK_FLOAT)
	{
		//::OutputDebugStringA("DBLCLICK_FLOAT\n");
		m_pManager->SetTimer(m_pTitleOwer,0x20,10);
	}
	else
	{
		//::OutputDebugStringA("DRAG_FLOAT\n");
		m_pManager->SetTimer(m_pTitleOwer,0x40,10);
	}
}

void CDockTitleUI::MoveTitleItem(bool left)
{
	int nMove = GetTitleItemIndex();
	int nMoveTo = nMove;
	if (left)
		nMoveTo--;
	else
		nMoveTo++;
	if (nMoveTo < 0 || (m_pItemLayout->GetCount() - 1) < nMoveTo)
		return;
	m_pItemLayout->MoveItem(nMove, nMoveTo);
	if(m_pTitleOwer)
	{
		m_pTitleOwer->MoveDocker(nMove, nMoveTo);
	}
	m_pItemLayout->Invalidate();
}

void CDockTitleUI::SelectTitle(CDockTitleItemUI* pSelect)
{
	int Old = GetTitleItemIndex();
	m_pSelected = pSelect;
	int New = GetTitleItemIndex();
	if(m_pTitleOwer)
		m_pTitleOwer->SelectDocker(Old,New);
}

void CDockTitleUI::SetOwer(CDockPanelUI* pPanel)
{
	m_pTitleOwer =  pPanel;
}

bool CDockTitleUI::Add(CControlUI* pControl)
{
	if (pControl == NULL) return false;
	CUIString className = pControl->GetClass();
	if (className == _T("DockTitleItemUI"))
	{
		((CDockTitleItemUI*)pControl)->SetOwer(this);
		((CDockTitleItemUI*)pControl)->SetGroup(m_sUniGroupName.GetData());
		bool ret = m_pItemLayout->Add(pControl);
		if(m_pItemLayout->GetCount() == 1)
		{
			SelectTitle((CDockTitleItemUI*)pControl);
			((CDockTitleItemUI*)pControl)->Selected(true);
		}
		return ret;
	}
	else
		return CContainerUI::Add(pControl);
}

bool CDockTitleUI::AddAt(CControlUI* pControl, int nIndex)
{
	if (pControl == NULL) return false;

	CUIString className = pControl->GetClass();
	if (className == _T("DockTitleItemUI"))
	{
		((CDockTitleItemUI*)pControl)->SetOwer(this);
		((CDockTitleItemUI*)pControl)->SetGroup(m_sUniGroupName.GetData());
		//if (m_pItemLayout->GetCount() == 0) ((CDockTitleItemUI*)pControl)->Selected(true);
		return m_pItemLayout->AddAt(pControl, nIndex);
	}
	else
		return CContainerUI::AddAt(pControl, nIndex);
}

void CDockTitleUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	CUIString strName = pstrName;
	if (strName.Left(4) == _T("item"))
	{
		strName = strName.Right(strName.GetLength() - 4);
		SetItemsAttribute(strName.GetData(), pstrValue);
		return;
	}
	else if (_tcscmp(pstrName, _T("listitemview")) == 0) 
	{
		SetItemView(_tcscmp(pstrValue, _T("true")) == 0);
	}
	else if (strName.Left(6) == _T("layout"))
	{
		strName = strName.Right(strName.GetLength() - 6);
		if(m_pItemLayout)
		{
			m_pItemLayout->SetAttribute(strName.GetData(), pstrValue);
		}
		return;
	}
	else if(strName == _T("mainlayout"))
	{ 
		if (_tcscmp(pstrName, _T("horizontal")) == 0)
			SetLayoutMethod();
		else
			SetLayoutMethod(Vertical_Method);
	}
	else if (strName == _T("heighttitle"))
	{
		SetTitleLength(_ttoi(pstrValue));
	}
	else if (_tcscmp(pstrName, _T("calcsize")) == 0)
	{
		SetCalculateSize(_tcscmp(pstrValue, _T("true")) == 0);	
	}
	else if (_tcscmp(pstrName, _T("miniattr")) == 0)
	{
		m_strMiniAttr.Empty();
		m_strMiniAttr = pstrValue;
		if(m_pMiniBtn)
			m_pMiniBtn->ApplyAttributeList(m_strMiniAttr.GetData());
	}
	else if (_tcscmp(pstrName, _T("listoptattr")) == 0)
	{
		m_strListOptAttr.Empty();
		m_strListOptAttr = pstrValue;
		if(m_pListItemOpt)
			m_pListItemOpt->ApplyAttributeList(m_strListOptAttr.GetData());
	}
	CContainerUI::SetAttribute(pstrName, pstrValue);
}

void CDockTitleUI::SetCalculateSize(bool bCalcsize)
{
	m_bCalcsize = bCalcsize;
	if (m_bCalcsize)
	{
		int nCount = m_pItemLayout->GetCount();
		for (size_t i = 0; i < nCount; i++)
		{
			((CDockTitleItemUI*)m_pItemLayout->GetItemAt((int)i))->CalculateSize();
		}
	}
}

void CDockTitleUI::SetItemsAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	int nCount = m_pItemLayout->GetCount();
	for (size_t i = 0; i < nCount; i++)
	{
		m_pItemLayout->GetItemAt((int)i)->SetAttribute(pstrName, pstrValue);
	}
}

CContainerUI * CDockTitleUI::GetItemsLayout()
{
	return m_pItemLayout;
}

CDockTitleItemUI* CDockTitleUI::GetTitleItemAt(int index)
{
	CControlUI* pUI = m_pItemLayout->GetItemAt(index);
	if(pUI)
		return static_cast<CDockTitleItemUI*>(pUI);
	else
		return NULL;
}

int CDockTitleUI::GetTitleItemCount()
{
	return m_pItemLayout->GetCount();
}

void CDockTitleUI::FloatTitleItemOut(int index)
{
	FloatTitleItemOut(static_cast<CDockTitleItemUI*>(m_pItemLayout->GetItemAt(index)));
}

void CDockTitleUI::FloatTitleItemOut(CDockTitleItemUI* pItem)
{
	pItem->SetGroup(_T(""));
	pItem->Selected(false);
	pItem->SetOwer(NULL);
	int outIndex = m_pItemLayout->GetItemIndex(pItem);
	int maxIndex = m_pItemLayout->GetCount()-1;
	BOOL bSelected = FALSE;
	//判断浮出后下一个显示的Tab
	do{
		//前面有元素
		for(int i = outIndex-1; i >= 0; i--)
		{
			CDockTitleItemUI* pItemUI = static_cast<CDockTitleItemUI*>(m_pItemLayout->GetItemAt(i));
			if(!pItemUI->IsVisible()) continue; //跳过隐藏的Tab
			pItemUI->Selected(true);
			bSelected = TRUE;
			break;
		}
		if(bSelected) break;
		//后面有元素
		for(int i = outIndex+1; i <= maxIndex; i++)
		{
			CDockTitleItemUI* pItemUI =  static_cast<CDockTitleItemUI*>(m_pItemLayout->GetItemAt(i));
			if(!pItemUI->IsVisible()) continue; //跳过隐藏的Tab
			pItemUI->Selected(true);
			break;
		}
	}while(false);
	m_pItemLayout->RemoveNotDestroy(pItem);
}

void CDockTitleUI::RemoveItemFormGroup(CDockTitleItemUI* pItem)
{
	pItem->SetGroup(_T(""));
}

void CDockTitleUI::SetItemView(bool bView)
{
	m_incView = !bView;
}

bool CDockTitleUI::GetItemView()
{
	return m_incView;
}

/***************************CDockPanelUI*********************************/

CDockPanelUI::CDockPanelUI(void)
{
	m_titlePosition = _T('N');
	m_pParent.m_pParentWin = NULL;
	m_pTitleLayout = NULL;
	m_pDockerLayout = NULL;
	m_bAcceptDocking = false;
	m_titleFixedHeight = 40;
	this->SetFloatColor(0xff2D3E5B);
	CDockManager::AddDockPanelToWatch(this);
	m_bMini = false;
	m_nOldFixed = m_nPanelState = 0;
	m_dwWinStyle = UI_WNDSTYLE_FRAME;
	m_dwWinExStyle = 0;
}

CDockPanelUI::~CDockPanelUI(void)
{
	CDockManager::RemoveDockPanel(this);
}

CControlUI* CDockPanelUI::CreateControl(LPCTSTR pstrClass)
{
	CDockBuilderCallback callback;
	return callback.CreateControl(pstrClass);
}

LPCTSTR CDockPanelUI::GetClass() const
{
	return _T("DockPanelUI");
}

LPVOID CDockPanelUI::GetInterface(LPCTSTR pstrName)
{
	if(_tcscmp(pstrName, DUI_CTR_DOCKPANEL) == 0)
		return this;
	return __super::GetInterface(pstrName);
}

CDockerUI* CDockPanelUI::GetDockerAt(int nIndex) const
{
	if( nIndex < 0 || nIndex >= m_arrDockPtr.GetSize() ) return NULL;
	return static_cast<CDockerUI*>(m_arrDockPtr[nIndex]);
}

int CDockPanelUI::GetDockerIndex(CControlUI* pControl) const
{
	for( int it = 0; it < m_arrDockPtr.GetSize(); it++ ) {
		if( static_cast<CControlUI*>(m_arrDockPtr[it]) == pControl ) {
			return it;
		}
	}
	return -1;
}

bool CDockPanelUI::SetDockerIndex(CControlUI* pControl, int nIndex)
{
	for( int it = 0; it < m_arrDockPtr.GetSize(); it++ ) {
		if( static_cast<CControlUI*>(m_arrDockPtr[it]) == pControl ) {
			MoveDocker(it,nIndex);
		}
	}
	return false;
}

int CDockPanelUI::GetDockerCount() const
{
	return m_arrDockPtr.GetSize();
}

bool  CDockPanelUI::Add(CControlUI* pControl)
{
	if(_tcscmp(pControl->GetClass(),  _T("DockerUI")) == 0)
		return AddDocker(pControl);
	else if(_tcscmp(pControl->GetClass(),  _T("DockTitleUI")) == 0)
	{
		if(m_pTitleLayout == NULL)
			m_pTitleLayout = static_cast<CDockTitleUI*>(pControl);
		else
			delete  pControl; //only one Title
		return true;
	}
	else
		return __super::Add(pControl);
}

bool CDockPanelUI::AddDocker(CControlUI* pControl)
{
	CDockerUI* pDocker = static_cast<CDockerUI*>(pControl);
	if( pDocker == NULL) return false;
	if(m_pTitleLayout == NULL)
	{
		m_pTitleLayout = new CDockTitleUI();
	}
	else if(m_pDockerLayout == NULL)
	{
		m_pDockerLayout = new CHorizontalLayoutUI();
	}
	m_arrDockPtr.Add(pControl);
	m_pDockerLayout->Add(pDocker);
	int index = m_arrDockPtr.GetSize() - 1;
	if(m_pTitleLayout->GetTitleItemCount() < m_arrDockPtr.GetSize() ) //less than DockPtr ,add TitleItem
	{
		if(pDocker->GetTitleItem() != NULL) //use old 
		{
			m_pTitleLayout->Add(pDocker->GetTitleItem());
		}
		else
		{
			CUIString strText = pDocker->GetText();
			CDockTitleItemUI* pTitleItem = new CDockTitleItemUI;
			LPCTSTR attrList = m_pManager->GetDefaultAttributeList(DUI_CTR_DOCKTITLEITEM);
			pTitleItem->ApplyAttributeList(attrList);
			pTitleItem->SetText(strText.GetData());
			pDocker->SetTitleItem(pTitleItem);
			m_pTitleLayout->Add(pTitleItem);
		}
	}
	else //set TitleItem to Docker
	{
		pDocker->SetTitleItem(m_pTitleLayout->GetTitleItemAt(index));
	}
	pDocker->SetVisible(false);
	if(m_pDockerLayout->GetCount() == 1)
		pDocker->SetVisible(true);
	pDocker->SetOwner(this);
	pDocker->SetIndexOfDocker(index);
	pDocker->SetBuilderCallback(m_pParent.m_pParentWin);
	return true;
}

bool CDockPanelUI::AddDockerAt(CControlUI* pControl, int nIndex, bool reIn /*= false*/)
{
	if( pControl == NULL) return false;
	CDockerUI* pDocker = static_cast<CDockerUI*>(pControl);
	if(!reIn)
	{
		if(!m_pDockerLayout->AddAt(pControl, nIndex ))
		{
			nIndex = 0;
			if(!m_pDockerLayout->AddAt(pControl, nIndex )) return false;
		}
		//reset docker index
		for(int index = nIndex; index < m_arrDockPtr.GetSize() ; index++)
		{
			CDockerUI* pDocker = static_cast<CDockerUI*>(m_arrDockPtr.GetAt(index));
			pDocker->SetIndexOfDocker(pDocker->GetPostionOfDocker()+1);
		}
	}
	else
	{
		for(int index = 0; index < m_arrDockPtr.GetSize()  ; index++)
		{
			CDockerUI* pDocker = static_cast<CDockerUI*>(m_arrDockPtr.GetAt(index));
			if(pDocker->GetPostionOfDocker() >= nIndex)
			{
				nIndex = index;
				break;
			}
		}
		if(nIndex < 0)
			nIndex = 0;
		if(nIndex > m_arrDockPtr.GetSize())
		{
			nIndex = m_arrDockPtr.GetSize();
		}
		if(!m_pDockerLayout->AddAt(pControl, nIndex )) return false;
	}
	m_pTitleLayout->AddAt(pDocker->GetTitleItem(),nIndex);
	//pDocker->SetIndexOfDocker(nIndex);
	pDocker->SetVisible(false);
	this->Invalidate();
	return m_arrDockPtr.InsertAt(nIndex, pControl);
}

bool CDockPanelUI::FloatDockerOut(CControlUI* pControl)
{
	if( pControl == NULL) return false;
	int index = m_arrDockPtr.Find(pControl);
	if(index == -1) return false;
	m_arrDockPtr.Remove(index);
	CDockerUI* pDocker = static_cast<CDockerUI*>(pControl);
	m_pTitleLayout->FloatTitleItemOut(pDocker->GetTitleItem());// remove TitleItem first
	m_pDockerLayout->RemoveNotDestroy(pDocker); 
	return true;
}

bool CDockPanelUI::FloatDockerOut(int nIndex)
{
	CDockerUI* pDocker = NULL;
	pDocker = static_cast<CDockerUI*>(m_pDockerLayout->GetItemAt(nIndex));
	if(pDocker)
	{
		RemoveDocker(pDocker);
		return true;
	}
	return false;
}

bool CDockPanelUI::RemoveDocker(CControlUI* pControl)
{
	int index = m_arrDockPtr.Find(pControl);
	if(index == -1) return false;
	m_arrDockPtr.Remove(index);
	CDockerUI* pDocker = static_cast<CDockerUI*>(pControl);
	m_pTitleLayout->GetItemsLayout()->RemoveAt(index);
	m_pDockerLayout->Remove(pControl);
	return true;
}

bool CDockPanelUI::RemoveDockerAt(int nIndex)
{
	return RemoveDocker(m_pDockerLayout->GetItemAt(nIndex));
}

void CDockPanelUI::RemoveAllDocker()
{
	m_pDockerLayout->RemoveAll();
	m_pTitleLayout->GetItemsLayout()->RemoveAll();
	m_arrDockPtr.Empty();
}

void CDockPanelUI::SetTitlePosition(TCHAR position)
{
	switch(position)
	{
	case _T('E'):
		m_titlePosition = _T('E');
		break;
	case _T('W'):
		m_titlePosition = _T('W');
		break;
	case _T('S'):
		m_titlePosition = _T('S');
		break;
	default:
		m_titlePosition = _T('N');
	}
	ReArrangeItems();
}

TCHAR CDockPanelUI::GetTitlePosition()
{
	return m_titlePosition;
}

bool CDockPanelUI::GetTitlePositionHor()
{
	switch(m_titlePosition)
	{
	case _T('E'):
		break;
	case _T('W'):
		break;
	default:
		return true;
	}
	return false;
}

void CDockPanelUI::SetTitleFixed(int  titlefixed)
{
	m_titleFixedHeight = titlefixed;
}

int CDockPanelUI::GetTitleFixed()
{
	return m_titleFixedHeight;
}

void CDockPanelUI::SetRestore(bool restore)
{
	m_restore = restore;
}

bool CDockPanelUI::GetRestore()
{
	return m_restore;
}

std::vector<UINT> CDockPanelUI::GetWinLayoutIndex()
{
	return m_nLayoutIndex;
}

CUIString CDockPanelUI::GetUniGroup() const
{
	return m_sUniGroup;
}

void CDockPanelUI::SetUniGroup(LPCTSTR pStrGroupName)
{
	m_sUniGroup.Assign(pStrGroupName);
	SetDockersAttribute(_T("group"),pStrGroupName);
}

void CDockPanelUI::SetParentWindow(CWindowUI* pWin)
{
	m_pParent.m_pParentWin = pWin;
}

void CDockPanelUI::SetParentWindow(INotifyUI* pWin)
{
	m_pParent.m_pParentNotify = pWin;
}

INotifyUI* CDockPanelUI::GetParentNotify()
{
	return m_pParent.m_pParentNotify;
}

void CDockPanelUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	CUIString strName = pstrName;
	if (strName.Left(4) == _T("doer"))
	{
		strName = strName.Right(strName.GetLength() - 4);
		SetDockersAttribute(strName.GetData(), pstrValue);
		return;
	}
	else if (_tcscmp(pstrName, _T("floatcolor")) == 0) {
		if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetFloatColor(clrColor);
	}
	else if (_tcscmp(pstrName, _T("titlepos")) == 0) 
	{
		SetTitlePosition((TCHAR)(*pstrValue));
	}
	else if (_tcscmp(pstrName, _T("winxml")) == 0) 
		m_winXml.Assign(pstrValue);
	else if (_tcscmp(pstrName, _T("unigroup")) == 0) 
		SetUniGroup(pstrValue);
	else if (_tcscmp(pstrName, _T("titlefixed")) == 0)
	{
		SetTitleFixed(_ttoi(pstrValue));
	}
	else if (_tcscmp(pstrName, _T("winstyle")) == 0)
	{
		LPTSTR pstr = NULL;
		DWORD  dwStyle= _tcstoul(pstrValue, &pstr, 16);
		DWORD  dwExStyle= _tcstoul(pstr+1, &pstr, 16);
		SetWinStyle(dwStyle,dwExStyle);
	}
	else if (_tcscmp(pstrName, _T("winindex")) == 0) 
	{
		LPTSTR pstr = NULL;
		UINT nNum = 0;
		m_nLayoutIndex.clear();
		nNum = _tcstol(pstrValue, &pstr, 10); 
		if(_tcscmp(pstr,_T(""))==0 || _tcscmp(pstr,_T(" "))==0)
		{
			m_nLayoutIndex.push_back(nNum);
			return;
		}
		m_nLayoutIndex.push_back(nNum);
		do{
			nNum = _tcstol(pstr+1, &pstr, 10); 
			ASSERT(pstr);  
			if(_tcscmp(pstr,_T(""))==0)
				break;
			pstrValue++;
			if(_tcscmp(pstr,_T(" "))==0)
				continue;
			m_nLayoutIndex.push_back(nNum);
		}while(true);
		return;
	}
	else if (_tcscmp(pstrName, _T("restore")) == 0) 
		SetRestore(_tcscmp(pstrValue, _T("true")) == 0);
	__super::SetAttribute(pstrName, pstrValue);
}

void CDockPanelUI::DoEvent(TEventUI& event_)
{
	if (event_.Type == UIEVENT_TIMER)
	{
		switch(event_.wParam)
		{
		case 0x20:
			m_pManager->KillTimer(this,0x20);
			FloatDocker(false);
			break;
		case 0x40:
			m_pManager->KillTimer(this,0x40);
			FloatDocker(true);
			break;
		}
	}
	__super::DoEvent(event_);
}

void CDockPanelUI::ReArrangeItems()
{
	CContainerUI* pMain = NULL;
	int nCount = this->GetCount();
	std::vector<CControlUI*> vDockers;
	if(nCount > 0) //has main layout
	{
		pMain = (CContainerUI*)GetItemAt(0);
		pMain->RemoveNotDestroy(m_pDockerLayout);	
		pMain->RemoveNotDestroy(m_pTitleLayout);
		this->SetDelayedDestroy(false);
		this->RemoveAll();
		this->SetDelayedDestroy(true);
		pMain = NULL;
	}
	if(m_titlePosition == 'N' || m_titlePosition == 'S')
		pMain = new CVerticalLayoutUI();
	else
		pMain = new CHorizontalLayoutUI();
	this->Add(pMain);
	switch(m_titlePosition)
	{
	case _T('E'):
		pMain->Add(m_pDockerLayout);
		pMain->Add(m_pTitleLayout);
		break;
	case _T('W'):
		pMain->Add(m_pTitleLayout);
		pMain->Add(m_pDockerLayout);
		break;
	case _T('S'):
		pMain->Add(m_pDockerLayout);
		pMain->Add(m_pTitleLayout);
		break;
	default:
		pMain->Add(m_pTitleLayout);
		pMain->Add(m_pDockerLayout);
	}
	m_pTitleLayout->SetOwer(this);
	//Setlayout after add
	m_pTitleLayout->SetTitleLength(m_titleFixedHeight);
	if(m_titlePosition == 'N' || m_titlePosition == 'S')
		m_pTitleLayout->SetLayoutMethod(CDockTitleUI::Horizontal_Method);
	else
		m_pTitleLayout->SetLayoutMethod(CDockTitleUI::Vertical_Method);
	this->Invalidate();
}

void CDockPanelUI::MoveDocker(int src, int to)
{
	if(m_arrDockPtr.Move(src,to))
	{
		int to_pos = static_cast<CDockerUI*>(m_pDockerLayout->GetItemAt(to))->GetPostionOfDocker();
		int src_pos = static_cast<CDockerUI*>(m_pDockerLayout->GetItemAt(src))->GetPostionOfDocker();
		static_cast<CDockerUI*>(m_pDockerLayout->GetItemAt(src))->SetIndexOfDocker(to_pos);
		static_cast<CDockerUI*>(m_pDockerLayout->GetItemAt(to))->SetIndexOfDocker(src_pos);
		m_pDockerLayout->MoveItem(src,to);
	}
	m_pDockerLayout->Invalidate();
}

void CDockPanelUI::FloatDocker(bool bMousePt)
{
	int index = m_pTitleLayout->GetTitleItemIndex();
	HWND hPaint = m_pManager->GetPaintWindow();

	CPoint ptMouse;
	::GetCursorPos(&ptMouse);
	::ScreenToClient(hPaint,&ptMouse);
	::SendMessage(hPaint, WM_LBUTTONUP, ptMouse.x, ptMouse.y);
	CDockerUI* pDocker = GetDockerAt(index);
	if(bMousePt)
	{
		CDockerWindow<CDockerUI,CWindowUI> *pFloatw = new CDockerWindow<CDockerUI,CWindowUI>;
		pFloatw->Init(GetManager(),pDocker,m_dwWinStyle,m_dwWinExStyle,m_winXml.GetData());
		if(!::IsWindow(pFloatw->GetHWND()))
			return;
		pFloatw->MoveWithMousePt();
	}
	else
	{
		CDockerWindow<CDockerUI,CWindowUI> *pFloatw = new CDockerWindow<CDockerUI,CWindowUI>;
		pFloatw->Init(GetManager(),pDocker,m_dwWinStyle,m_dwWinExStyle,m_winXml.GetData());
		if(!::IsWindow(pFloatw->GetHWND()))
			return;
		pFloatw->CenterWindow();
		pFloatw->ShowWindow();
	}
}

CRect CDockPanelUI::GetDockingArea()
{
	RECT rcItem = m_pTitleLayout->GetItemsLayout()->GetPos();
	::ClientToScreen(m_pManager->GetPaintWindow(), (LPPOINT)&rcItem.left);
	::ClientToScreen(m_pManager->GetPaintWindow(), (LPPOINT)&rcItem.right);
	return rcItem;
}

void CDockPanelUI::AcceptDocking(bool bAccept)
{
	if(!m_bAcceptDocking && bAccept)
	{
		m_bAcceptDocking = bAccept;
		CDockManager::SendInMessage();
		this->Invalidate();
	}
	if(m_bAcceptDocking && !bAccept)
	{
		m_bAcceptDocking = bAccept;
		CDockManager::SendLeaveMessage();
		this->Invalidate();
	}
}

void CDockPanelUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	CContainerUI::DoPaint(hDC,rcPaint);
	PaintDockingState(hDC);
}

void CDockPanelUI::PaintDockingState(HDC hDC)
{
	if(m_bAcceptDocking && m_dwBackColor)
	{
		Gdiplus::SolidBrush pBrush(Color(200, GetBValue(m_dwFloatColor), GetGValue(m_dwFloatColor), GetRValue(m_dwFloatColor))); // 透明度 128
		Gdiplus::Pen pPen(Color(100, 140, 180, 255),2.0f); // 透明度 128
		Graphics graphics(hDC);
		CRect rcItem = /*m_pDockerLayout->*/GetPos();
		graphics.FillRectangle(&pBrush, Gdiplus::Rect(rcItem.left,rcItem.top,rcItem.GetWidth(),rcItem.GetHeight())); 
		graphics.DrawRectangle(&pPen,Gdiplus::Rect(rcItem.left+4,rcItem.top+4,rcItem.GetWidth()-8,rcItem.GetHeight()-8));
		//CRenderEngine::DrawRect(hDC, m_rcItem, 100, GetAdjustColor(m_dwBackColor));
	}
}

void CDockPanelUI::SelectDocker(int nIndexOld,int nIndexNew)
{
	CDockerUI* pDocker = NULL;
	if(m_pDockerLayout==NULL) return;
	if(nIndexOld != -1)
		pDocker = static_cast<CDockerUI*>(m_pDockerLayout->GetItemAt(nIndexOld));
	if(pDocker)
		pDocker->SetVisible(false);
	pDocker = static_cast<CDockerUI*>(m_pDockerLayout->GetItemAt(nIndexNew));
	if(pDocker)
		pDocker->SetVisible(true);
}

void CDockPanelUI::SetDockersAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	int nCount = m_pDockerLayout->GetCount();
	for (size_t i = 0; i < nCount; i++)
	{
		m_pDockerLayout->GetItemAt((int)i)->SetAttribute(pstrName, pstrValue);
	}
}

CDockerUI* CDockPanelUI::GetCurrentDocker()
{
	CDockerUI* pDocker = NULL;
	if(m_pDockerLayout==NULL) return pDocker;
	int nCount = m_pDockerLayout->GetCount();
	for (size_t i = 0; i < nCount; i++)
	{

		if(m_pDockerLayout->GetItemAt((int)i)->IsVisible())
		{
			pDocker = static_cast<CDockerUI*>(m_pDockerLayout->GetItemAt((int)i));
			break;
		}
	}
	return pDocker;
}

void CDockPanelUI::SetWinStyle(DWORD dwStyle, DWORD dwExStyle)
{
	m_dwWinStyle = dwStyle;
	m_dwWinExStyle = dwExStyle;
}

std::pair<DWORD,DWORD> CDockPanelUI::GetWinStyle()
{
	return ::make_pair(m_dwWinStyle,m_dwWinExStyle);
}

/*************************CDockerUI***********************************/

CDockerUI::CDockerUI()
{
	m_pIDBcallback = NULL;
	m_pTitleItem = NULL;
	m_pParentDock = NULL;
	m_clearClose = false;
	m_useXml = false;
	m_index = 0;
	m_pParentWnd = NULL;
	m_dwStyleSub = 0;
	m_dwExStyleSub = 0;
	m_dwStylePlus = 0;
	m_dwExStylePlus = 0;
	m_bDisableMaxMin = FALSE;
}

CDockerUI::~CDockerUI()
{}

LPCTSTR CDockerUI::GetClass() const
{
	return _T("DockerUI");
}

LPVOID CDockerUI::GetInterface(LPCTSTR pstrName)
{
	if(_tcscmp(pstrName, DUI_CTR_DOCKER) == 0)
		return this;
	return __super::GetInterface(pstrName);
}

void CDockerUI::SetTitleItem(CDockTitleItemUI* pTitleItem)
{
	if(pTitleItem == NULL) return;
	m_pTitleItem = pTitleItem;
}

CDockTitleItemUI* CDockerUI::GetTitleItem( )
{
	return m_pTitleItem;
}

LPCTSTR CDockerUI::GetDockGroup() const
{
	return m_sDockGroup;
}

void CDockerUI::SetDockGroup(LPCTSTR pStrGroupName)
{
	m_sDockGroup.Assign(pStrGroupName);
}

void CDockerUI::SetClearClose(bool bClear)
{
	m_clearClose = bClear;
}

bool CDockerUI::GetClearClose()
{
	return m_clearClose;
}

void CDockerUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcscmp(pstrName, _T("xmlfile")) == 0) 
	{
		InitXml(pstrValue);
	}
	else if (_tcscmp(pstrName, _T("xmltilteimg")) == 0) 
	{
		m_tilteimgXml.Empty();
		m_tilteimgXml.Append(pstrValue);
	}
	else	if (_tcscmp(pstrName, _T("group")) == 0) 
	{
		SetDockGroup(pstrValue);
	}
	else if (_tcscmp(pstrName, _T("usexml")) == 0) 
	{
		if(_tcscmp(pstrValue, _T("true")) == 0)
		{
			m_useXml = true;
		}	
	}
	else if (_tcscmp(pstrName, _T("closeup")) == 0) 
	{
		SetClearClose(_tcscmp(pstrValue, _T("true")) == 0);
	}
	else if (_tcscmp(pstrName, _T("ignoremaxmin")) == 0) 
	{
		SetDisableMaxMin(_tcscmp(pstrValue, _T("true")) == 0);
	}
	else if (_tcscmp(pstrName, _T("addwinstyle")) == 0)
	{
		LPTSTR pstr = NULL;
		m_dwStylePlus= _tcstoul(pstrValue, &pstr, 16);
		m_dwExStylePlus= _tcstoul(pstr+1, &pstr, 16);
	}
	else if (_tcscmp(pstrName, _T("subwinstyle")) == 0)
	{
		LPTSTR pstr = NULL;
		m_dwStyleSub= _tcstoul(pstrValue, &pstr, 16);
		m_dwExStyleSub= _tcstoul(pstr+1, &pstr, 16);
	}
	__super::SetAttribute(pstrName, pstrValue);
}

void CDockerUI::SetBuilderCallback(IDialogBuilderCallback* pIDBcallback)
{
	m_pIDBcallback = pIDBcallback;
}

void CDockerUI::InitXml(LPCTSTR pstrXml)
{
	if(!m_useXml)  return;
	this->RemoveAll(); //clear first
	m_strXml = pstrXml;
	CDialogBuilder builder;
	CControlUI* pControl = NULL;
	pControl	= builder.Create(m_strXml.GetData(), NULL, m_pIDBcallback, this->GetManager());
	if(pControl)
	{
		this->Add(pControl);
	}
}

CUIString CDockerUI::GetXml()
{
	return m_strXml;
}

bool CDockerUI::ReInPanel(bool bClose)
{
	bool bChangeParent = false;
	CDockPanelUI* pDockPanel = CDockManager::GetDockingPanel();
	if(pDockPanel && m_pParentDock != pDockPanel)
	{
		bChangeParent = true;
		m_pParentDock = pDockPanel;
	}
	if(m_pParentDock)
	{
		if(!bClose){
			if(!m_pParentDock->GetAcceptDocking()) return false;
		}
		m_pParentDock->AcceptDocking(false);
		CContainerUI* pParent = (CContainerUI*)GetParent();
		std::pair<int,int> itemSize(m_pTitleItem->GetFixedWidth(),m_pTitleItem->GetFixedHeight());
		if(m_pParentDock->GetTitlePositionHor()) //hor
		{
			if(itemSize.first < itemSize.second) //ver
			{
				m_pTitleItem->SetFixedHeight(itemSize.first);
				m_pTitleItem->SetFixedWidth(itemSize.second);
			}
		}
		else
		{
			if(itemSize.first > itemSize.second) //hor
			{
				m_pTitleItem->SetFixedHeight(itemSize.first);
				m_pTitleItem->SetFixedWidth(itemSize.second);
			}
		}
		pParent->RemoveNotDestroy(this);
		if(bChangeParent)
		{
			m_index = 0;
			m_pParentDock->AddDockerAt(this,m_index);
		}
		else
			m_pParentDock->AddDockerAt(this,m_index,true);
		m_pParentDock->Invalidate();
		m_pTitleItem->Selected(true);
		m_pParentDock->SetVisible(true);
		m_pParentWnd = NULL;
		return true;
	}
	return false;
}

void CDockerUI::CloseClearUpDocker()
{
	if(!m_clearClose) return;
	this->RemoveAll();
	CContainerUI* pParent = (CContainerUI*)GetParent();
	pParent->RemoveNotDestroy(this);
	m_pTitleItem->RemoveAll();
	delete m_pTitleItem;
	m_pTitleItem = NULL;
	delete this;
}

void CDockerUI::DoEvent(TEventUI& event)
{
	//if (event.Type == UIEVENT_SETFOCUS)
	//{
	//	::OutputDebugString(_T("UIEVENT_SETFOCUS--"));
	//	::OutputDebugString(this->GetText().GetData());
	//	::OutputDebugString(_T("\n"));
	//}
	//if (event.Type == UIEVENT_KILLFOCUS)
	//{
	//	::OutputDebugString(_T("UIEVENT_KILLFOCUS--"));
	//	::OutputDebugString(this->GetText().GetData());
	//	::OutputDebugString(_T("\n"));
	//}
	//if (event.Type == UIEVENT_MOUSEHOVER)
	//{
	//	::OutputDebugString(_T("UIEVENT_MOUSEHOVER--"));
	//	::OutputDebugString(this->GetText().GetData());
	//	::OutputDebugString(_T("\n"));
	//}
	//if (event.Type == UIEVENT_BUTTONDOWN)
	//{
	//	::OutputDebugString(_T("UIEVENT_BUTTONDOWN--"));
	//	::OutputDebugString(this->GetText().GetData());
	//	::OutputDebugString(_T("\n"));
	//	return;
	//}
	//if (event.Type == UIEVENT_DBLCLICK)
	//{
	//	::OutputDebugString(_T("UIEVENT_DBLCLICK--"));
	//	::OutputDebugString(this->GetText().GetData());
	//	::OutputDebugString(_T("\n"));
	//	return;
	//}
	//if (event.Type == UIEVENT_MOUSEMOVE)
	//{
	//	return;
	//}
	//if (event.Type == UIEVENT_BUTTONUP)
	//{
	//	::OutputDebugString(_T("UIEVENT_BUTTONUP--"));
	//	::OutputDebugString(this->GetText().GetData());
	//	::OutputDebugString(_T("\n"));
	//	return;
	//}
	//if (event.Type == UIEVENT_CONTEXTMENU)
	//{
	//	if (IsContextMenuUsed()) {
	//		m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
	//	}
	//	return;
	//}
	//if (event.Type == UIEVENT_MOUSEENTER)
	//{
	//::OutputDebugString(_T("UIEVENT_MOUSEENTER--"));
	//	::OutputDebugString(this->GetText().GetData());
	//	::OutputDebugString(_T("\n"));
	//	// return;
	//}
	//if (event.Type == UIEVENT_MOUSELEAVE)
	//{
	//	::OutputDebugString(_T("UIEVENT_MOUSELEAVE--"));
	//	::OutputDebugString(this->GetText().GetData());
	//	::OutputDebugString(_T("\n"));
	//	// return;
	//}

	CContainerUI::DoEvent(event);
}

/************************************************************/