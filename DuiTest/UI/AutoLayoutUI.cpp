#include "stdafx.h"
#include "AutoLayoutUI.h"


const TCHAR*  DUI_CTR_AUTOLAYOUT = _T("AutoLayout");

namespace DirectUI
{

	void CImagePaint::SetSelImage(LPCTSTR pstrImage)
	{
		m_strSelImg = pstrImage;
	}

	CUIString CImagePaint::GetSelImage()
	{
		return m_strSelImg;
	}

	void CImagePaint::SetHotImage(LPCTSTR pstrImage)
	{
		m_strHotImg = pstrImage;
	}

	CUIString CImagePaint::GetHotImage()
	{
		return m_strHotImg;
	}

	void CImagePaint::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("selimage")) == 0 )
			SetSelImage(pstrValue);
		else if( _tcscmp(pstrName, _T("hotimage")) == 0 )
			SetHotImage(pstrValue);
	}

	void CImagePaint::DoImgPaint(HDC hDC, const RECT& rcPaint)
	{
		//if(m_strSelImg.IsEmpty() || m_pSelControl == NULL) return;
		//RECT rcItem = m_pSelControl->GetPos();
		//if (!::IntersectRect(&m_rcPaint, &rcPaint, &rcItem)) return;

		//if (!DrawImage(hDC, rcItem, (LPCTSTR)m_strSelImg))
		//	m_strSelImg.Empty();
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//CAutoLayoutUI

	CAutoLayoutUI::CAutoLayoutUI() : m_bMode(Vertical), m_uButtonState(0), m_bImmMode(false)	,m_pSelControl(NULL),m_pHotControl(NULL)
	{
		ptLastMouse.x = ptLastMouse.y = 0;
		::ZeroMemory(&m_rcNewPos, sizeof(m_rcNewPos));
	}

	LPCTSTR CAutoLayoutUI::GetClass() const
	{
		return _T("LayerListUI");
	}

	LPVOID CAutoLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		//if( _tcscmp(pstrName, DUI_CTR_LAYERLIST) == 0 ) return static_cast<CAutoLayoutUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	void CAutoLayoutUI::SetMode(size_t nLayoutMode)
	{
		m_bMode = nLayoutMode;
	}

	size_t CAutoLayoutUI::GetMode() const
	{
		return m_bMode;
	}

	void CAutoLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		CContainerUI::SetAttribute(pstrName, pstrValue);
		if( _tcscmp(pstrName, _T("sepwidth")) == 0 ) _ttoi(pstrValue);
		if( _tcscmp(pstrName, _T("hscrollbar")) == 0 && _tcscmp(pstrValue, _T("true")) == 0)
		{
			//m_pHorizontalScrollBar->SetVisible(false);
		}
		else if( _tcscmp(pstrName, _T("vscrollbar")) == 0 && _tcscmp(pstrValue, _T("true")) == 0)
		{
			//m_pVerticalScrollBar->SetVisible(false);
		}
		else
		{
			CUIString strName = pstrName;
			if( _tcscmp(strName.Left(4).GetData(), _T("item")) == 0 )
			{
				strName = strName.Right(strName.GetLength() - 4);
				SetItemAttribute(strName.GetData(),pstrValue);
			}
		}
	}

	void CAutoLayoutUI::SetItemAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		int nCount = GetCount();
		for(int index = 0; index < nCount; index++)
		{
			CControlUI* pCtl = this->GetItemAt(index);
			if(pCtl)
			{
				pCtl->SetAttribute(pstrName,pstrValue);
			}
		}
	}

	void CAutoLayoutUI::DoEvent(TEventUI& event)
	{
		if (event.Type == UIEVENT_BUTTONDOWN && IsEnabled())
		{
			int index = 0;
			if(PtHitControl(event.ptMouse,index) != NULL)
			{
				SelectItem(index);
			}
		}
		if (event.Type == UIEVENT_WINDOWSIZE)
		{
			//EnsureVisible(m_iCurSel);
		}
		else if (event.Type == UIEVENT_MOUSEMOVE)
		{
			int index = 0;
			CControlUI* pControl = PtHitControl(event.ptMouse,index);
			if(pControl && pControl != m_pSelControl)
			{
				if(m_pHotControl) m_pHotControl->Invalidate();
				m_pHotControl = pControl;
				if(m_pHotControl) m_pHotControl->Invalidate();
			}
			else
			{
				if(m_pHotControl) m_pHotControl->Invalidate();
				m_pHotControl = NULL;
			}
		}
		else if (event.Type == UIEVENT_MOUSELEAVE)
		{
			m_pHotControl = NULL;
			this->Invalidate();
		}
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
		{
			if (m_pParent != NULL) m_pParent->DoEvent(event);
			else CControlUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_SETFOCUS)
		{
			m_bFocused = true;
			return;
		}
		if (event.Type == UIEVENT_KILLFOCUS)
		{
			m_bFocused = false;
			return;
		}
		if (m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsEnabled())
		{
			if (event.Type == UIEVENT_KEYDOWN)
			{
				switch (event.chKey)
				{
				case VK_DOWN:
					LineDown();
					return;
				case VK_UP:
					LineUp();
					return;
				case VK_NEXT:
					PageDown();
					return;
				case VK_PRIOR:
					PageUp();
					return;
				case VK_HOME:
					HomeUp();
					return;
				case VK_END:
					EndDown();
					return;
				}
			}
			else if (event.Type == UIEVENT_SCROLLWHEEL)
			{
				switch (LOWORD(event.wParam))
				{
				case SB_LINEUP:
					LineUp();
					return;
				case SB_LINEDOWN:
					LineDown();
					return;
				}
			}
		}
		else if (m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsEnabled())
		{
			if (event.Type == UIEVENT_KEYDOWN)
			{
				switch (event.chKey)
				{
				case VK_DOWN:
					LineRight();
					return;
				case VK_UP:
					LineLeft();
					return;
				case VK_NEXT:
					PageRight();
					return;
				case VK_PRIOR:
					PageLeft();
					return;
				case VK_HOME:
					HomeLeft();
					return;
				case VK_END:
					EndRight();
					return;
				}
			}
			else if (event.Type == UIEVENT_SCROLLWHEEL)
			{
				switch (LOWORD(event.wParam))
				{
				case SB_LINEUP:
					LineLeft();
					return;
				case SB_LINEDOWN:
					LineRight();
					return;
				}
			}
		}

		CControlUI::DoEvent(event);
	}

	void CAutoLayoutUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		RECT rcTemp = { 0 };
		if (!::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) return;

		CRenderClip clip;
		CRenderClip::GenerateClip(hDC, rcTemp, clip);
		CControlUI::DoPaint(hDC, rcPaint);

		if (m_items.GetSize() > 0)
		{
			RECT rc = m_rcItem;
			rc.left += m_rcInset.left;
			rc.top += m_rcInset.top;
			rc.right -= m_rcInset.right;
			rc.bottom -= m_rcInset.bottom;

			if (!::IntersectRect(&rcTemp, &rcPaint, &rc))
			{
				for (int it = 0; it < m_items.GetSize(); it++)
				{
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					if (!pControl->IsVisible()) continue;
					if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos())) continue;
					if (pControl->IsFloat())
					{
						if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos())) continue;
						pControl->DoPaint(hDC, rcPaint);
					}
				}
			}
			else
			{
				CRenderClip childClip;
				CRenderClip::GenerateClip(hDC, rcTemp, childClip);
				for (int it = 0; it < m_items.GetSize(); it++)
				{
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					if (!pControl->IsVisible()) continue;
					if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos())) continue;
					if (pControl->IsFloat())
					{
						if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos())) continue;
						CRenderClip::UseOldClipBegin(hDC, childClip);
						pControl->DoPaint(hDC, rcPaint);
						CRenderClip::UseOldClipEnd(hDC, childClip);
					}
					else
					{
						if (!::IntersectRect(&rcTemp, &rc, &pControl->GetPos())) continue;
						pControl->DoPaint(hDC, rcPaint);
					}
				}
			}
		}
		if (m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible())
		{
			if (::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()))
				m_pVerticalScrollBar->DoPaint(hDC, rcPaint);
		}

		if (m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible())
		{
			if (::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()))
				m_pHorizontalScrollBar->DoPaint(hDC, rcPaint);
		}
	}

	bool CAutoLayoutUI::DrawImage(HDC hDC, const RECT &rcItem, LPCTSTR pStrImage, LPCTSTR pStrModify)
	{
		return CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, pStrImage, pStrModify);
	}

	void CAutoLayoutUI::SetPos(RECT rc)
	{
		if(CheckRectInvalid(rc)) return;
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
		bool verticalLayout = true;
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

	void CAutoLayoutUI::ProcessScrollBar(RECT rc, int cxRequired, int cyRequired)
	{
		if(CheckRectInvalid(rc)) return;
		if (m_pHorizontalScrollBar != NULL)
		{
			RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight() };
			m_pHorizontalScrollBar->SetScrollRange(cxRequired - (rc.right - rc.left));
			m_pHorizontalScrollBar->SetPos(rcScrollBarPos);
		}

		if (m_pVerticalScrollBar == NULL) return;

		if (cyRequired > rc.bottom - rc.top)
		{
			m_pVerticalScrollBar->SetScrollRange(cyRequired - (rc.bottom - rc.top));
			m_pVerticalScrollBar->SetScrollPos(0);
			m_bScrollProcess = true;
			//SetPos(m_rcItem);
			m_bScrollProcess = false;
			return;
		}
		// No scrollbar required
		if (!m_pVerticalScrollBar->IsVisible()) return;

		// Scroll not needed anymore?
		int cyScroll = cyRequired - (rc.bottom - rc.top);
		if (cyScroll <= 0 && !m_bScrollProcess)
		{
			m_pVerticalScrollBar->SetVisible(false);
			m_pVerticalScrollBar->SetScrollPos(0);
			m_pVerticalScrollBar->SetScrollRange(0);
			SetPos(m_rcItem);
		}
		else
		{
			RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->GetFixedWidth(), rc.bottom };
			m_pVerticalScrollBar->SetPos(rcScrollBarPos);

			if (m_pVerticalScrollBar->GetScrollRange() != cyScroll)
			{
				int iScrollPos = m_pVerticalScrollBar->GetScrollPos();
				m_pVerticalScrollBar->SetScrollRange(::abs(cyScroll));
				if (m_pVerticalScrollBar->GetScrollRange() == 0)
				{
					m_pVerticalScrollBar->SetVisible(false);
					m_pVerticalScrollBar->SetScrollPos(0);
				}
				if (iScrollPos > m_pVerticalScrollBar->GetScrollPos())
					SetPos(m_rcItem);
			}
		}
	}

	SIZE CAutoLayoutUI::GetScrollPos() const
	{
		SIZE sz = {0, 0};
		if (m_pVerticalScrollBar) sz.cy = m_pVerticalScrollBar->GetScrollPos();
		if (m_pHorizontalScrollBar) sz.cx = m_pHorizontalScrollBar->GetScrollPos();
		return sz;
	}

	SIZE CAutoLayoutUI::GetScrollRange() const
	{
		SIZE sz = {0, 0};
		if( m_pVerticalScrollBar) sz.cy = m_pVerticalScrollBar->GetScrollRange();
		if( m_pHorizontalScrollBar) sz.cx = m_pHorizontalScrollBar->GetScrollRange();
		return sz;
	}

	void CAutoLayoutUI::SetScrollPos(SIZE szPos)
	{
		int cx = 0;
		int cy = 0;
		if (m_pVerticalScrollBar) 
		{
			int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
			m_pVerticalScrollBar->SetScrollPos(szPos.cy);
			cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if (m_pHorizontalScrollBar)
		{
			int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
			m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
			cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if (cx == 0 && cy == 0) return;

		RECT rcPos;
		for (int it2 = 0; it2 < m_items.GetSize(); it2++)
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if (!pControl->IsVisible()) continue;
			if (pControl->IsFloat()) continue;

			rcPos = pControl->GetPos();
			rcPos.left -= cx;
			rcPos.right -= cx;
			rcPos.top -= cy;
			rcPos.bottom -= cy;
			pControl->SetPos(rcPos);
		}

		Invalidate();
	}

	void CAutoLayoutUI::PageUp()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
		sz.cy -= iOffset;
		SetScrollPos(sz);
	}

	void CAutoLayoutUI::PageDown()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
		sz.cy += iOffset;
		SetScrollPos(sz);
	}

	void CAutoLayoutUI::PageLeft()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
		sz.cx -= iOffset;
		SetScrollPos(sz);
	}

	void CAutoLayoutUI::PageRight()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
		sz.cx += iOffset;
		SetScrollPos(sz);
	}

	int CAutoLayoutUI::GetCurSel() const
	{
		return m_iCurSel;
	}

	CControlUI* CAutoLayoutUI::GetCurSel()
	{
		return m_pSelControl;
	}

	bool CAutoLayoutUI::SelectItem(int nIndex, bool bTakeFocus)
	{
		int iOldSel = m_iCurSel;

		int nCount = GetCount();

		if (nIndex < 0 && nIndex > nCount-1) return false;

		CControlUI* pControl = GetItemAt(nIndex);
		if (!pControl) return false;

		if (!pControl->IsVisible() || !pControl->IsEnabled())
			return false;

		m_iCurSel = nIndex;
		if(m_pSelControl) m_pSelControl->Invalidate();
		m_pSelControl = pControl;
		m_pSelControl->Invalidate();
		EnsureVisible(m_iCurSel);

		if (bTakeFocus)
		{
			pControl->SetFocus();
		}

		if (m_pManager)
		{
			TNotifyUI notify;
			notify.pSender = this;
			notify.sType = DUI_MSGTYPE_ITEMSELECT;
			notify.wParam = m_iCurSel;
			notify.lParam = iOldSel;
			/*if(this->OnNotify)
			OnNotify(&notify);*/
			m_pManager->SendNotify(notify);

		}

		return true;
	}

	void CAutoLayoutUI::EnsureVisible(int nIndex)
	{
		if(!m_bEnsureVisible || m_iCurSel < 0) return;
		RECT rcItem = GetItemAt(nIndex)->GetPos();
		RECT rcList = this->GetPos();
		RECT rcListInset = this->GetInset();

		rcList.left += rcListInset.left;
		rcList.top += rcListInset.top;
		rcList.right -= rcListInset.right;
		rcList.bottom -= rcListInset.bottom;

		if(1/*m_bVertical*/)
		{
			if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
			if( rcItem.top < rcList.top && rcItem.bottom > rcList.bottom ) return;
			int dy = 0;
			if( rcItem.top < rcList.top )
				dy = rcItem.top - rcList.top; //-
			else if( rcItem.bottom > rcList.bottom )
				dy = rcItem.bottom - rcList.bottom;
			if( dy == 0 ) return;
			SIZE sz = this->GetScrollPos();
			this->SetScrollPos(CSize(sz.cx , sz.cy + dy));
		}
		else
		{
			if( rcItem.left < rcList.left && rcItem.right > rcList.right ) return;
			if( rcItem.left >= rcList.left && rcItem.right < rcList.right ) return;
			int dx = 0;
			if( rcItem.left < rcList.left )
				dx =  rcItem.left - rcList.left;
			else if( rcItem.right > rcList.right ) 
				dx = rcItem.right - rcList.right;
			if( dx == 0 ) return;
			SIZE sz = this->GetScrollPos();
			this->SetScrollPos(CSize(sz.cx + dx, sz.cy));
		}

	}

	CControlUI* CAutoLayoutUI::PtHitControl(POINT ptMouse, int &nIndex)
	{
		CControlUI* pControl = NULL;
		nIndex = 0;
		int nCount = GetCount();
		for(int index = 0; index < nCount; index++)
		{
			CControlUI* pCtl = this->GetItemAt(index);
			if(pCtl && pCtl->IsVisible())
			{
				CRect rcItem = pCtl->GetPos();
				if(::PtInRect(&rcItem,ptMouse))
				{
					pControl = pCtl;
					nIndex = index;
					break;
				}
			}
		}
		return pControl;
	}

	void CAutoLayoutUI::LoadItemBkImage(CUIString strFormat)
	{
		int nCount = GetCount();
		CUIString strImg;
		for(int index = 0; index < nCount; index++)
		{
			CControlUI* pCtl = this->GetItemAt(index);
			if(pCtl)
			{
				strImg.Format(strFormat.GetData(),index+1);
				pCtl->SetBkImage(strImg.GetData());
			}
		}
	}

	void CAutoLayoutUI::ClearAll()
	{
		this->RemoveAll();
		m_pHotControl = NULL;
		m_pSelControl = NULL;
		m_iCurSel = -1;
	}

	bool CAutoLayoutUI::CheckRectInvalid(RECT &rc)
	{
		if(rc.right < rc.left || rc.bottom < rc.top)
			return true;
		return false;
	}
}
