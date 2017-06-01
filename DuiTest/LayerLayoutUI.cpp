#include "stdafx.h"
#include "LayerLayoutUI.h"
#include <hash_set>

#define UI(ctr) ctr##UI 
const TCHAR*  DUI_CTR_LAYERLIST = _T("LayerList");
const TCHAR*  DUI_CTR_LAYERITEM = _T("Layer");
const TCHAR*  DUI_CTR_GROUPITEM = _T("Group");
const TCHAR*  DUI_CTR_GROUPHEADER= _T("GroupHeader");
const TCHAR*  INTERFACE_INTERMSG  =  _T("IInterMessage");

namespace DirectUI
{
	using namespace Inter;

	void Inter::AttachEvent(CControlUI* pControl, IInterMessage* pthis, EventFunc pfunc, bool self )
	{
		if (pControl == NULL) return;
		if(self)
		{
			pControl->OnEvent+= MakeDelegate<IInterMessage,IInterMessage>(pthis,pfunc);
			return;
		}
		CContainerUI* pContainer = static_cast<CContainerUI*>(pControl->GetInterface(DUI_CTR_CONTAINER));
		if(pContainer == NULL)
		{
			pControl->OnEvent+= MakeDelegate<IInterMessage,IInterMessage>(pthis,pfunc);
			return;
		}
		for(int it=0;it<pContainer->GetCount();it++)
		{
			AttachEvent(pContainer->GetItemAt(it),pthis,pfunc);
		}
	}

	void Inter::ShareInfo(CControlUI* pControl, shared_ptr<_tagLayerInfo>& info)
	{
		if(!pControl) return;
		if(_tcscmp(pControl->GetClass(), TEXT("GroupUI") ) == 0)
		{
			CGroupUI* pGroup = (CGroupUI*)(pControl->GetInterface(DUI_CTR_GROUPITEM));
			pGroup->m_LayerInfo = info;
			for(int it=0;it<pGroup->GetCount();it++)
			{
				ShareInfo(pGroup->GetItemAt(it),info);
			}
		}
		else if(_tcscmp(pControl->GetClass(), TEXT("LayerUI") ) == 0)
		{
			CLayerUI* pLayer = (CLayerUI*)(pControl->GetInterface(DUI_CTR_LAYERITEM));
			pLayer->m_LayerInfo = info;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//CLayerUI

	CLayerUI::CLayerUI()
	{

	}

	LPCTSTR CLayerUI::GetClass() const
	{
		return _T("LayerUI");
	}

	LPVOID CLayerUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_LAYERITEM) == 0 ) return static_cast<CLayerUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	UINT CLayerUI::GetControlFlags() const
	{
		return UIFLAG_SETCURSOR;
	}

	void CLayerUI::Message(TEventUI* event_, InterNotifyMsg what)
	{
		if(m_pParent)
		{
			IInterMessage* pInter = static_cast<IInterMessage*>(m_pParent->GetInterface(INTERFACE_INTERMSG));
			if(pInter)
				pInter->Message(event_,what);
		}
	}

	void CLayerUI::DoEvent(TEventUI& event_)
	{
		if (!IsMouseEnabled() && event_.Type > UIEVENT__MOUSEBEGIN && event_.Type < UIEVENT__MOUSEEND)
		{
			if (m_pParent != NULL) m_pParent->DoEvent(event_);
			else CContainerUI::DoEvent(event_);
			return;
		}
		if (event_.Type == UIEVENT_BUTTONDOWN)
		{
			//SetExpand(!m_bExpand);
			//Invalidate();
			//this->NeedParentUpdate();
			//Message(0,NeedLayoutUpdate);
			event_.pSender = this;
			Message(&event_,LayerItemDrag);
			m_bButtonDown = true;
			return;
		}
		if (event_.Type == UIEVENT_MOUSEMOVE)
		{
			if(m_bButtonDown)
			{
				Message(&event_,LayerItemMove);
			}
			return;
		}
		if (event_.Type == UIEVENT_BUTTONUP)
		{
			if(m_bButtonDown)
			{
				Message(&event_,LayerItemDrop);
				m_bButtonDown = false;
			}
			return;
		}
		CContainerUI::DoEvent(event_);
	}

