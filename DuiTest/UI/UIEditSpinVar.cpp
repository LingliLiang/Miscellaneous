#include "stdafx.h"
#include "UIEdit.h"
#include "UIEditSpinVar.h"

//#define  DUI_CTR_EDITSPIN                       (_T("EditSpinVar")) uidefine.h
//else if (_tcscmp(pstrClass, DUI_CTR_EDITSPIN) == 0)	  pControl = new CEditSpinVarUI; uidlgbuilder.cpp
//#include "Control/UIEditSpinVar.h" uilib.h

namespace DirectUI
{
	CEditSpinVarUI::CEditSpinVarUI()
		:m_dwDropColor(0xff3F3F3F),
		m_bEnableSpin(TRUE),m_uSpinAlign(AL_RIGHT),m_uSpinSlide(),
		m_uSpin1State(0),m_uSpin2State(0),m_varPrecise(1),m_varType(0),
		m_varStep(0.0),m_bAutoSpin(FALSE),m_bEnableMaxMin(FALSE)
	{
		SetBkColor(0xff000000);
		SetNativeBkColor(0xff262626);
		SetNativeEditBkColor(0xff262626);
		OnNotify += MakeDelegate(this, &CEditSpinVarUI::OnInnerNotify);
		::memset(&m_unVar,0,sizeof(uiVar));
		::memset(&m_unMinVar,0,sizeof(uiVar));
		::memset(&m_unMaxVar,0,sizeof(uiVar));
	}

	LPCTSTR CEditSpinVarUI::GetClass() const
	{
		return _T("EditSpinVarUI");
	}

