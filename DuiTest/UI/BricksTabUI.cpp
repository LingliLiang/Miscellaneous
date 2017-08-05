#include "stdafx.h"
#include "BricksTabUI.h"

#define BRICKSOPTIONUI COptionUI/*Container*/

namespace DirectUI
{

	COptionContainerUI::COptionContainerUI()
		:m_rcInset()
	{
	}

	COptionContainerUI::~COptionContainerUI()
	{
		RemoveAll();
	}

	LPCTSTR COptionContainerUI::GetClass() const
	{
		return _T("OptionContainerUI");
	}

	LPVOID COptionContainerUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_OPTIONCONTAINER) == 0) return static_cast<COptionContainerUI*>(this);
		else if (_tcscmp(pstrName, _T("IContainer")) == 0) return static_cast<IContainerUI*>(this);
		return COptionUI::GetInterface(pstrName);
	}

	void COptionContainerUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
	{
		for (int it = 0; it < m_items.GetSize(); it++)
		{
			static_cast<CControlUI*>(m_items[it])->SetManager(pManager, this, bInit);
		}

		COptionUI::SetManager(pManager, pParent, bInit);
	}

	SIZE COptionContainerUI::EstimateSize(SIZE szAvailable)
	{
		return COptionUI::EstimateSize(szAvailable);
	}

	void COptionContainerUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
			if (_tcscmp(pstrName, _T("inset")) == 0)
			{
				RECT rcInset = { 0 };
				LPTSTR pstr = NULL;
				rcInset.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
				rcInset.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
				rcInset.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
				rcInset.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
				m_rcInset = rcInset;
			}
			else COptionUI::SetAttribute(pstrName, pstrValue);
	}
	
	CControlUI* COptionContainerUI::GetItemAt(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= m_items.GetSize()) return NULL;
		return static_cast<CControlUI*>(m_items[nIndex]);
	}

	int COptionContainerUI::GetItemIndex(CControlUI* pControl) const
	{
		for (int it = 0; it < m_items.GetSize(); it++) {
			if (static_cast<CControlUI*>(m_items[it]) == pControl) {
				return it;
			}
		}

		return -1;
	}

	bool COptionContainerUI::SetItemIndex(CControlUI* pControl, int nIndex)
	{
		for (int it = 0; it < m_items.GetSize(); it++) {
			if (static_cast<CControlUI*>(m_items[it]) == pControl) {
				NeedUpdate();
				m_items.Remove(it);
				return m_items.InsertAt(nIndex, pControl);
			}
		}

		return false;
	}

	int COptionContainerUI::GetCount() const
	{
		return m_items.GetSize();
	}

	bool COptionContainerUI::Add(CControlUI* pControl)
	{
		if (pControl == NULL) return false;

		if (m_pManager != NULL) m_pManager->InitControls(pControl, this);
		if (IsVisible()) NeedUpdate();
		else pControl->SetInternVisible(false);
		return m_items.Add(pControl);
	}

	bool COptionContainerUI::AddAt(CControlUI* pControl, int nIndex)
	{
		if (pControl == NULL) return false;

		if (m_pManager != NULL) m_pManager->InitControls(pControl, this);
		if (IsVisible()) NeedUpdate();
		else pControl->SetInternVisible(false);
		return m_items.InsertAt(nIndex, pControl);
	}

	bool COptionContainerUI::Remove(CControlUI* pControl)
	{
		if (pControl == NULL) return false;

		for (int it = 0; it < m_items.GetSize(); it++)
		{
			if (static_cast<CControlUI*>(m_items[it]) == pControl)
			{
				m_items.Remove(it);
				NeedUpdate();
				delete pControl;
				return true;
			}
		}

		return false;
	}

	bool COptionContainerUI::RemoveAt(int nIndex)
	{
		CControlUI* pControl = GetItemAt(nIndex);
		if (pControl != NULL) {
			return Remove(pControl);
		}

		return false;
	}

	void COptionContainerUI::RemoveAll()
	{
		for (int it = 0; it < m_items.GetSize(); it++) {
			delete static_cast<CControlUI*>(m_items[it]);
		}
		m_items.Empty();
		NeedUpdate();
	}

	void COptionContainerUI::MoveItem(int nSrcIndex, int nDesIndex)
	{
		if ((nSrcIndex == nDesIndex) || (nSrcIndex<0) || (nDesIndex<0))
		{
			return;
		}

		int nItemCount = m_items.GetSize();
		if (nItemCount <= nSrcIndex || nItemCount <= nDesIndex)
		{
			return;
		}

		m_items.Move(nSrcIndex, nDesIndex);

		NeedUpdate();
	}

	void COptionContainerUI::SetPos(RECT rc)
	{
		COptionUI::SetPos(rc);
		if (m_items.IsEmpty()) return;
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		for (int it = 0; it < m_items.GetSize(); it++)
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
			if (!pControl->IsVisible()) continue;
			if (pControl->IsFloat()) SetFloatPos(it);
			else pControl->SetPos(rc); // 所有非float子控件放大到整个客户区
		}
	}

	void COptionContainerUI::DoPaint(HDC hDC, const RECT & rcPaint)
	{
		RECT rcTemp = { 0 };
		if (!::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) return;

		CRenderClip clip;
		CRenderClip::GenerateClip(hDC, rcTemp, clip);
		COptionUI::DoPaint(hDC, rcPaint);

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
	}

	void COptionContainerUI::SetFloatPos(int nIndex)
	{
		// 因为CControlUI::SetPos对float的操作影响，这里不能对float组件添加滚动条的影响
		if (nIndex < 0 || nIndex >= m_items.GetSize()) return;

		CControlUI* pControl = static_cast<CControlUI*>(m_items[nIndex]);

		if (!pControl->IsVisible()) return;
		if (!pControl->IsFloat()) return;

		SIZE szXY = pControl->GetFixedXY();
		SIZE sz = { pControl->GetFixedWidth(), pControl->GetFixedHeight() };
		RECT rcCtrl = { 0 };
		if (szXY.cx >= 0)
		{
			rcCtrl.left = m_rcItem.left + szXY.cx;
			rcCtrl.right = m_rcItem.left + szXY.cx + sz.cx;
		}
		else
		{
			rcCtrl.left = m_rcItem.right + szXY.cx - sz.cx;
			rcCtrl.right = m_rcItem.right + szXY.cx;
		}

		if (szXY.cy >= 0)
		{
			rcCtrl.top = m_rcItem.top + szXY.cy;
			rcCtrl.bottom = m_rcItem.top + szXY.cy + sz.cy;
		}
		else
		{
			rcCtrl.top = m_rcItem.bottom + szXY.cy - sz.cy;
			rcCtrl.bottom = m_rcItem.bottom + szXY.cy;
		}

		if (pControl->IsRelativePos())
		{
			TRelativePosUI tRelativePos = pControl->GetRelativePos();
			SIZE szParent = { m_rcItem.right - m_rcItem.left,m_rcItem.bottom - m_rcItem.top };
			if (tRelativePos.szParent.cx != 0)
			{
				int nIncrementX = szParent.cx - tRelativePos.szParent.cx;
				int nIncrementY = szParent.cy - tRelativePos.szParent.cy;
				rcCtrl.left += (nIncrementX*tRelativePos.nMoveXPercent / 100);
				rcCtrl.top += (nIncrementY*tRelativePos.nMoveYPercent / 100);
				rcCtrl.right = rcCtrl.left + sz.cx + (nIncrementX*tRelativePos.nZoomXPercent / 100);
				rcCtrl.bottom = rcCtrl.top + sz.cy + (nIncrementY*tRelativePos.nZoomYPercent / 100);
			}
			pControl->SetRelativeParentSize(szParent);
		}
		pControl->SetPos(rcCtrl);
	}
	
	CControlUI* COptionContainerUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
	{
		// Check if this guy is valid
		if ((uFlags & UIFIND_VISIBLE) != 0 && !IsVisible()) return NULL;
		if ((uFlags & UIFIND_ENABLED) != 0 && !IsEnabled()) return NULL;
		if ((uFlags & UIFIND_HITTEST) != 0)
		{
			if (!::PtInRect(&m_rcItem, *(static_cast<LPPOINT>(pData)))) return NULL;
		}

		CControlUI* pResult = NULL;
		if (pResult != NULL) return pResult;

		if ((uFlags & UIFIND_ME_FIRST) != 0)
		{
			CControlUI* pControl = CControlUI::FindControl(Proc, pData, uFlags);
			if (pControl != NULL) return pControl;
		}

		RECT rc = m_rcItem;
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;
		if ((uFlags & UIFIND_TOP_FIRST) != 0)
		{
			for (int it = m_items.GetSize() - 1; it >= 0; it--)
			{
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it])->FindControl(Proc, pData, uFlags);
				if (pControl != NULL)
				{
					if ((uFlags & UIFIND_HITTEST) != 0 && !pControl->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))))
						continue;
					else
						return pControl;
				}
			}
		}
		else
		{
			for (int it = 0; it < m_items.GetSize(); it++)
			{
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it])->FindControl(Proc, pData, uFlags);
				if (pControl != NULL)
				{
					if ((uFlags & UIFIND_HITTEST) != 0 && !pControl->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))))
						continue;
					else
						return pControl;
				}
			}
		}

		if (pResult == NULL && (uFlags & UIFIND_ME_FIRST) == 0)
			pResult = CControlUI::FindControl(Proc, pData, uFlags);

		return pResult;
	}
	
	/***************************CBricksTabUI***************************************/

	CBricksTabUI::CBricksTabUI()
		:m_bVertical(true),m_nCurSel(-1),m_bMuliVisible(false)
	{
		m_layout.container = NULL;
	}

	CBricksTabUI::~CBricksTabUI()
	{
	}

	LPCTSTR CBricksTabUI::GetClass() const
	{
		return _T("BricksTabUI");
	}

	LPVOID CBricksTabUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_BRICKSTAB) == 0) return static_cast<CBricksTabUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	void CBricksTabUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("selectedid")) == 0) 
		{
			SelectItem(_ttoi(pstrValue));
			return;
		}
		else if (_tcscmp(pstrName, _T("vermode")) == 0)
		{
			SetVerticalMode(_tcscmp(pstrValue, _T("true")) == 0);
			return;
		}
		else if(_tcscmp(pstrName, _T("multivisible")) == 0)
		{
			SetMultiVisible(_tcscmp(pstrValue, _T("true")) == 0);
			return;
		}
		else
		{
			CUIString strName = pstrName;
			if (_tcscmp(strName.Left(6).GetData(), _T("option")) == 0)
			{
				strName = strName.Right(strName.GetLength() - 6);
				SetItemAttribute(strName.GetData(), pstrValue);
			}
			return;
		}
		CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	void CBricksTabUI::SetItemAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		CContainerUI* pContainer = m_layout.container == NULL? this : m_layout.container;
		int nCount = pContainer->GetCount();
		bool bSet = false;
		for (int index = 0; index < nCount; index++)
		{
			CControlUI* pCtl = pContainer->GetItemAt(index);
			bSet = pCtl->GetInterface(DUI_CTR_OPTION) != NULL;
			if (pCtl && bSet)
			{
				pCtl->SetAttribute(pstrName, pstrValue);
			}
		}
	}

	//Attributes
	bool CBricksTabUI::GetVerticalMode() const
	{
		return m_bVertical;
	}

	void CBricksTabUI::SetVerticalMode(bool bVertical)
	{
		m_bVertical = bVertical;
	}

	bool CBricksTabUI::GetMultiVisible() const
	{
		return m_bMuliVisible;
	}

	void CBricksTabUI::SetMultiVisible(bool b)
	{
		m_bMuliVisible = b;
	}

	bool CBricksTabUI::OptionEvent(LPVOID pEvent_)
	{
		assert(pEvent_);
		TEventUI & event_ = *(static_cast<TEventUI*>(pEvent_));
		
		if (event_.Type == UIEVENT_SETFOCUS)
		{
		}
		if (event_.Type == UIEVENT_KILLFOCUS)
		{
		}
		if (event_.Type == UIEVENT_BUTTONUP)
		{
		}
		if (event_.Type == UIEVENT_MOUSEENTER)
		{
		}
		if (event_.Type == UIEVENT_MOUSELEAVE)
		{
		}
		return true;
	}

	bool CBricksTabUI::OptionNotify(LPVOID pNotify)
	{
		assert(pNotify);
		TNotifyUI & notify = *(static_cast<TNotifyUI*>(pNotify));

		if(notify.sType == DUI_MSGTYPE_SELECTCHANGED)
		{
			if (m_layout.container)
			{
				BRICKSOPTIONUI* pOption = static_cast<BRICKSOPTIONUI*>(notify.pSender);
				int index = m_layout.container->GetItemIndex(pOption);
				if (index != -1)
				{
					if(!m_bMuliVisible && pOption->IsSelected()){
						for (int it = 0; it < m_layout.container->GetCount(); it+=2)
						{
							BRICKSOPTIONUI* pOption2 = static_cast<BRICKSOPTIONUI*>( m_layout.container->GetItemAt(it));
							if(index == it)
								m_layout.container->GetItemAt(it+1)->SetVisible(true);
							else
								m_layout.container->GetItemAt(it+1)->SetVisible(false);
						}
					}
					else
					{
						if(pOption->IsSelected())
							m_layout.container->GetItemAt(index+1)->SetVisible(true);
						else
							m_layout.container->GetItemAt(index+1)->SetVisible(false);
					}
				}
			}
		}
		return false;
	}

	CUIString CBricksTabUI::GetGroupName()
	{
		CUIString str;
		str.Format(_T("BricksTab_%p"),this);
		return str;
	}

	void CBricksTabUI::ApplyGroupName()
	{
		if(!m_bMuliVisible && m_layout.container){
			for (int index = 0; index < m_layout.container->GetCount(); index+=2)
			{
				BRICKSOPTIONUI* pOption = static_cast<BRICKSOPTIONUI*>( m_layout.container->GetItemAt(index));
				if(pOption) pOption->SetGroup(GetGroupName());
			}
		}
	}

	bool CBricksTabUI::Add(CControlUI* pControl)
	{
		bool ret = false;
		if(m_layout.container == NULL)
		{
			ret = __super::Add(pControl);
		}
		else
		{
			ret = m_layout.container->Add(pControl);
		}
		if( !ret ) return ret;
		if(pControl->GetInterface(DUI_CTR_OPTION) == NULL)
		{
			pControl->SetVisible(false);
		}
		return ret;
	}

	bool CBricksTabUI::AddAt(CControlUI* pControl, int nIndex)
	{
		bool ret = false;
		if(m_layout.container == NULL)
		{
			ret = __super::AddAt(pControl,nIndex);
		}
		else
		{
			ret = m_layout.container->AddAt(pControl,nIndex);
		}
		if (!ret) return ret;

		if(pControl->GetInterface(DUI_CTR_OPTION) == NULL)
		{
			pControl->SetVisible(false);
		}

		return ret;
	}

	bool CBricksTabUI::Remove(CControlUI* pControl)
	{
		if (pControl == NULL) return false;

		CContainerUI* pContainer = 0;
		bool ret = false;
		int index = GetItemIndex(pControl);
		if(m_layout.container == NULL)
		{
			ret = __super::Remove(pControl);
			pContainer = this;
		}
		else
		{
			ret = m_layout.container->Remove(pControl);
			pContainer = m_layout.container;
		}
		if( !ret ) return false;

		//if (m_nCurSel == index)
		//{
		//	if (pContainer->GetCount() > 0)
		//	{
		//		m_nCurSel=0;
		//		GetItemAt(m_nCurSel)->SetVisible(true);
		//	}
		//	else m_nCurSel=-1;
		//	NeedParentUpdate();
		//}
		//else if (m_nCurSel > index)
		//{
		//	m_nCurSel -= 1;
		//}

		return ret;
	}

	void CBricksTabUI::RemoveAll()
	{
		if(m_layout.container == NULL)
		{
			__super::RemoveAll();
		}
		else
		{
			m_layout.container->RemoveAll();
		}
		m_nCurSel = -1;
		NeedParentUpdate();
	}

	int CBricksTabUI::GetCurSel() const
	{
		return m_nCurSel;
	}

	bool CBricksTabUI::SelectItem(int nIndex)
	{
		CContainerUI* pContainer = m_layout.container;
		if(!pContainer) return false;
		if (nIndex < 0 || nIndex >= pContainer->GetCount()/2) return false;
		if (nIndex == m_nCurSel) return true;

		int iOldSel = m_nCurSel;
		m_nCurSel = nIndex;
		for (int it = 1; it <  pContainer->GetCount(); it+=2)
		{
			if (it == nIndex*2+1)
			{
				BRICKSOPTIONUI* pOption = static_cast<BRICKSOPTIONUI*>( pContainer->GetItemAt(it-1));
				if(pOption) pOption->Selected(true);
			}
			else
			{
				if(!m_bMuliVisible)
					 pContainer->GetItemAt(it)->SetVisible(false);
			}
		}

		NeedParentUpdate();

		if (m_pManager != NULL)
		{
			m_pManager->SendNotify(this, DUI_MSGTYPE_TABSELECT, m_nCurSel, iOldSel);
		}
		return true;
	}

	bool CBricksTabUI::SelectItem(CControlUI* pControl)
	{
		if(!m_layout.container || !pControl) return false;
		bool	bSel = pControl->GetInterface(DUI_CTR_OPTION) !=0;
		if(bSel)
		{
			int nIndex = m_layout.container->GetItemIndex(pControl);
			if (nIndex == -1)
				return false;
			else
				return SelectItem((nIndex/2));
		}
		return false;
	}

	void CBricksTabUI::Init()
	{
		InvalidLayout();
		ApplyGroupName();
		if(m_nCurSel == -1) SelectItem(0);
		__super::Init();
	}

	void CBricksTabUI::InvalidLayout()
	{
		auto RemoveUI = [](CContainerUI* pThis, CStdPtrArray& itemArray)->void
		{
			for (int i = 0; i < itemArray.GetSize(); i++)
			{
				pThis->RemoveNotDestroy(static_cast<CControlUI*>(itemArray[i]));
			}
		};
		auto AddUI = [](CContainerUI* pThis, CStdPtrArray& itemArray)->void
		{
			for (int i = 0; i < itemArray.GetSize(); i++)
			{
				pThis->Add(static_cast<CControlUI*>(itemArray[i]));
			}
		};
		auto GetItems = [](CContainerUI* pThis, CStdPtrArray& itemArray)->void
		{
			for (int i = 0; i < pThis->GetCount(); i++)
			{
				itemArray.Add(pThis->GetItemAt(i));
			}
		};
		auto CopyItems = [](CStdPtrArray& itemArrayto, CStdPtrArray& itemArray)->void
		{
			for (int i = 0; i < itemArray.GetSize(); i++)
			{
				itemArrayto.Add(itemArray[i]);
			}
		};
		auto InsertTitle = [&](CStdPtrArray& itemArray, CBricksTabUI* pThis)->void
		{
			int size = itemArray.GetSize();
			for (int i = 0; i < size;i++)
			{
				BRICKSOPTIONUI* pTitle = 0;
				CControlUI* pControl = static_cast<CControlUI*>(itemArray[i]);
				if (pControl->GetInterface(DUI_CTR_OPTION) != NULL)
				{
					pTitle = static_cast<BRICKSOPTIONUI*>(pControl);
					pTitle->OnEvent += MakeDelegate(this, &CBricksTabUI::OptionEvent);
					pTitle->OnNotify += MakeDelegate(this, &CBricksTabUI::OptionNotify);
					i++;
					pControl = NULL;
					if(i < size)
						pControl = static_cast<CControlUI*>(itemArray[i]);
					if (!pControl || pControl->GetInterface(DUI_CTR_OPTION) != NULL)
					{
						//add new 
						pControl = new CContainerUI;
						itemArray.InsertAt(i, pControl);
						size++;
						pTitle->SetTag((UINT_PTR)pControl);
					}
					else
					{
						pTitle->SetTag((UINT_PTR)pControl);
						pTitle->SetText(pControl->GetText());
					}
				}
				else
				{
					//insert
					pTitle = new COptionContainerUI;
					pTitle->OnEvent += MakeDelegate(this, &CBricksTabUI::OptionEvent);
					pTitle->OnNotify += MakeDelegate(this, &CBricksTabUI::OptionNotify);
					pTitle->SetText(pControl->GetText());
					itemArray.InsertAt(i++, pTitle);
					size++;
					pTitle->SetTag((UINT_PTR)itemArray[i]);
				}
			}
		};
		CStdPtrArray itemArray;
		if (m_items.GetSize() == 0) return;
		if (m_layout.container != NULL)
		{
			if (m_layout.container != m_items[m_items.GetSize() - 1]) return;
			GetItems(m_layout.container, itemArray);
			RemoveUI(m_layout.container, itemArray);
			this->RemoveNotDestroy(m_layout.container);
			delete m_layout.container;

			m_layout.container = NULL;
			m_bVertical ? m_layout.container = new CVerticalLayoutUI : m_layout.container = new CHorizontalLayoutUI;
			CContainerUI::Add(m_layout.container);
			AddUI(m_layout.container, itemArray);
		}
		else
		{
			CopyItems(itemArray, m_items);
			RemoveUI(this, itemArray);
			m_bVertical ? m_layout.container = new CVerticalLayoutUI : m_layout.container = new CHorizontalLayoutUI;
			CContainerUI::Add(m_layout.container);
			InsertTitle(itemArray, this);
			AddUI(m_layout.container, itemArray);
		}

	}

	 void CBricksTabUI::DoPaint(HDC hDC, const RECT& rcPaint)
	 {
		 CContainerUI::DoPaint(hDC,rcPaint);
	 }

}