	bool CLayerUI::OnChildEvent(void* event_in)
	{
		TEventUI* pEvent = static_cast<TEventUI*>(event_in);
		if(!pEvent) return false;
		if (pEvent->Type == UIEVENT_BUTTONDOWN)
		{
			pEvent->pSender = this;
			Message(pEvent,LayerItemSelect);
		}
		return true;
	}

	bool CLayerUI::Add(CControlUI* pControl)
	{
		if(pControl == NULL) return false;
		AttachEvent(pControl,this,(EventFunc)&CLayerUI::OnChildEvent);
		return __super::Add(pControl);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//CGroupUI

	CGroupUI::CGroupUI()
		:m_itemInset(CRect()),m_itemHeight(40),m_defaultHeaderHeight(40),
		m_bExpand(0),m_bButtonDown(false)
	{


	}

	CGroupUI::~CGroupUI()
	{
		if(m_items.GetSize() > 0 &&
			_tcscmp(static_cast<CControlUI*>(m_items[0])->GetClass(), TEXT("GroupHeaderUI") ) == 0 )
		{
			m_items.Remove(0); // leave GroupHeaderUI
		}

	}

	LPCTSTR CGroupUI::GetClass() const
	{
		return _T("GroupUI");
	}

	LPVOID CGroupUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_GROUPITEM) == 0 ) return static_cast<CGroupUI*>(this);
		if( _tcscmp(pstrName, INTERFACE_INTERMSG) == 0 ) return static_cast<IInterMessage*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	UINT CGroupUI::GetControlFlags() const
	{
		return UIFLAG_SETCURSOR;
	}

	SIZE CGroupUI::EstimateSize(SIZE szAvailable)
	{
		size_t cyFixed = 0;
		size_t nEstimateNum = 0;
		size_t headerHeight = m_defaultHeaderHeight;
		if(m_header.get())
		{
			if(m_header->GetFixedHeight() == 0)
			{
				m_header->SetFixedHeight((int)m_defaultHeaderHeight);
			}
			headerHeight = m_header->GetFixedHeight();
		}
		if(m_bExpand)
		{
			// Determine the minimum size
			for( int it1 = 1; it1 < m_items.GetSize(); it1++ ) { //skip header
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
				if( !pControl->IsVisible() ) continue;
				if( pControl->IsFloat() ) continue;
				SIZE sz = pControl->EstimateSize(szAvailable);
				if( sz.cy == 0 ) {
					sz.cy = (LONG)m_itemHeight;
				}
				else {
					if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
					if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
				}
				cyFixed += sz.cy + pControl->GetPadding().top + pControl->GetPadding().bottom;
				nEstimateNum++;
			}
			cyFixed += (nEstimateNum - 1) * m_iChildPadding;
			cyFixed += headerHeight;
			m_cxyFixed.cy = (LONG)cyFixed + m_rcInset.top + m_rcInset.bottom;
		}
		else
		{
			m_cxyFixed.cy = (LONG)headerHeight + m_rcInset.top + m_rcInset.bottom;
		}
		return m_cxyFixed;
	}

	void CGroupUI::Message(TEventUI* event_, InterNotifyMsg what)
	{
		if(m_pParent)
		{
			IInterMessage* pInter = static_cast<IInterMessage*>(m_pParent->GetInterface(INTERFACE_INTERMSG));
			if(pInter)
				pInter->Message(event_,what);
		}
	}

	bool CGroupUI::Add(CControlUI* pControl)
	{
		if(pControl == NULL) return false;
		if( _tcscmp(pControl->GetClass(), TEXT("GroupHeaderUI") ) == 0 )
		{
			std::unique_ptr<CGroupHeaderUI> header(static_cast<CGroupHeaderUI*>(pControl));
			m_header = std::move(header);
			AttachEvent(pControl,this,(EventFunc)&CGroupUI::OnChildEvent);
		}
		else if(_tcscmp(pControl->GetClass(), TEXT("LayerUI") ) == 0)
		{
			CLayerUI* pLayer = (CLayerUI*)(pControl->GetInterface(DUI_CTR_LAYERITEM));
			pLayer->m_LayerInfo = m_LayerInfo;
			AttachEvent(pControl,pLayer,(EventFunc)&CLayerUI::OnChildEvent,true);
		}

		return __super::Add(pControl);
	}

	void CGroupUI::RemoveAll()
	{
		for( int it = 1 /*ignore GroupHeaderUI*/; m_bAutoDestroy && it < m_items.GetSize(); it++ ) {
			if( m_bDelayedDestroy && m_pManager ) m_pManager->AddDelayedCleanup(static_cast<CControlUI*>(m_items[it]));             
			else delete static_cast<CControlUI*>(m_items[it]);
		}
		m_items.Empty();
		NeedUpdate();
	}

	void CGroupUI::DoEvent(TEventUI& event_)
	{
		if (!IsMouseEnabled() && event_.Type > UIEVENT__MOUSEBEGIN && event_.Type < UIEVENT__MOUSEEND)
		{
			if (m_pParent != NULL) m_pParent->DoEvent(event_);
			else CContainerUI::DoEvent(event_);
			return;
		}
		if (event_.Type == UIEVENT_SETFOCUS)
		{
			//::OutputDebugString(_T("UIEVENT_SETFOCUS--"));
			//::OutputDebugString(this->GetText().GetData());
			//::OutputDebugString(_T("\n"));
		}
		if (event_.Type == UIEVENT_KILLFOCUS)
		{
			//::OutputDebugString(_T("UIEVENT_KILLFOCUS--"));
			//::OutputDebugString(this->GetText().GetData());
			//::OutputDebugString(_T("\n"));
		}
		else if (event_.Type == UIEVENT_SETCURSOR)
		{
			::OutputDebugString(_T("UIEVENT_SETCURSOR--\n"));
			//::SetCursor(::LoadCursor(NULL,IDC_IBEAM));
		}
		if (event_.Type == UIEVENT_MOUSEHOVER)
		{
			//::OutputDebugString(_T("UIEVENT_MOUSEHOVER--"));
			//::OutputDebugString(this->GetText().GetData());
			//::OutputDebugString(_T("\n"));
		}
		if (event_.Type == UIEVENT_BUTTONDOWN)
		{
			::OutputDebugString(_T("UIEVENT_BUTTONDOWN--"));
			::OutputDebugString(this->GetText().GetData());
			::OutputDebugString(_T("\n"));

			if(event_.ptMouse.x < m_rcItem.left+30)
			{
				event_.pSender = m_header.get();
				Message(&event_,LayerItemDrag);
			}
			else
			{
				SetExpand(!m_bExpand);
				Invalidate();
				this->NeedParentUpdate();
				Message(0,NeedLayoutUpdate);
			}
			m_bButtonDown = true;
			return;
		}
		if (event_.Type == UIEVENT_DBLCLICK)
		{
			//::OutputDebugString(_T("UIEVENT_DBLCLICK--"));
			//::OutputDebugString(this->GetText().GetData());
			//::OutputDebugString(_T("\n"));
			return;
		}
		if (event_.Type == UIEVENT_MOUSEMOVE)
		{
			if(m_bButtonDown)
			{
				Message(&event_,LayerItemMove);
			}
			return;
		}
		if (event_.Type == UIEVENT_BUTTONUP)
		{
			//::OutputDebugString(_T("UIEVENT_BUTTONUP--"));
			//::OutputDebugString(this->GetText().GetData());
			//::OutputDebugString(_T("\n"));
			if(m_bButtonDown)
			{
				Message(&event_,LayerItemDrop);
				m_bButtonDown = false;
			}
			return;
		}
		if (event_.Type == UIEVENT_CONTEXTMENU)
		{
			if (IsContextMenuUsed()) {
				m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event_.wParam, event_.lParam);
			}
			return;
		}
		if (event_.Type == UIEVENT_MOUSEENTER)
		{
			//::OutputDebugString(_T("UIEVENT_MOUSEENTER--"));
			//::OutputDebugString(this->GetText().GetData());
			//::OutputDebugString(_T("\n"));
		}
		if (event_.Type == UIEVENT_MOUSELEAVE)
		{
			//::OutputDebugString(_T("UIEVENT_MOUSELEAVE--"));
			//::OutputDebugString(this->GetText().GetData());
			//::OutputDebugString(_T("\n"));
		}
		CContainerUI::DoEvent(event_);
	}

	bool CGroupUI::OnChildEvent(void* event_in)
	{
		TEventUI* pEvent = static_cast<TEventUI*>(event_in);
		if(!pEvent) return false;
		if (pEvent->Type == UIEVENT_BUTTONDOWN)
		{
			pEvent->pSender = m_header.get();
			Message(pEvent,LayerItemSelect);
		}
		return true;
	}

	void CGroupUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("hscrollbar")) == 0 || _tcscmp(pstrName, _T("vscrollbar")) == 0)
		{
			return ; //we don't want scrollbar
		}
		else if(_tcscmp(pstrName,_T("height"))==0)
		{
			return ; //auto height
		}
		else if (_tcscmp(pstrName, _T("iteminset")) == 0)
		{
			RECT rcInset = { 0 };
			LPTSTR pstr = NULL;
			rcInset.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcInset.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcInset.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcInset.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetItemInset(rcInset);
			SetItemAttribute(pstrName,pstrValue);
		}
		__super::SetAttribute(pstrName, pstrValue);
	}

	void CGroupUI::SetItemAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
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

	void CGroupUI::SetPos(RECT rc)
	{
		CRect rcItem = rc;
		CControlUI::SetPos(rcItem);
		rc = m_rcItem;
		rcItem.Deflate(&m_rcInset);// Adjust for inset

		SIZE szAvailable = { rcItem.right - rcItem.left, rcItem.bottom - rcItem.top };

		// Place elements
		int cyNeeded = 0;
		int cyExpand = 0;
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosY = rc.top;
		int iPosX = rc.left;
		int cyFixedRemaining = szRemaining.cy/*cyFixed*/;
		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it2);
				continue;
			}

			RECT rcPadding = pControl->GetPadding();
			szRemaining.cy -= rcPadding.top;
			SIZE sz = pControl->EstimateSize(szRemaining);
			if( sz.cy == 0 ) {
				sz.cy = (LONG)m_itemHeight;
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			}
			else {
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
				cyFixedRemaining -= sz.cy;
			}

			sz.cx = pControl->GetFixedWidth();
			if( sz.cx == 0 ) sz.cx = szAvailable.cx - rcPadding.left - rcPadding.right;
			if( sz.cx < 0 ) sz.cx = 0;
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

			CRect rcCtrl = CRect( iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top + rcPadding.bottom );
			if( it2 )
			{
				rcCtrl.Deflate(&m_itemInset);
				if(!m_bExpand)
					rcCtrl.Empty();
			}
			pControl->SetPos(rcCtrl);

			iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
			cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
			szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
		}
	}

	void CGroupUI::SetItemInset(RECT rc)
	{
		m_itemInset = rc;
	}

	CRect CGroupUI::GetItemInset()
	{
		return m_itemInset;
	}

	bool CGroupUI::SetExpand(bool bExpand)
	{
		m_bExpand = bExpand ? 1 : 0;
		return bExpand;
	}

	bool CGroupUI::GetExpand()
	{
		return m_bExpand == 1;
	}

	size_t CGroupUI::SetHeaderHeight(size_t nHeight)
	{
		if(m_header.get())
		{
			m_header->SetFixedHeight((int)nHeight);
			return nHeight;
		}
		return 0;
	}

	size_t CGroupUI::GetHeaderHeight()
	{
		if(m_header.get())
		{
			return (size_t)m_header->GetFixedHeight();
		}
		return 0;
	}

	size_t CGroupUI::SetItemHeight(size_t nHeight)
	{
		m_itemHeight = nHeight;
		return m_itemHeight;
	}

	size_t CGroupUI::GetItemHeight()
	{
		return m_itemHeight;
	}

	CGroupHeaderUI* CGroupUI::GetHeaderUI()
	{
		return m_header.get();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//CGroupHeaderUI

	CGroupHeaderUI::CGroupHeaderUI()
	{
		m_bDelayedDestroy = false;
	}

	LPCTSTR CGroupHeaderUI::GetClass() const
	{
		return _T("GroupHeaderUI");
	}

	LPVOID CGroupHeaderUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_GROUPITEM) == 0 ) return static_cast<CGroupHeaderUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	void CGroupHeaderUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("hscrollbar")) == 0 || _tcscmp(pstrName, _T("vscrollbar")) == 0)
		{
			return ; //we don't want scrollbar
		}
		__super::SetAttribute(pstrName, pstrValue);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//CLayerLayoutUI

	CLayerLayoutUI::CLayerLayoutUI() : m_bVertical(true), m_uButtonState(0), 
		m_bImmMode(false),m_pSelControl(NULL),m_pHotControl(NULL)
	{
		m_LayerInfo = std::make_shared<_tagLayerInfo>();
		ptLastMouse.x = ptLastMouse.y = 0;
		::ZeroMemory(&m_rcNewPos, sizeof(m_rcNewPos));
	}

	LPCTSTR CLayerLayoutUI::GetClass() const
	{
		return _T("LayerListUI");
	}

	LPVOID CLayerLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_LAYERLIST) == 0 ) return static_cast<CLayerLayoutUI*>(this);
		if( _tcscmp(pstrName, INTERFACE_INTERMSG) == 0 ) return static_cast<IInterMessage*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	void CLayerLayoutUI::Message(TEventUI* event_, InterNotifyMsg what)
	{
		// 处理所有的子控件内部消息
		switch(what)
		{
		case NeedLayoutUpdate: 
			this->Invalidate();
			this->NeedParentUpdate();
			break;
		case LayerItemDrag:
			MakeCursorImage(m_pManager->GetPaintDC(),event_->pSender->GetPos(),event_->ptMouse);
			m_hCursor.ptMove = event_->ptMouse;
			m_hCursor.bDrag = 1;
			break;
		case LayerItemMove: 
			if(!::PtInRect(&m_rcItem,event_->ptMouse))
			{
				::SetCursor(::LoadCursor(NULL,IDC_NO));
			}
			else
			{
				if(m_hCursor.bDrag)
				{
					m_hCursor.ptMove = event_->ptMouse;
				}
				//if(m_hCursor.hCursor)
				//	::SetCursor(m_hCursor.hCursor);
				//else
					::SetCursor(::LoadCursor(NULL,IDC_ARROW));
				Invalidate();
			}
			break;
		case LayerItemDrop:
			if(m_hCursor.pBitmap)
			{
				m_hCursor.Release();
			}
			m_hCursor.bDrag = 0;
			for(auto iter = m_funcDrop.begin();iter != m_funcDrop.end(); ++iter)
			{
				(*iter)();
			}
			Invalidate();
			break;
		case LayerItemSelect:
			//DUI__Trace(L"name of control = %s\n",event_->pSender->GetName().GetData());
			m_pSelControl = event_->pSender;
			Invalidate();
			break;
		}

	}

	bool CLayerLayoutUI::Add(CControlUI* pControl)
	{
		if(pControl == NULL) return false;
		if(_tcscmp(pControl->GetClass(), TEXT("LayerUI") ) == 0)
		{
			CLayerUI* pLayer = (CLayerUI*)(pControl->GetInterface(DUI_CTR_LAYERITEM));
			ShareInfo(pControl,m_LayerInfo);
			AttachEvent(pControl,pLayer,(EventFunc)&CLayerUI::OnChildEvent,true);
		}
		else	if(_tcscmp(pControl->GetClass(), TEXT("GroupUI") ) == 0)
		{
			ShareInfo(pControl,m_LayerInfo);
		}
		return __super::Add(pControl);
	}

	/*void CLayerLayoutUI::SetSelImage(LPCTSTR pstrImage)
	{
	m_strSelImg = pstrImage;
	}

	CUIString CLayerLayoutUI::GetSelImage()
	{
	return m_strSelImg;
	}

	void CLayerLayoutUI::SetHotImage(LPCTSTR pstrImage)
	{
	m_strHotImg = pstrImage;
	}

	CUIString CLayerLayoutUI::GetHotImage()
	{
	return m_strHotImg;
	}*/

	void CLayerLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
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
		else if( _tcscmp(pstrName, _T("vermode")) == 0 )
			(_tcscmp(pstrValue, _T("true")) == 0);
		//else if( _tcscmp(pstrName, _T("selimage")) == 0 )
		//	SetSelImage(pstrValue);
		//else if( _tcscmp(pstrName, _T("hotimage")) == 0 )
		//	SetHotImage(pstrValue);
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

	void CLayerLayoutUI::SetItemAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
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

	void CLayerLayoutUI::DoEvent(TEventUI& event_)
	{
		if (event_.Type == UIEVENT_BUTTONDOWN && IsEnabled())
		{
			int index = 0;
			if(PtHitControl(event_.ptMouse,index) != NULL)
			{
				SelectItem(index);
			}
		}
		if (event_.Type == UIEVENT_WINDOWSIZE)
		{
			//EnsureVisible(m_iCurSel);
		}
		else if (event_.Type == UIEVENT_MOUSEMOVE)
		{
			int index = 0;
			CControlUI* pControl = PtHitControl(event_.ptMouse,index);
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
		else if (event_.Type == UIEVENT_MOUSELEAVE)
		{
			m_pHotControl = NULL;
			this->Invalidate();
		}
		if (!IsMouseEnabled() && event_.Type > UIEVENT__MOUSEBEGIN && event_.Type < UIEVENT__MOUSEEND)
		{
			if (m_pParent != NULL) m_pParent->DoEvent(event_);
			else CControlUI::DoEvent(event_);
			return;
		}

		if (event_.Type == UIEVENT_SETFOCUS)
		{
			m_bFocused = true;
			return;
		}
		if (event_.Type == UIEVENT_KILLFOCUS)
		{
			m_bFocused = false;
			return;
		}
		if (m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsEnabled())
		{
			if (event_.Type == UIEVENT_KEYDOWN)
			{
				switch (event_.chKey)
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
			else if (event_.Type == UIEVENT_SCROLLWHEEL)
			{
				switch (LOWORD(event_.wParam))
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
			if (event_.Type == UIEVENT_KEYDOWN)
			{
				switch (event_.chKey)
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
			else if (event_.Type == UIEVENT_SCROLLWHEEL)
			{
				switch (LOWORD(event_.wParam))
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

		CControlUI::DoEvent(event_);
	}

	bool CLayerLayoutUI::OnChildEvent(void* event_in)
	{
		return true;
	}

	void CLayerLayoutUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		CContainerUI::DoPaint(hDC,rcPaint);
		PaintMoveItem(hDC);
	}

	void CLayerLayoutUI::PaintBkColor(HDC hDC)
	{
		__super::PaintBkColor(hDC);
		if(m_LayerInfo.get())
		{
			//paint selected layer or group
			if(m_LayerInfo->dwItemSelectedBkColor && m_pSelControl != NULL)
			{
				RECT rcItem = m_pSelControl->GetPos();
				CRenderEngine::DrawColor(hDC, rcItem, m_LayerInfo->dwItemSelectedBkColor);
			}
			if(m_LayerInfo->dwItemHotBkColor && m_pHotControl != NULL)
			{
				RECT rcItem = m_pHotControl->GetPos();
				CRenderEngine::DrawColor(hDC, rcItem, m_LayerInfo->dwItemHotBkColor);
			}
		}
	}

	void CLayerLayoutUI::PaintBkImage(HDC hDC)
	{
		__super::PaintBkColor(hDC);
	}

	void CLayerLayoutUI::PaintStatusImage(HDC hDC)
	{
		__super::PaintBkColor(hDC);
		if(m_LayerInfo.get())
		{
			//paint selected layer or group
			if(!m_LayerInfo->sItemSelectedImage.IsEmpty() && m_pSelControl != NULL)
			{
				RECT rcItem = m_pSelControl->GetPos();
				if (!DrawImage(hDC, rcItem, (LPCTSTR)m_LayerInfo->sItemSelectedImage))
					m_LayerInfo->sItemSelectedImage.Empty();
			}
			if(!m_LayerInfo->sItemHotImage.IsEmpty() && m_pHotControl != NULL)
			{
			RECT rcItem = m_pHotControl->GetPos();
			if (!DrawImage(hDC, rcItem, (LPCTSTR)m_LayerInfo->sItemHotImage))
				m_LayerInfo->sItemHotImage.Empty();
			}
		}

	}

	void CLayerLayoutUI::PaintText(HDC hDC)
	{
		__super::PaintBkColor(hDC);
	}

	void CLayerLayoutUI::PaintBorder(HDC hDC)
	{
		__super::PaintBkColor(hDC);
	}

	void CLayerLayoutUI::PaintMoveLine(HDC hDC)
	{}

	void CLayerLayoutUI::PaintMoveItem(HDC hDC)
	{
		if(m_hCursor.bDrag)
		{
			m_hCursor.ptMove.x;
			Graphics g(hDC);
			g.DrawImage(m_hCursor.pBitmap,m_hCursor.ptOffset.x,m_hCursor.ptMove.y-m_hCursor.ptOffset.y);
		}
	}

	bool CLayerLayoutUI::DrawImage(HDC hDC, const RECT &rcItem, LPCTSTR pStrImage, LPCTSTR pStrModify)
	{
		return CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, pStrImage, pStrModify);
	}

	CControlUI* CLayerLayoutUI::GetCurSel()
	{
		return m_pSelControl;
	}

	bool CLayerLayoutUI::SelectItem(int nIndex, bool bTakeFocus)
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

	CControlUI* CLayerLayoutUI::PtHitControl(POINT ptMouse, int &nIndex)
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

	void CLayerLayoutUI::ClearAll()
	{
		this->RemoveAll();
		m_pHotControl = NULL;
		m_pSelControl = NULL;
		m_iCurSel = -1;
	}

	bool CLayerLayoutUI::CheckRectInvalid(RECT &rc)
	{
		if(rc.right < rc.left || rc.bottom < rc.top)
			return true;
		return false;
	}	

	void CLayerLayoutUI::EnsureVisible(int nIndex)
	{
		if(!m_bEnsureVisible || m_iCurSel < 0) return;
		RECT rcItem = GetItemAt(nIndex)->GetPos();
		RECT rcList = this->GetPos();
		RECT rcListInset = this->GetInset();

		rcList.left += rcListInset.left;
		rcList.top += rcListInset.top;
		rcList.right -= rcListInset.right;
		rcList.bottom -= rcListInset.bottom;

		if(m_bVertical)
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

	bool CLayerLayoutUI::MakeCursorImage(HDC hDC,const CRect rcItem, const CPoint pt, int alpha)
	{
		auto GetEncoderClsid=[](LPCTSTR format, CLSID* pClsid)->int
		{
			UINT num= 0;
			UINT size= 0;

			ImageCodecInfo* pImageCodecInfo= NULL;

			GetImageEncodersSize(&num, &size);
			if(size== 0)
			{
				return -1;
			}
			pImageCodecInfo= (ImageCodecInfo*)(malloc(size));
			if(pImageCodecInfo== NULL)
			{
				return -1;
			}

			GetImageEncoders(num, size, pImageCodecInfo);

			for(UINT j=0; j< num; ++j)
			{
				if(wcscmp(pImageCodecInfo[j].MimeType, format)== 0)
				{
					*pClsid= pImageCodecInfo[j].Clsid;
					free(pImageCodecInfo);
					return j;
				}
			}

			free(pImageCodecInfo);
			return -1;
		};
		if(m_hCursor.pBitmap) m_hCursor.Release();
		if(!m_hCursor.pBitmap)
		{
			m_hCursor.ptOffset.x = rcItem.left;
			m_hCursor.ptOffset.y = pt.y-rcItem.top;
			ICONINFO   ii = {FALSE,pt.x-rcItem.left,pt.y-rcItem.top,0,0};  
			Color clear(0,0,0,0);
			m_hCursor.pBitmap = new Bitmap( rcItem.GetWidth()+32,rcItem.GetHeight()+32 , PixelFormat32bppARGB );
			HDC hmemDC = CreateCompatibleDC(hDC);   
			HBITMAP hBmp = CreateCompatibleBitmap(hDC, rcItem.GetWidth(),rcItem.GetHeight());  
			::SelectObject(hmemDC, hBmp);  
			::BitBlt(hmemDC, 0, 0,  rcItem.GetWidth(), rcItem.GetHeight(),  
				hDC, rcItem.left, rcItem.top, SRCCOPY);

			Bitmap bitmap(hBmp,NULL);
			Graphics g(m_hCursor.pBitmap);
			g.Clear(clear);
			PixelFormat pf = m_hCursor.pBitmap->GetPixelFormat();
			REAL m[][5] = {{1,0,0,0,0}, {0,1,0,0,0}, {0,0,1,0,0}, {0,0,0,(float)alpha/255,0}, {0,0,0,0,1} };  
			ImageAttributes imageAtt;  
			imageAtt.SetColorMatrix((ColorMatrix*)&m, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap); 

			Rect rc(0, 0, rcItem.GetWidth(),rcItem.GetHeight());
			g.DrawImage(&bitmap, rc, 0, 0, rcItem.GetWidth(), rcItem.GetHeight(), UnitPixel, &imageAtt);
	
			//{
			//	ICONINFO ci = {0};
			//	HCURSOR drawCur = ::GetCursor();
			//	::GetIconInfo(drawCur,&ci);
			//	//::DrawIcon(hmemDC,0,0,drawCur);
			//	Bitmap curBitmap(ci.hbmColor,NULL);
			//	Rect rcc(ii.xHotspot, ii.yHotspot, 32,32);
			//	g.DrawImage(&curBitmap, rcc, 0, 0, rcc.Width, rcc.Height, UnitPixel);
			//	if(drawCur) ::DeleteObject(drawCur);
			//}

			//CLSID encoderClsid;
			//GetEncoderClsid(L"image/png",&encoderClsid);
			//m_hCursor.pBitmap->Save(L"test.png",&encoderClsid);
			m_hCursor.pBitmap->GetHBITMAP(clear,&m_hCursor.hBitmap);
			ii.hbmMask=m_hCursor.hBitmap;
			ii.hbmColor=m_hCursor.hBitmap; 
			m_hCursor.hCursor=(HCURSOR)CreateIconIndirect(&ii);
			::SelectObject(hmemDC,(HBITMAP)NULL);
			::DeleteObject(hmemDC);
			::DeleteObject(hBmp);
		}
		return 0;
	}

}//namespace DirectUI