	LPVOID CEditSpinVarUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_EDITSPIN) == 0)
			return static_cast<CEditSpinVarUI*>(this);

		return CEditUI::GetInterface(pstrName);
	}

	void CEditSpinVarUI::DoEvent(TEventUI& eventui)
	{
		if (!IsMouseEnabled() && eventui.Type > UIEVENT__MOUSEBEGIN && eventui.Type < UIEVENT__MOUSEEND) 
		{
			if (m_pParent != NULL) m_pParent->DoEvent(eventui);
			else CLabelUI::DoEvent(eventui);
			return;
		}
		if (eventui.Type == UIEVENT_SETCURSOR && IsEnabled())
		{
			if(::PtInRect(&m_rcSpin[0],eventui.ptMouse) || ::PtInRect(&m_rcSpin[1],eventui.ptMouse))
			{
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			}
			else 
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			return;
		}
		if (eventui.Type == UIEVENT_SCROLLWHEEL)
		{
			if (m_pWindow != NULL) {
				m_pWindow->Close();
			}
			if (m_pParent != NULL) m_pParent->DoEvent(eventui);
			return;
		}
		if( eventui.Type == UIEVENT_BUTTONDOWN || eventui.Type == UIEVENT_DBLCLICK )
		{
			if(IsEnabled())
			{
				BOOL bInSpin = FALSE;
				if( ::PtInRect(&m_rcSpin[0],eventui.ptMouse)) {
					m_uSpin1State |= UISTATE_PUSHED | UISTATE_CAPTURED;
					Invalidate();
					bInSpin = TRUE;
				}
				else if(::PtInRect(&m_rcSpin[1],eventui.ptMouse))
				{
					m_uSpin2State |= UISTATE_PUSHED | UISTATE_CAPTURED;
					Invalidate();
					bInSpin = TRUE;					
				}
				if(bInSpin){
					if (m_pWindow != NULL) {
						m_pWindow->Close();
					}
					return;
				} //bInSpin
			}
		}
		if( eventui.Type == UIEVENT_MOUSEMOVE )
		{
			BOOL bInSpin1 = ::PtInRect(&m_rcSpin[0], eventui.ptMouse);
			BOOL bInSpin2 = bInSpin1 ? FALSE : ::PtInRect(&m_rcSpin[1], eventui.ptMouse);

			if(bInSpin1 && !(m_uSpin1State & UISTATE_HOT)){
				m_uSpin1State |= UISTATE_HOT;
				Invalidate();
			}
			else if(!bInSpin1 && (m_uSpin1State & UISTATE_HOT))
			{
				m_uSpin1State &= ~UISTATE_HOT;
				Invalidate();
			}
			if(bInSpin2 && !(m_uSpin2State & UISTATE_HOT)){
				m_uSpin2State |= UISTATE_HOT;
				Invalidate();
			}
			else if(!bInSpin2 && (m_uSpin2State & UISTATE_HOT))
			{
				m_uSpin2State &= ~UISTATE_HOT;
				Invalidate();
			}

			if( m_uSpin1State & UISTATE_CAPTURED ) {
				if(bInSpin1) m_uSpin1State |= UISTATE_PUSHED;
				else m_uSpin1State &= ~UISTATE_PUSHED;
				Invalidate();
			}
			else if( m_uSpin2State & UISTATE_CAPTURED ) {
				if( bInSpin2) m_uSpin2State |= UISTATE_PUSHED;
				else m_uSpin2State &= ~UISTATE_PUSHED;
				Invalidate();
			}
			return;
		}
		if( eventui.Type == UIEVENT_BUTTONUP )
		{
			if( (m_uSpin1State & UISTATE_CAPTURED) != 0 ) {
				if( ::PtInRect(&m_rcSpin[0], eventui.ptMouse) ) ActiveSpin(0);
				m_uSpin1State &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
				Invalidate();
			}
			else if( (m_uSpin2State & UISTATE_CAPTURED) != 0 ) {
				if( ::PtInRect(&m_rcSpin[1], eventui.ptMouse) ) ActiveSpin(1);
				m_uSpin2State &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
				Invalidate();
			}
			return;
		}
		if( eventui.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				m_uSpin1State &= ~UISTATE_HOT;
				m_uSpin2State &= ~UISTATE_HOT;
			}
		}
		CEditUI::DoEvent(eventui);
	}

	bool CEditSpinVarUI::OnInnerNotify(LPVOID lpMsg)
	{
		TNotifyUI &Msg = *(static_cast<TNotifyUI*>(lpMsg));
		if(Msg.sType == DUI_MSGTYPE_TEXTCHANGED)
		{
			ChangedText();
		}
		return true;
	}

	void CEditSpinVarUI::ChangedText()
	{
		if(m_varType == 1)
		{
			int vInt = 0;
			auto var = _stscanf_s(m_sText.GetData(),_T("%d"), &vInt);
			if(var){
				m_unVar.varInt = vInt;
				if(m_bEnableMaxMin)
				{
					if(m_unVar.varInt < m_unMinVar.varInt) m_unVar.varInt = m_unMinVar.varInt;
					if(m_unVar.varInt > m_unMaxVar.varInt) m_unVar.varInt = m_unMaxVar.varInt;
				}
			}
			else
			{
				m_sText.Format(_T("%d"),m_unVar.varInt);
			}
		}
		else if(m_varType == 2)
		{
			double vDouble = 0;
			auto var = _stscanf_s(m_sText.GetData(),_T("%lf"), &vDouble);
			if(var){
				m_unVar.varDouble = vDouble;
				if(m_bEnableMaxMin)
				{
					if(m_unVar.varDouble < m_unMinVar.varDouble) m_unVar.varDouble = m_unMinVar.varDouble;
					if(m_unVar.varDouble > m_unMaxVar.varDouble) m_unVar.varDouble = m_unMaxVar.varDouble;
				}
			}
			else
			{
				TCHAR fmt[64] = {0};
				auto var2 = _stprintf_s(fmt,_T("%%.%dlf"),m_varPrecise);
				if(var2){
					m_sText.Format(fmt,m_unVar.varDouble);
				}
			}
		}
	}

	LPCTSTR CEditSpinVarUI::GetEmptyText()
	{
		return m_sEmptyText;
	}

	void CEditSpinVarUI::SetEmptyText(LPCTSTR pStrText)
	{
		m_sEmptyText = pStrText;
		Invalidate();
	}

	void CEditSpinVarUI::SetDropColor(DWORD dwTextColor)
	{
		m_dwDropColor = dwTextColor;
		Invalidate();
	}

	void CEditSpinVarUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("emptytext")) == 0 ) SetEmptyText(pstrValue);
		else if( _tcscmp(pstrName, _T("headtext")) == 0 ) SetHeadText(pstrValue);
		else if (_tcscmp(pstrName, _T("etdropcolor")) == 0)
		{
			if (*pstrValue == _T('#'))
				pstrValue = ::CharNext(pstrValue);

			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetDropColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("enablespin")) == 0 )
		{
			(_tcscmp(pstrValue, _T("true")) != 0) ? m_bEnableSpin = FALSE : m_bEnableSpin = TRUE;
		}
		else if( _tcscmp(pstrName, _T("enablemaxmin")) == 0 )
		{
			(_tcscmp(pstrValue, _T("true")) != 0) ? m_bEnableMaxMin = FALSE : m_bEnableMaxMin = TRUE;
		}
		else if( _tcscmp(pstrName, _T("spinslide")) == 0 ) 
		{
			LPTSTR pstr = NULL;
			m_uSpinSlide.cx = _tcstoul(pstrValue, &pstr, 10);
			m_uSpinSlide.cy = _tcstoul(pstr+1, &pstr, 10);
		}
		else if( _tcscmp(pstrName, _T("spinalign")) == 0 ) 
		{
			LPTSTR pstr = NULL;
			m_uSpinAlign = _tcstoul(pstrValue, &pstr, 10);
		}
		else if( _tcscmp(pstrName, _T("spin1normalimage")) == 0 ) SetSpin1NormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("spin1hotimage")) == 0 ) SetSpin1HotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("spin1pushedimage")) == 0 ) SetSpin1PushedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("spin1disabledimage")) == 0 ) SetSpin1DisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("spin2normalimage")) == 0 ) SetSpin2NormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("spin2hotimage")) == 0 ) SetSpin2HotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("spin2pushedimage")) == 0 ) SetSpin2PushedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("spin2disabledimage")) == 0 ) SetSpin2DisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("precise")) == 0 )
		{
			SetValueType(m_varType, _ttoi(pstrValue));
		}
		else if( _tcscmp(pstrName, _T("vtype")) == 0 )
		{
			SetValueType(_ttoi(pstrValue), m_varPrecise);
		}
		else if( _tcscmp(pstrName, _T("value")) == 0 )
		{
			if(m_varType == 1) SetValue(_ttoi(pstrValue));
			else if(m_varType == 2) SetValue(_tstof(pstrValue));			
		}
		else if( _tcscmp(pstrName, _T("vstep")) == 0 )
		{
			SetValueStep(_tstof(pstrValue));			
		}
		else if( _tcscmp(pstrName, _T("autospin")) == 0 )
		{
			(_tcscmp(pstrValue, _T("true")) != 0) ? m_bAutoSpin = FALSE : m_bAutoSpin = TRUE;		
		}
		else if( _tcscmp(pstrName, _T("vmaxmin")) == 0 ) 
		{
			TCHAR strValue[128] = {0};
			::memcpy_s(strValue,sizeof(strValue),pstrValue,_tcslen(pstrValue)*sizeof(TCHAR));
			int pos = 0;
			while(true)
			{
				if(!strValue[pos])
				{
					pos = -1;
					break;
				}
				if(strValue[pos] == _T(','))
				{
					break;
				}
				pos++;
			}
			if(pos != -1)
			{
				strValue[pos] = _T('\0');
				LPCTSTR pStr = &strValue[0];
				LPCTSTR pStr1 = &strValue[pos+1];
				if(m_varType == 1){
					SetMaxValue(_ttoi(pStr));
					SetMinValue(_ttoi(pStr1));
				}
				else if(m_varType == 2){
					SetMaxValue(_tstof(pStr));
					SetMinValue(_tstof(pStr1));
				}
			}
		}
		else CEditUI::SetAttribute(pstrName, pstrValue);
	}

	void CEditSpinVarUI::SetPos(RECT rc)
	{
		//not need call surper SetPos()
		CControlUI::SetPos(rc);
		if( m_pWindow != NULL ) {
			RECT rcPos = m_pWindow->CalPos();
			::SetWindowPos(m_pWindow->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, 
				rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);        
		}
		CalcSpinPos();
	}

	void CEditSpinVarUI::SetText(LPCTSTR pstrText)
	{
		m_sText = pstrText;
		ChangedText();
		if( m_pWindow != NULL ) Edit_SetText(*m_pWindow, m_sText);
		Invalidate();
	}

	void CEditSpinVarUI::CalcSpinPos()
	{
		CRect rcItem = m_rcItem;
		rcItem.Inflate(&m_rcPadding);
		auto subHeight = (rcItem.GetHeight() - m_uSpinSlide.cy*2)/2;
		m_rcSpin[0] = CRect(0,0,m_uSpinSlide.cx,m_uSpinSlide.cy);
		m_rcSpin[1] = CRect(0,0,m_uSpinSlide.cx,m_uSpinSlide.cy);
		if(m_uSpinAlign == AL_LEFT)
		{
			m_rcSpin[0].MoveToXY(m_rcItem.left,m_rcItem.top + subHeight);
			m_rcSpin[1].MoveToXY(m_rcItem.left,m_rcItem.top + subHeight+m_uSpinSlide.cy);
			if(m_rcTextPadding.left < m_uSpinSlide.cx)
			{
				m_rcTextPadding.left += m_uSpinSlide.cx;
			}
		}
		else if(m_uSpinAlign == AL_RIGHT)
		{
			m_rcSpin[0].MoveToXY(m_rcItem.right - m_uSpinSlide.cx,m_rcItem.top + subHeight);
			m_rcSpin[1].MoveToXY(m_rcItem.right - m_uSpinSlide.cx,m_rcItem.top + subHeight+m_uSpinSlide.cy);
			if(m_rcTextPadding.right < m_uSpinSlide.cx)
			{
				m_rcTextPadding.right += m_uSpinSlide.cx;
			}
		}
		else if(m_uSpinAlign == AL_SPLIT)
		{
			m_rcSpin[0].MoveToXY(m_rcItem.left,m_rcItem.top + subHeight);
			m_rcSpin[1].MoveToXY(m_rcItem.right - m_uSpinSlide.cx,m_rcItem.top + subHeight+m_uSpinSlide.cy);
			if(m_rcTextPadding.left < m_uSpinSlide.cx)
			{
				m_rcTextPadding.left += m_uSpinSlide.cx;
			}
			if(m_rcTextPadding.right < m_uSpinSlide.cx)
			{
				m_rcTextPadding.right += m_uSpinSlide.cx;
			}
		}
	}

	void CEditSpinVarUI::PaintStatusImage(HDC hDC)
	{
		CEditUI::PaintStatusImage(hDC);
		if(!m_bEnableSpin) return;
		if( !IsEnabled() ){
			m_uSpin1State |= UISTATE_DISABLED;
			m_uSpin2State |= UISTATE_DISABLED;
		}
		else{
			m_uSpin1State &= ~UISTATE_DISABLED;
			m_uSpin2State &= ~UISTATE_DISABLED;
		}
		PaintSpin(hDC,m_uSpin1State,m_sSpin1Image,m_rcSpin[0]);
		PaintSpin(hDC,m_uSpin2State,m_sSpin2Image,m_rcSpin[1]);
	}
	
	void CEditSpinVarUI::PaintImg(HDC& hDC, CUIString &strImg, RECT* rectPaint /*= NULL*/)
	{
		bool sucss = false;
		if(strImg.IsEmpty()) return;
		if(rectPaint)
		{
			sucss = CRenderEngine::DrawImageString(hDC, m_pManager,*rectPaint, *rectPaint, strImg.GetData(), NULL);
		}
		else
		{
			sucss = CRenderEngine::DrawImageString(hDC, m_pManager, m_rcItem, m_rcPaint, strImg.GetData(), NULL);
		}
		if(!sucss) strImg.Empty();
	}

	void CEditSpinVarUI::PaintSpin(HDC& hDC, UINT state, CUIString (&spinImg)[4], RECT rcPaint)
	{
		if(state & UISTATE_DISABLED)
		{
			PaintImg(hDC, spinImg[ID_DIS], &rcPaint);
		}
		else if(state & UISTATE_PUSHED)
		{
			PaintImg(hDC, spinImg[ID_PUSH], &rcPaint);
		}
		else if(state & UISTATE_HOT)
		{
			PaintImg(hDC, spinImg[ID_HOT], &rcPaint);
		}
		else
		{
			PaintImg(hDC, spinImg[ID_NOR], &rcPaint);
		}
	}

	void CEditSpinVarUI::PaintText(HDC hDC)
	{
		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		CUIString sText = m_sText;
		bool emptyText = m_sText.IsEmpty();

		if (emptyText && !m_sEmptyText.IsEmpty())
		{
			sText = m_sEmptyText;
		}

		if( m_bPasswordMode ) {
			if (!emptyText){
				sText.Empty();
				LPCTSTR p = m_sText.GetData();
				while( *p != _T('\0') ) {
					sText += m_cPasswordChar;
					p = ::CharNext(p);
				}
			}
		}

		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		if(!m_sHeadText.IsEmpty())
		{
			RECT rcHead = rc;
			SIZE szHead = CRenderEngine::GetTextSize(hDC,m_pManager,m_sHeadText.GetData(),m_iFont,DT_VCENTER | DT_LEFT);
			rc.left += szHead.cx;
			rcHead.right += szHead.cx;
			if( IsEnabled() ) {
				CRenderEngine::DrawText(hDC, m_pManager, rcHead, m_sHeadText, emptyText ? m_dwTextColor-m_dwDropColor : m_dwTextColor, \
					m_iFont, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
			}
			else {
				CRenderEngine::DrawText(hDC, m_pManager, rcHead, m_sHeadText, emptyText ? m_dwTextColor - m_dwDropColor : m_dwDisabledTextColor, \
					m_iFont, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
			}
		}

		if(m_varType == 1)
		{
			sText.Format(_T("%d"),m_unVar.varInt);
			m_sText = sText;
		}
		else if(m_varType == 2)
		{
			TCHAR fmt[64] = {0};
			auto var = _stprintf_s(fmt,_T("%%.%dlf"),m_varPrecise);
			if(var){
				sText.Format(fmt,m_unVar.varDouble);
				m_sText = sText;
			}
		}

		if( IsEnabled() ) {
			CRenderEngine::DrawText(hDC, m_pManager, rc, sText, emptyText ? m_dwTextColor-m_dwDropColor : m_dwTextColor, \
				m_iFont, DT_SINGLELINE | m_uTextStyle);
		}
		else {
			CRenderEngine::DrawText(hDC, m_pManager, rc, sText, emptyText ? m_dwTextColor - m_dwDropColor : m_dwDisabledTextColor, \
				m_iFont, DT_SINGLELINE | m_uTextStyle);
		}
	}

	void CEditSpinVarUI::ActiveSpin(int index)
	{
		if(m_bAutoSpin)
		{	
			if(m_varType == 1)
			{
				index ? m_unVar.varInt -= (int)m_varStep : m_unVar.varInt += (int)m_varStep;
				if(m_bEnableMaxMin)
				{
					if(m_unVar.varInt < m_unMinVar.varInt) m_unVar.varInt = m_unMinVar.varInt;
					if(m_unVar.varInt > m_unMaxVar.varInt) m_unVar.varInt = m_unMaxVar.varInt;
				}
			}
			else if(m_varType == 2)
			{
				index ? m_unVar.varDouble -= m_varStep : m_unVar.varDouble += m_varStep;
				if(m_bEnableMaxMin)
				{
					if(m_unVar.varDouble < m_unMinVar.varDouble) m_unVar.varDouble = m_unMinVar.varDouble;
					if(m_unVar.varDouble > m_unMaxVar.varDouble) m_unVar.varDouble = m_unMaxVar.varDouble;
				}
			}
		}
		if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK, index, m_bAutoSpin);
	}

	LPCTSTR CEditSpinVarUI::GetSpin1NormalImage()
	{
		return m_sSpin1Image[ID_NOR];
	}
	void CEditSpinVarUI::SetSpin1NormalImage(LPCTSTR pStrImage)
	{
		m_sSpin1Image[ID_NOR] = pStrImage;
	}
	LPCTSTR CEditSpinVarUI::GetSpin1HotImage()
	{
		return m_sSpin1Image[ID_HOT];
	}
	void CEditSpinVarUI::SetSpin1HotImage(LPCTSTR pStrImage)
	{
		m_sSpin1Image[ID_HOT] = pStrImage;
	}
	LPCTSTR CEditSpinVarUI::GetSpin1PushedImage()
	{
		return m_sSpin1Image[ID_PUSH];
	}
	void CEditSpinVarUI::SetSpin1PushedImage(LPCTSTR pStrImage)
	{
		m_sSpin1Image[ID_PUSH] = pStrImage;
	}
	LPCTSTR CEditSpinVarUI::GetSpin1DisabledImage()
	{
		return m_sSpin1Image[ID_DIS];
	}
	void CEditSpinVarUI::SetSpin1DisabledImage(LPCTSTR pStrImage)
	{
		m_sSpin1Image[ID_DIS] = pStrImage;
	}

	LPCTSTR CEditSpinVarUI::GetSpin2NormalImage()
	{
		return m_sSpin2Image[ID_NOR];
	}
	void CEditSpinVarUI::SetSpin2NormalImage(LPCTSTR pStrImage)
	{
		m_sSpin2Image[ID_NOR] = pStrImage;
	}
	LPCTSTR CEditSpinVarUI::GetSpin2HotImage()
	{
		return m_sSpin2Image[ID_HOT];
	}
	void CEditSpinVarUI::SetSpin2HotImage(LPCTSTR pStrImage)
	{
		m_sSpin2Image[ID_HOT] = pStrImage;
	}
	LPCTSTR CEditSpinVarUI::GetSpin2PushedImage()
	{
		return m_sSpin2Image[ID_PUSH];
	}
	void CEditSpinVarUI::SetSpin2PushedImage(LPCTSTR pStrImage)
	{
		m_sSpin2Image[ID_PUSH] = pStrImage;
	}
	LPCTSTR CEditSpinVarUI::GetSpin2DisabledImage()
	{
		return m_sSpin2Image[ID_DIS];
	}
	void CEditSpinVarUI::SetSpin2DisabledImage(LPCTSTR pStrImage)
	{
		m_sSpin2Image[ID_DIS] = pStrImage;
	}

	void CEditSpinVarUI::SetHeadText(LPCTSTR pStrFormat)
	{
		m_sHeadText = pStrFormat;
	}
	LPCTSTR CEditSpinVarUI::GetHeadText()
	{
		return m_sHeadText;
	}

	void CEditSpinVarUI::SetValueType(unsigned int type, unsigned short precise /*= 1*/)
	{
		if(type < 3){
			m_varType = type;
			m_varPrecise = precise;
		}
	}
	unsigned int CEditSpinVarUI::GetValueType() const
	{
		return m_varType;
	}
	bool CEditSpinVarUI::SetValue(int value)
	{
		if(m_varType == 1)
		{
			m_unVar.varInt = value;
			if(m_bEnableMaxMin)
			{
				if(m_unVar.varInt < m_unMinVar.varInt) m_unVar.varInt = m_unMinVar.varInt;
				if(m_unVar.varInt > m_unMaxVar.varInt) m_unVar.varInt = m_unMaxVar.varInt;
			}
			Invalidate();
			return true;
		}
		return false;
	}
	bool CEditSpinVarUI::SetValue(double value)
	{
		if(m_varType == 2)
		{
			m_unVar.varDouble = value;
			if(m_bEnableMaxMin)
			{
				if(m_unVar.varDouble < m_unMinVar.varDouble) m_unVar.varDouble = m_unMinVar.varDouble;
				if(m_unVar.varDouble > m_unMaxVar.varDouble) m_unVar.varDouble = m_unMaxVar.varDouble;
			}
			Invalidate();
			return true;
		}
		return false;
	}
	int CEditSpinVarUI::GetValue() const
	{
		if(m_varType == 1)
		{
			return m_unVar.varInt;
		}
		return 0;
	}
	double CEditSpinVarUI::GetValue(double) const
	{
		if(m_varType == 2)
		{
			return m_unVar.varDouble;
		}
		return 0.0;
	}
	void CEditSpinVarUI::SetValueStep(double value)
	{
		m_varStep = value;
	}
	bool CEditSpinVarUI::SetMaxValue(int value)
	{
		if(m_varType == 1)
		{
			m_unMaxVar.varInt = value;
			return true;
		}
		return false;
	}
	bool CEditSpinVarUI::SetMaxValue(double value)
	{
		if(m_varType == 2)
		{
			m_unMaxVar.varDouble = value;
			return true;
		}
		return false;
	}
	int CEditSpinVarUI::GetMaxValue() const
	{
		if(m_varType == 1)
		{
			return m_unMaxVar.varInt;
		}
		return 0;
	}
	double CEditSpinVarUI::GetMaxValue(double) const
	{
		if(m_varType == 2)
		{
			return m_unMaxVar.varDouble;
		}
		return 0.0;
	}
	bool CEditSpinVarUI::SetMinValue(int value)
	{
		if(m_varType == 1)
		{
			m_unMinVar.varInt = value;
			return true;
		}
		return false;
	}
	bool CEditSpinVarUI::SetMinValue(double value)
	{
		if(m_varType == 2)
		{
			m_unMinVar.varDouble = value;
			return true;
		}
		return false;
	}
	int CEditSpinVarUI::GetMinValue() const
	{
		if(m_varType == 1)
		{
			return m_unMinVar.varInt;
		}
		return 0;
	}
	double CEditSpinVarUI::GetMinValue(double) const
	{
		if(m_varType == 2)
		{
			return m_unMinVar.varDouble;
		}
		return 0.0;
	}


} //namespace DirectUI
