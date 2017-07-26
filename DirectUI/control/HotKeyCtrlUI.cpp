#include "StdAfx.h"
#include "HotKeyCtrlUI.h"

using namespace DirectUI;
CHotKeyCtrlUI::CHotKeyCtrlUI(void):m_dwKeyCode(0)
{
}


CHotKeyCtrlUI::~CHotKeyCtrlUI(void)
{
}

LPCTSTR CHotKeyCtrlUI::GetClass() const
{
	return _T("HotKeyUI");
}

LPVOID CHotKeyCtrlUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, DUI_CTR_HOTKEY) == 0 ) return static_cast<CHotKeyCtrlUI*>(this);
	return CLabelUI::GetInterface(pstrName);
}

#define KEY_SHIFT 0x01
#define KEY_CTRL 0x02
#define KEY_ALT 0x04
void CHotKeyCtrlUI::DoEvent(TEventUI& event)
{
	if( IsEnabled() ) {
		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			Invalidate();
		}
		if (event.Type==UIEVENT_KEYDOWN||event.Type==UIEVENT_SYSKEYDOWN) {
			WORD wHigh=0;
			bool bGet=true;
			if (event.wKeyState&MK_CONTROL) {
				wHigh|=KEY_CTRL;
			}
			if (event.wKeyState&MK_SHIFT) {
				wHigh|=KEY_SHIFT;
			}
			if (event.wKeyState&MK_ALT) {
				wHigh|=KEY_ALT;
			}
			if (event.chKey!=VK_SHIFT&&event.chKey!=VK_CONTROL&&event.chKey!=VK_MENU) {
				bGet=false;
			}
			WORD wLow=LOWORD(m_dwKeyCode);
			m_dwKeyCode=MAKELONG(bGet?wLow:event.chKey,wHigh);
			ParserCode(m_dwKeyCode);
		} else if (event.Type==UIEVENT_KEYUP||event.Type==UIEVENT_KILLFOCUS||event.Type==UIEVENT_SYSKEYUP) {
			WORD wHigh=HIWORD(m_dwKeyCode);
			WORD wLow=LOWORD(m_dwKeyCode);
			if (wLow==0)	{
				wHigh=0;
				bool bGet=true;
				if (event.wKeyState&MK_CONTROL) {
					wHigh|=KEY_CTRL;
				}
				if (event.wKeyState&MK_SHIFT) {
					wHigh|=KEY_SHIFT;
				}
				if (event.wKeyState&MK_ALT) {
					wHigh|=KEY_ALT;
				}
				m_dwKeyCode=MAKELONG(wLow,wHigh);
			//bool bGet=true;
			//if (event.chKey==VK_SHIFT) {
			//	wHigh &=~KEY_SHIFT;
			//} else if(event.chKey==VK_CONTROL){
			//	wHigh &=~KEY_CTRL;
			//}else if (event.chKey==VK_MENU) {
			//	wHigh&=~KEY_ALT;
			//}
			//m_dwKeyCode=MAKELONG(event.chKey,wHigh);
				ParserCode(m_dwKeyCode);
			}
		}
	}
	CLabelUI::DoEvent(event);
}


void CHotKeyCtrlUI::SetKeyCode(DWORD dwKeyCode)
{
	m_dwKeyCode=dwKeyCode;
	ParserCode(dwKeyCode);
}

void CHotKeyCtrlUI::ParserCode(DWORD dwKey)
{
	WORD wLow=LOWORD(dwKey);
	WORD wHigh=HIWORD(dwKey);
	CUIString strText;
	if (wHigh&KEY_SHIFT) {
		strText=_T("Shift+");
	}
	if (wHigh&KEY_CTRL) {
		strText +=_T("Ctrl+");
	}
	if (wHigh&KEY_ALT) {
		strText +=_T("Alt+");
	}

	TCHAR chOut[100];
	memset(chOut,0,sizeof(TCHAR)*100);
	unsigned int scanCode = MapVirtualKey(wLow, MAPVK_VK_TO_VSC);

    // because MapVirtualKey strips the extended bit for some keys
    switch (wLow)
    {
        case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN: // arrow keys
        case VK_PRIOR: case VK_NEXT: // page up and page down
        case VK_END: case VK_HOME:
        case VK_INSERT: case VK_DELETE:
        case VK_DIVIDE: // numpad slash
        case VK_NUMLOCK:
        {
            scanCode |= 0x100; // set extended bit
            break;
        }
		case VK_PAUSE:
			scanCode =0x45;
			break;
    }
	GetKeyNameText(scanCode<<16, chOut, sizeof(TCHAR)*100);  
	//GetKeyNameText(lParam, chOut,100*sizeof(TCHAR));
	strText +=chOut;
	SetText(strText);
	SetToolTip(strText);
	//if (wLow==0) {
	//} else {
	//	switch (wLow)
	//	{
	//	case VK_CAPITAL:
	//		strText+=_T("Caps Lock");
	//		break;
	//	case VK_ESCAPE:
	//		strText+=_T("Esc");
	//		break;
	//	case VK_SPACE:
	//		strText+=_T("Space");
	//		break;
	//	case VK_PRIOR:
	//		strText+=_T("Page Up");
	//		break;
	//	case VK_NEXT:
	//		strText+=_T("Page Down");
	//		break;
	//	case VK_END:
	//		strText+=_T("End");
	//		break;
	//	case VK_HOME:
	//		strText+=_T("Home");
	//		break;
	//	case VK_LEFT:
	//		strText+=_T("Left");
	//		break;
	//	case VK_UP:
	//		strText+=_T("Up");
	//		break;
	//	case VK_RIGHT:
	//		strText+=_T("Right");
	//		break;
	//	case VK_DOWN:
	//		strText+=_T("Down");
	//		break;
	//	}
	//}
}

void CHotKeyCtrlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
		if( _tcscmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
		else
			CLabelUI::SetAttribute(pstrName,pstrValue);
}

void CHotKeyCtrlUI::SetFocusedImage(LPCTSTR szImg)
{
	m_sFocusedImage=szImg;
}

void CHotKeyCtrlUI::PaintStatusImage(HDC hDC)
{
	if( IsFocused() ) {
		if( !m_sFocusedImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sFocusedImage) ) m_sFocusedImage.Empty();
			else goto Label_ForeImage;
		}
	} else {

		if( !this->m_sBkImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sBkImage) ) m_sBkImage.Empty();
			else goto Label_ForeImage;
		}

		if(!m_sForeImage.IsEmpty() )
			goto Label_ForeImage;

		return;

Label_ForeImage:
		if(!m_sForeImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sForeImage) ) m_sForeImage.Empty();
		}
	}
}