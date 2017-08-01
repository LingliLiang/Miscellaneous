#include "stdafx.h"
#include "BricksTabUI.h"

namespace DirectUI
{
	/***************************COptionContainerUI***************************************/

	LPCTSTR COptionContainerUI::GetClass() const
	{
		return _T("OptionContainerUI");
	}

	LPVOID COptionContainerUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_OPTIONCONTAINER) == 0) return static_cast<COptionContainerUI*>(this);
		else if( _tcscmp(pstrName, _T("IContainer")) == 0 ) return static_cast<IContainerUI*>(this);
		return COptionUI::GetInterface(pstrName);
	}


	CControlUI* COptionContainerUI::GetItemAt(int nIndex) const
	{
		return 0;
	}

	int COptionContainerUI::GetItemIndex(CControlUI* pControl) const
	{
		return 0;
	}

	bool COptionContainerUI::SetItemIndex(CControlUI* pControl, int nIndex){return false;}
	
	int COptionContainerUI::GetCount() const
	{
		return 0;
	}
	
	bool COptionContainerUI::Add(CControlUI* pControl)
	{
		return false;
	}

	bool COptionContainerUI::AddAt(CControlUI* pControl, int nIndex)
	{
		return false;
	}
	
	bool COptionContainerUI::Remove(CControlUI* pControl)
	{
		return false;
	}
	
	bool COptionContainerUI::RemoveAt(int nIndex)
	{
		return false;
	}
	
	void COptionContainerUI::RemoveAll()
	{
	}

	/***************************CBricksTabUI***************************************/

	CBricksTabUI::CBricksTabUI()
		:m_bVertical(false),m_nCurSel(-1)
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
		else
		{
			CUIString strName = pstrName;
			if (_tcscmp(strName.Left(5).GetData(), _T("title")) == 0)
			{
				strName = strName.Right(strName.GetLength() - 5);
				SetItemAttribute(strName.GetData(), pstrValue);
			}
			return;
		}
		CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	void CBricksTabUI::SetItemAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		CContainerUI* pContainer = m_layout.container;
		if(!pContainer) pContainer = this;
		int nCount = pContainer->GetCount();
		bool bSet = false;
		for (int index = 0; index < nCount; index++)
		{
			CControlUI* pCtl = pContainer->GetItemAt(index);
			bSet = _tcsicmp(pCtl->GetClass(), _T("OptionContainerUI")) != 0;
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

	bool CBricksTabUI::OptionEvent(LPVOID pEvent_)
	{
		assert(pEvent_);
		TEventUI & event_ = *(static_cast<TEventUI*>(pEvent_));
		
		if (event_.Type == UIEVENT_SETFOCUS)
		{
			return false;
		}
		if (event_.Type == UIEVENT_KILLFOCUS)
		{
			return false;
		}
		if (event_.Type == UIEVENT_BUTTONUP)
		{
			::OutputDebugStringA("UIEVENT_BUTTONUP\n");
			if (m_layout.container)
			{
				SetItemAttribute(_T("visible"), _T("false"));
				int index = m_layout.container->GetItemIndex(event_.pSender);
				if (index != -1)
				{
					static_cast<CControlUI*>(m_layout.container->GetItemAt(++index))->SetVisible(true);
				}
			}

			return false;
		}
		if (event_.Type == UIEVENT_MOUSEENTER)
		{
			// return;
		}
		if (event_.Type == UIEVENT_MOUSELEAVE)
		{
			// return;
		}
			return false;
	}

	void CBricksTabUI::SetVerticalMode(bool bVertical)
	{
		m_bVertical = bVertical;
	}

	bool CBricksTabUI::Add(CControlUI* pControl)
	{
		bool ret = CContainerUI::Add(pControl);
		if( !ret ) return ret;

		if(m_nCurSel == -1 && pControl->IsVisible())
		{
			m_nCurSel = GetItemIndex(pControl);
		}
		else
		{
			pControl->SetVisible(false);
		}

		return ret;
	}

	bool CBricksTabUI::AddAt(CControlUI* pControl, int nIndex)
	{
		bool ret = CContainerUI::AddAt(pControl, nIndex);
		if (!ret) return ret;

		if (m_nCurSel == -1 && pControl->IsVisible())
		{
			m_nCurSel = GetItemIndex(pControl);
		}
		else if (m_nCurSel != -1 && nIndex <= m_nCurSel)
		{
			m_nCurSel += 1;
		}
		else
		{
			pControl->SetVisible(false);
		}

		return ret;
	}

	bool CBricksTabUI::Remove(CControlUI* pControl)
	{
		if (pControl == NULL) return false;

		int index = GetItemIndex(pControl);
		bool ret = CContainerUI::Remove(pControl);
		if( !ret ) return false;

		if (m_nCurSel == index)
		{
			if (GetCount() > 0)
			{
				m_nCurSel=0;
				GetItemAt(m_nCurSel)->SetVisible(true);
			}
			else m_nCurSel=-1;
			NeedParentUpdate();
		}
		else if (m_nCurSel > index)
		{
			m_nCurSel -= 1;
		}

		return ret;
	}

	void CBricksTabUI::RemoveAll()
	{
		m_nCurSel = -1;
		CContainerUI::RemoveAll();
		NeedParentUpdate();
	}

	int CBricksTabUI::GetCurSel() const
	{
		return m_nCurSel;
	}

	bool CBricksTabUI::SelectItem(int nIndex)
	{
		if (nIndex < 0 || nIndex >= m_items.GetSize()) return false;
		if (nIndex == m_nCurSel) return true;

		int iOldSel = m_nCurSel;
		m_nCurSel = nIndex;
		for (int it = 0; it < m_items.GetSize(); it++)
		{
			if (it == nIndex)
			{
				GetItemAt(it)->SetVisible(true);
				GetItemAt(it)->SetFocus();
				SetPos(m_rcItem);
			}
			else GetItemAt(it)->SetVisible(false);
		}

		NeedParentUpdate();

		if (m_pManager != NULL)
		{
			m_pManager->SetNextTabControl();
			m_pManager->SendNotify(this, DUI_MSGTYPE_TABSELECT, m_nCurSel, iOldSel);
		}
		return true;
	}

	bool CBricksTabUI::SelectItem(CControlUI* pControl)
	{
		int nIndex = GetItemIndex(pControl);
		if (nIndex == -1)
			return false;
		else
			return SelectItem(nIndex);
	}

	void CBricksTabUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		rc = m_rcItem;

		// Adjust for inset
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		for (int it = 0; it < m_items.GetSize(); it++) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
			if (!pControl->IsVisible()) continue;
			if (pControl->IsFloat()) 
			{
				SetFloatPos(it);
				continue;
			}

			if (it != m_nCurSel) continue;

			RECT rcPadding = pControl->GetPadding();
			rc.left += rcPadding.left;
			rc.top += rcPadding.top;
			rc.right -= rcPadding.right;
			rc.bottom -= rcPadding.bottom;

			SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
			SIZE sz = pControl->EstimateSize(szAvailable);
			if (sz.cx == 0)
			{
				sz.cx = max(0, szAvailable.cx);
			}

			if (sz.cx < pControl->GetMinWidth()) sz.cx = pControl->GetMinWidth();
			if (sz.cx > pControl->GetMaxWidth()) sz.cx = pControl->GetMaxWidth();

			if (sz.cy == 0) 
			{
				sz.cy = min(0, szAvailable.cy);
			}

			if (sz.cy < pControl->GetMinHeight()) sz.cy = pControl->GetMinHeight();
			if (sz.cy > pControl->GetMaxHeight()) sz.cy = pControl->GetMaxHeight();

			RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy};
			pControl->SetPos(rcCtrl);
		}
	}

	void CBricksTabUI::Init()
	{
		InvalidLayout();
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
			int size = itemArray.GetSize() * 2;
			for (int i = 0; i < size; i += 2)
			{
				COptionContainerUI* pTitle = new COptionContainerUI;
				pTitle->OnEvent.Empty();
				pTitle->OnEvent += MakeDelegate(this,&CBricksTabUI::OptionEvent);
				pTitle->SetText(static_cast<CControlUI*>(itemArray[i])->GetText());
				itemArray.InsertAt(i, pTitle);
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
			this->Add(m_layout.container);
			AddUI(m_layout.container, itemArray);
		}
		else
		{
			CopyItems(itemArray, m_items);
			RemoveUI(this, itemArray);
			m_bVertical ? m_layout.container = new CVerticalLayoutUI : m_layout.container = new CHorizontalLayoutUI;
			this->Add(m_layout.container);
			InsertTitle(itemArray, this);
			AddUI(m_layout.container, itemArray);
		}

	}

}