#include "stdafx.h"
#include "LayerLayoutUI.h"
//#include <hash_set>

#define UI(ctr) ctr##UI 
const TCHAR*  DUI_CTR_LAYERLAYOUT = _T("LayerLayout");
const TCHAR*  DUI_CTR_LAYERITEM = _T("Layer");
const TCHAR*  DUI_CTR_GROUPITEM = _T("Group");
const TCHAR*  DUI_CTR_GROUPHEADER= _T("GroupHeader");
const TCHAR*  INTERFACE_INTERMSG  =  _T("IInterMessage");

namespace DirectUI
{
	using namespace Inter;

	void PrintZ(CLayerUI* pLayer) {
		if (pLayer) {
			DUI__Trace(_T("name %s - Z %u\n"), pLayer->GetName().GetData(), pLayer->GetZ());
		}
	}

	bool CheckGroup(CGroupUI* pGroup)
	{
		if (pGroup)
		{
			int size = pGroup->GetCount();
			for (int index = 1;index < size;index++)
			{
				if (CheckGroup((CGroupUI*)pGroup->GetItemAt(index)->GetInterface(DUI_CTR_GROUPITEM))) continue;
				PrintZ((CLayerUI*)pGroup->GetItemAt(index)->GetInterface(DUI_CTR_LAYERITEM));
			}
			return true;
		}
		return false;
	}

	void PrintLayerZ(CLayerLayoutUI* pLayout)
	{
		int sizeOfLayout = pLayout->GetCount();
		for (int index = 0;index < sizeOfLayout;index++)
		{
			if(CheckGroup((CGroupUI*)pLayout->GetItemAt(index)->GetInterface(DUI_CTR_GROUPITEM))) continue;
			PrintZ((CLayerUI*)pLayout->GetItemAt(index)->GetInterface(DUI_CTR_LAYERITEM));
		}
	}

	static CContainerUI* GetParentLayout(CControlUI* p)
	{
		if(!p) return nullptr;
		CControlUI* pcon = p->GetParent();
		if (pcon)  return(CContainerUI*)(pcon->GetInterface(DUI_CTR_CONTAINER));
		return nullptr;
	}

	static std::pair<CControlUI *, CControlUI *> GetNeighbor(CControlUI* p)
	{
		CControlUI* p1 = nullptr;
		CControlUI* p2 = nullptr;
		CGroupUI* pGroup = nullptr;
		CContainerUI* pLayout = GetParentLayout(p);
		int index = pLayout->GetItemIndex(p);
		int size = pLayout->GetCount() - 1;
		if (pLayout)
		{
			//neighbor top
			if (index > 0) {
				p1 = pLayout->GetItemAt(index - 1);
				pGroup = (CGroupUI*)(p1->GetInterface(DUI_CTR_GROUPITEM));
				if (pGroup)
				{
					p1 = pGroup->GetHeaderUI();
				}
			}
			//neighbor bottom
			if (index < size)
			{
				p2 = pLayout->GetItemAt(index + 1);
				pGroup = (CGroupUI*)(p2->GetInterface(DUI_CTR_GROUPITEM));
				if (pGroup)
				{
					p2 = pGroup->GetHeaderUI();
				}
			}
		}

		return std::make_pair(p1, p2);
	}

	static CLayerUI* GetLayerFormContainer(CContainerUI* pCon, BOOL bTop /*top or tail*/)
	{
		if (!pCon) return nullptr;
		if (pCon->GetCount() == 0) return nullptr;
		auto index = bTop ? 0 : pCon->GetCount() - 1;
		while (true)
		{
			auto pCtl = pCon->GetItemAt(index);
			if (pCtl)
			{
				if (pCtl->GetInterface(DUI_CTR_LAYERITEM)) return static_cast<CLayerUI*>(pCtl);
				else if (pCtl->GetInterface(DUI_CTR_GROUPITEM))
				{
					auto pRet = GetLayerFormContainer(static_cast<CContainerUI*>(pCtl), bTop);
					if (pRet) return pRet; //找到返回,没找到继续
				}
			}
			else
			{
				break;
			}
			bTop ? index++ : index--;
		}
		return nullptr;
	}

	static CLayerUI* GetNeighborLayer(CControlUI* p, BOOL bTop /*top or tail*/)
	{
		CContainerUI* pCon = GetParentLayout(p);
		if (!pCon) return nullptr;
		if (pCon->GetCount() == 0) return nullptr;
		auto indexItem = pCon->GetItemIndex(p);
		auto index = bTop ? indexItem - 1 : indexItem + 1;
		while (true)
		{
			auto pCtl = pCon->GetItemAt(index);
			if (pCtl)
			{
				if (pCtl->GetInterface(DUI_CTR_LAYERITEM)) return static_cast<CLayerUI*>(pCtl);
				else if (pCtl->GetInterface(DUI_CTR_GROUPITEM))
				{
					auto pRet = GetLayerFormContainer(dynamic_cast<CContainerUI*>(pCtl), FALSE);
					if (pRet) return pRet; //找到返回,没找到继续
				}
			}
			else
			{
				break;
			}
			bTop ? index-- : index++;
		}
		return nullptr;
	}

	static void MakeLayers(CGroupUI* pGroup, std::vector<CLayerUI*>& layers)
	{
		if (!pGroup) return;
		int size = pGroup->GetCount();
		int index = 1;
		for (index;index < size;index++)
		{
			auto pctl = pGroup->GetItemAt(index);
			if (!pctl) continue;
			if (pctl->GetInterface(DUI_CTR_LAYERITEM))
			{
				layers.push_back((CLayerUI*)pctl);
			}
			else if (pctl->GetInterface(DUI_CTR_GROUPITEM))
			{
				MakeLayers((CGroupUI*)pctl,layers);
			}
		}
	}

	void Inter::IInterMessage::AttachEvent(CControlUI* pControl, IInterMessage* pthis, EventFunc pfunc, bool self )
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

	void Inter::IInterMessage::ShareInfo(CControlUI* pControl, std::shared_ptr<_tagLayerInfo>& info)
	{
		if(!pControl || !info.get()) return;
		if(pControl->GetInterface(DUI_CTR_GROUPITEM))
		{
			CGroupUI* pGroup = (CGroupUI*)(pControl);
			pGroup->m_spLayerInfo = info;
			for(int it=0;it<pGroup->GetCount();it++)
			{
				ShareInfo(pGroup->GetItemAt(it),info);
			}
		}
		else if(pControl->GetInterface(DUI_CTR_LAYERITEM))
		{
			CLayerUI* pLayer = (CLayerUI*)(pControl);
			pLayer->m_spLayerInfo = info;
		}
	}

	void Inter::IInterMessage::CheckMapControl(CControlUI * pControl)
	{
		if (!m_spLayerInfo.get()) return;
		if (pControl->GetInterface(DUI_CTR_LAYERITEM) 
			|| pControl->GetInterface(DUI_CTR_GROUPHEADER))
		{
			ControlPosInfo& info = m_spLayerInfo->mapControl[pControl];
			info.rcFit = pControl->GetPos();
		}
	}

	void Inter::IInterMessage::RemoveMapControl(CControlUI* pControl)
	{
		if(!m_spLayerInfo.get() || pControl == NULL) return;
		if(pControl->GetInterface(DUI_CTR_LAYERLAYOUT) || pControl->GetInterface(DUI_CTR_GROUPITEM))
		{
			m_spLayerInfo->mapControl.clear();
		}
		else
		{
			m_spLayerInfo->mapControl.erase(pControl);
		}
	}

	void Inter::IInterMessage::UpdateToVecLayers(CControlUI* pControl, int opt, UINT placement)
	{
		//check pControl had Manager
		if(m_spLayerInfo.get() && pControl->GetManager())
		{
			UINT nBegin = 0,nEnd = UINT_MAX;
			auto pLayer = (CLayerUI*)nullptr;
			auto &vecLayers = m_spLayerInfo->vecLayers;
			if (LO_Add == opt || LO_AddAt == opt) //add
			{
				pLayer = GetNeighborLayer(pControl, TRUE);
				if (pLayer)
				{
					nBegin = pLayer->GetZ();
					if (pControl->GetInterface(DUI_CTR_LAYERITEM))
					{
						if (nBegin == vecLayers.size() - 1)
						{
							vecLayers.push_back((CLayerUI*)pControl);
							nBegin = nBegin + 1;
							nEnd = vecLayers.size() - 1;
						}
						else if (nBegin < vecLayers.size())
						{
							vecLayers.insert(vecLayers.begin() + nBegin + 1, (CLayerUI*)pControl);
							nBegin = nBegin + 1;
							nEnd = vecLayers.size() - 1;
						}
					}
					else if (pControl->GetInterface(DUI_CTR_GROUPITEM))
					{
						std::vector<CLayerUI*> layers;
						MakeLayers((CGroupUI*)pControl, layers);
						if (layers.size())
						{
							vecLayers.insert(vecLayers.begin() + nBegin + 1,
								layers.begin(), layers.end());
							nBegin = nBegin + 1;
							nEnd = vecLayers.size() - 1;
						}
					}
				}
				else //add to first
				{
					if (pControl->GetInterface(DUI_CTR_LAYERITEM))
					{
						vecLayers.insert(vecLayers.begin(), (CLayerUI*)pControl);
						nEnd = vecLayers.size() - 1;
					}
					else if (pControl->GetInterface(DUI_CTR_GROUPITEM))
					{
						std::vector<CLayerUI*> layers;
						MakeLayers((CGroupUI*)pControl, layers);
						if (layers.size())
						{
							vecLayers.insert(vecLayers.begin(),layers.begin(), layers.end());
							nEnd = vecLayers.size() - 1;
						}
					}
				}

			}
			else if(LO_Remove == opt) //insert
			{
				if (pControl->GetInterface(DUI_CTR_LAYERITEM))
				{
					pLayer = ((CLayerUI*)pControl);
					nBegin = pLayer->GetZ();
					if (nBegin < vecLayers.size())
					{
						vecLayers.erase(vecLayers.begin() + nBegin);
						nEnd = vecLayers.size() - 1;
					}
				}
				else if (pControl->GetInterface(DUI_CTR_GROUPITEM))
				{
					pLayer = GetLayerFormContainer((CGroupUI*)pControl,TRUE);
					if (pLayer)
					{
						nBegin = pLayer->GetZ();
						pLayer = GetLayerFormContainer((CGroupUI*)pControl, FALSE);
						if (pLayer)
						{
							nEnd = pLayer->GetZ();
							if (nEnd < vecLayers.size())
							{
								if (nEnd == nBegin)
									vecLayers.erase(vecLayers.begin() + nBegin);
								else
									vecLayers.erase(vecLayers.begin() + nBegin, vecLayers.begin() + nEnd);
								nEnd = vecLayers.size() - 1;
							}
							else
							{
								nEnd = UINT_MAX;
							}
						}
					}
				}
			}
			else if (LO_Clear == opt) //clear
			{
				vecLayers.clear();
			}

			//update z order
			if (nEnd < vecLayers.size())
			{
				for (auto i = nBegin; i <= nEnd; ++i)
				{
					vecLayers[i]->SetInnerZ(i);
				}
			}
		}
	}

	CLayerUI* Inter::IInterMessage::GetLayer(UINT nZIndex)
	{
		if(m_spLayerInfo.get() && m_spLayerInfo->vecLayers.size() > nZIndex + 1)
		{
			auto item = m_spLayerInfo->vecLayers[nZIndex];
			return item;
		}
		return nullptr;
	}

	void Inter::IInterMessage::CheckLayers()
	{
		if(!m_spLayerInfo.get()) return;
		CContainerUI* p = dynamic_cast<CContainerUI*>(this);
		int size = p->GetCount();
		int index = 0;
		if(p->GetInterface(DUI_CTR_GROUPITEM)) index = 1;
		for (index ;index < size;index++)
		{
			auto pctl = p->GetItemAt(index);
			if (!pctl) continue;
			auto inter = static_cast<IInterMessage*>(pctl->GetInterface(INTERFACE_INTERMSG));
			if (inter) {
				inter->CheckLayers();
				continue;
			}
			if(pctl->GetInterface(DUI_CTR_LAYERITEM))
			{
				//为layer控件时,设置Z,并增加Z序计数
				((CLayerUI*)pctl)->SetInnerZ(m_spLayerInfo->vecLayers.size());
				m_spLayerInfo->vecLayers.push_back((CLayerUI*)pctl);
			}
		}
	}

	void Inter::IInterMessage::UpdateVecLayers()
	{
		if(!m_spLayerInfo.get() || !m_spLayerInfo->pLayout) return;
		m_spLayerInfo->vecLayers.clear();
		m_spLayerInfo->pLayout->CheckLayers();
	}

	template<typename Tfirst,typename Tsecond>
	class TMapFind
	{
	public:
		TMapFind(Tsecond& ts) :ts_(ts) {}
		bool operator()(typename const std::map<Tfirst, Tsecond>::value_type &pair)
		{
			return pair.second == ts_;
		}
	private:
		Tsecond& ts_;
	};
	class MapControlFind
	{
	public:
		MapControlFind(POINT pt, MoveItemInfo& mii) :pt_(pt),mii_(mii) {}
		bool operator()(const std::map<CControlUI*, ControlPosInfo>::value_type &pair)
		{
			do{
				if(mii_.src == pair.first)
				{
					mii_.nLinePos = 0;
					break; //it-self
				}
				if(mii_.src->GetInterface(DUI_CTR_GROUPHEADER)) //Group aceptor
				{
					CGroupUI* pLayout = (CGroupUI*)GetParentLayout(mii_.src);
					if(pLayout)
					{
						RECT rc = pLayout->GetPos();
						rc.bottom+=4;
						if(::PtInRect(&rc,pt_))
						{
							break; //Group-self
						}
					}
				}
				LONG half = ((pair.second.rcFit.bottom - pair.second.rcFit.top)>>1) + pair.second.rcFit.top;
				CContainerUI* pLayout = GetParentLayout(pair.first);
				int index = pLayout->GetItemIndex(pair.first);
				std::pair<CControlUI *,CControlUI *> pairNeighbor = GetNeighbor(mii_.src);//get moving selected item neighbors
				mii_.bGroup = 0;
				//bottom line control
				if (pair.second.rcFit.bottom >= pt_.y && half <= pt_.y)
				{
					if(pair.first->GetInterface(DUI_CTR_GROUPHEADER)) //Group aceptor
					{
						mii_.dst = pair.first;
						mii_.nLinePos = -1;
						mii_.bGroup = 1;
						mii_.gp_dst = (CContainerUI*)pair.first->GetParent();
						return true;
					}
					if(pairNeighbor.first == pair.first)
					{
						break;
					}
					mii_.nLinePos = pair.second.rcFit.bottom + 1;
					mii_.dst = pair.first;
					mii_.gp_dst = pLayout;
					mii_.bTop = 0;
					return true;
				}
				//top line control
				else if (pair.second.rcFit.top <= pt_.y && half >= pt_.y)
				{
					if(pair.first->GetInterface(DUI_CTR_GROUPHEADER)) //Group aceptor
					{
						mii_.dst = pair.first;
						mii_.nLinePos = -1;
						mii_.bGroup = 1;
						mii_.gp_dst = (CContainerUI*)pair.first->GetParent();
						return true;
					}
					if(pairNeighbor.second == pair.first)
					{
						break;
					}
					mii_.nLinePos = pair.second.rcFit.top - 1;
					mii_.dst = pair.first;
					mii_.gp_dst = pLayout;
					mii_.bTop = 1;
					return true;
				}
			}while(false);
			mii_.Clear();
			return false;
		}
	private:
		CPoint pt_;
		MoveItemInfo& mii_;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//CLayerUI

	CLayerUI::CLayerUI()
		:m_bButtonDown(false),m_nZ(UINT_MAX),
		m_bDrag(false)
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
			m_bButtonDown = true;
			m_ptLBDown = event_.ptMouse;
			return;
		}
		if (event_.Type == UIEVENT_MOUSEMOVE)
		{
			if(m_bButtonDown && !m_bDrag && m_spLayerInfo.get())
			{
				CRect rcTest = m_spLayerInfo->rcDragTest;
				rcTest.MoveToXY(m_ptLBDown.x-rcTest.GetWidth()/2,m_ptLBDown.y-rcTest.GetHeight()/2);
				if(!::PtInRect(&rcTest,event_.ptMouse))
				{
					m_bDrag = true;
					event_.pSender = this;
					event_.ptMouse = m_ptLBDown;
					Message(&event_,LayerItemDrag);
				}
			}
			Message(&event_,LayerItemMove);
			return;
		}
		if (event_.Type == UIEVENT_BUTTONUP)
		{
			if (m_bDrag) {
				m_bDrag = false;
				Message(&event_, LayerItemDrop);
			}
			m_bButtonDown = false;
			m_ptLBDown.x = m_ptLBDown.y = 0;
			return;
		}
		CContainerUI::DoEvent(event_);
	}

	bool CLayerUI::OnChildEvent(void* event_in)
	{
		//there also track CLayer Event
		TEventUI* pEvent = static_cast<TEventUI*>(event_in);
		if(!pEvent) return false;
		if (pEvent->Type == UIEVENT_BUTTONUP)
		{
			if (!m_bDrag) { //isn't Draging can select
				pEvent->pSender = this;
				Message(pEvent, LayerItemSelect);
				slot_SelectChange.Active(this);
			}
		}
		return true;
	}

	bool CLayerUI::Add(CControlUI* pControl)
	{
		if(pControl == NULL) return false;
		AttachEvent(pControl,this,(EventFunc)&CLayerUI::OnChildEvent);
		return __super::Add(pControl);
	}

	void CLayerUI::SetInnerZ(UINT z)
	{
		if(m_nZ == z) return;
		m_nZ = z;
		if(UINT_MAX != m_nZ) slot_ZChange.Active(this);
	}

	UINT CLayerUI::GetZ() { return m_nZ; }

	void CLayerUI::SetZ(UINT z)
	{
		if(m_nZ == z) return;
		m_nZ;
	}

	LPVOID CLayerUI::GetPtr(UINT index /*= 0*/) { return m_ptr.GetAt(index); }

	UINT CLayerUI::SetPtr(LPVOID ptr, UINT index/* = 0*/) 
	{
		if(index >= m_ptr.GetSize())
		{
			index = m_ptr.GetSize();
			m_ptr.Add(ptr);
		}
		else
		{
			m_ptr.SetAt(index,ptr); 
		}
		return index;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//CGroupUI

	CGroupUI::CGroupUI()
		:m_itemHeight(40),m_defaultHeaderHeight(40),
		m_bExpand(0),m_bButtonDown(false),m_bDrag(false)
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
			cyFixed += (nEstimateNum) * m_iChildPadding; //with header count in
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
		if( pControl->GetInterface(DUI_CTR_GROUPHEADER))
		{
			return AddAt(pControl,0);
		}
		else if(pControl->GetInterface(DUI_CTR_LAYERITEM))
		{
			CLayerUI* pLayer = static_cast<CLayerUI*>(pControl);
			pLayer->m_spLayerInfo = m_spLayerInfo;
			AttachEvent(pControl,pLayer,(EventFunc)&CLayerUI::OnChildEvent,true);
		}
		else if(pControl->GetInterface(DUI_CTR_GROUPITEM))
		{
			CGroupUI* pGroup = (CGroupUI*)(pControl);
			if(!pGroup->m_spLayerInfo.get()){
				ShareInfo(pGroup,m_spLayerInfo);
			}
		}
		auto ret = __super::Add(pControl);
		if(ret) UpdateToVecLayers(pControl, LayerOpt::LO_Add);
		return ret;
	}

	bool CGroupUI::AddAt(CControlUI* pControl, int nIndex)
	{
		if(pControl == NULL) return false;
		if( pControl->GetInterface(DUI_CTR_GROUPHEADER))
		{
			if(nIndex != 0) return false;
			std::unique_ptr<CGroupHeaderUI> header(static_cast<CGroupHeaderUI*>(pControl));
			m_header = std::move(header);
			AttachEvent(pControl,this,(EventFunc)&CGroupUI::OnChildEvent); // need track childcontrol?
		}
		else if(nIndex == 0)
		{
			return false;
		}
		if(pControl->GetInterface(DUI_CTR_LAYERITEM))
		{
			CLayerUI* pLayer = (CLayerUI*)(pControl);
			pLayer->m_spLayerInfo = m_spLayerInfo;
			//attach layer self event
			AttachEvent(pLayer,pLayer,(EventFunc)&CLayerUI::OnChildEvent,true);
		}
		else if(pControl->GetInterface(DUI_CTR_GROUPITEM))
		{
			CGroupUI* pGroup = (CGroupUI*)(pControl);
			if(!pGroup->m_spLayerInfo.get()){
				ShareInfo(pGroup,m_spLayerInfo);
			}
		}
		auto ret = __super::AddAt(pControl, nIndex);
		if(ret && nIndex != 0) UpdateToVecLayers(pControl, LayerOpt::LO_AddAt, nIndex);
		return ret;
	}

	bool CGroupUI::Remove(CControlUI * pControl)
	{
		if (pControl == NULL || pControl == m_header.get()) return false;
		for (int it = 0; it < m_items.GetSize(); it++)
		{
			if (static_cast<CControlUI*>(m_items[it]) == pControl)
			{
				RemoveMapControl(pControl);
				NeedUpdate();
				UpdateToVecLayers(pControl, LayerOpt::LO_Remove);
				if (m_bAutoDestroy)
				{
					if (m_bDelayedDestroy && m_pManager)
						m_pManager->AddDelayedCleanup(pControl);
					else delete pControl;
				}
				return m_items.Remove(it);
			}
		}

		return false;
	}

	void CGroupUI::RemoveAll()
	{
		UpdateToVecLayers(this, LayerOpt::LO_Remove);
		RemoveMapControl(this);
		for( int it = 1 /*ignore GroupHeaderUI*/; m_bAutoDestroy && it < m_items.GetSize(); it++ ) {
			if( m_bDelayedDestroy && m_pManager ) m_pManager->AddDelayedCleanup(static_cast<CControlUI*>(m_items[it]));             
			else delete static_cast<CControlUI*>(m_items[it]);
		}
		m_items.Empty();
		NeedUpdate();
	}

	bool CGroupUI::RemoveNotDestroy(CControlUI* pControl)
	{
		if (pControl == m_header.get()) return false;
		RemoveMapControl(pControl);
		auto ret = __super::RemoveNotDestroy(pControl);
		if (ret) UpdateToVecLayers(pControl, LayerOpt::LO_Remove);
		return ret;
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
		}
		if (event_.Type == UIEVENT_KILLFOCUS)
		{
		}
		else if (event_.Type == UIEVENT_SETCURSOR)
		{
			//::SetCursor(::LoadCursor(NULL,IDC_IBEAM));
		}
		if (event_.Type == UIEVENT_MOUSEHOVER)
		{
		}
		if (event_.Type == UIEVENT_BUTTONDOWN)
		{
			if(event_.ptMouse.x < m_rcItem.left+30)
			{
				m_bButtonDown = true;
			}
			else
			{
				SetExpand(!m_bExpand);
				Invalidate();
				this->NeedParentUpdate();
				Message(0,NeedLayoutUpdate);
			}
			m_ptLBDown = event_.ptMouse;
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
			if(m_bButtonDown && !m_bDrag && m_spLayerInfo.get())
			{
				CRect rcTest = m_spLayerInfo->rcDragTest;
				rcTest.MoveToXY(m_ptLBDown.x - rcTest.GetWidth() / 2, m_ptLBDown.y - rcTest.GetHeight() / 2);
				if (!::PtInRect(&rcTest, event_.ptMouse))
				{
					m_bDrag = true;
					event_.pSender = m_header.get();
					event_.ptMouse = m_ptLBDown;
					Message(&event_, LayerItemDrag);
				}
			}
			Message(&event_, LayerItemMove);
			return;
		}
		if (event_.Type == UIEVENT_BUTTONUP)
		{
			if (m_bDrag) {
				m_bDrag = false;
				Message(&event_, LayerItemDrop);
			}
			m_bButtonDown = false;
			m_ptLBDown.x = m_ptLBDown.y = 0;
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
		}
		if (event_.Type == UIEVENT_MOUSELEAVE)
		{
		}
		CContainerUI::DoEvent(event_);
	}

	bool CGroupUI::OnChildEvent(void* event_in)
	{
		TEventUI* pEvent = static_cast<TEventUI*>(event_in);
		if(!pEvent) return false;
		if (pEvent->Type == UIEVENT_BUTTONUP)
		{
			if (!m_bDrag) { //isn't Draging can select
				pEvent->pSender = m_header.get();
				Message(pEvent, LayerItemSelect);
			}
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
				if(m_spLayerInfo.get())
				{
					rcCtrl.Deflate(&m_spLayerInfo->rcLayerInset);
				}
				if(!m_bExpand)
					rcCtrl.Empty();
			}
			pControl->SetPos(rcCtrl);
			CheckMapControl(pControl);

			iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
			cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
			szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
		}
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

	void CGroupUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if(!m_spLayerInfo.get()) return;
		CContainerUI::DoPaint(hDC,rcPaint);
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
		if( _tcscmp(pstrName, DUI_CTR_GROUPHEADER) == 0 ) return static_cast<CGroupHeaderUI*>(this);
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

	CLayerLayoutUI::CLayerLayoutUI() :
	m_bMultiSel(false),m_pHotControl(NULL),m_bTrackEvents(true), m_bRmSel(true)
	{
		m_spLayerInfo = std::make_shared<_tagLayerInfo>();
		m_spLayerInfo->pLayout = this;
		m_miInfo.pSelControl = &m_mapSel;
		ptLastMouse.x = ptLastMouse.y = 0;
		::ZeroMemory(&m_rcNewPos, sizeof(m_rcNewPos));
	}

	LPCTSTR CLayerLayoutUI::GetClass() const
	{
		return _T("LayerLayoutUI");
	}

	LPVOID CLayerLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_LAYERLAYOUT) == 0 ) return static_cast<CLayerLayoutUI*>(this);
		if( _tcscmp(pstrName, INTERFACE_INTERMSG) == 0 ) return static_cast<IInterMessage*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	void CLayerLayoutUI::Init()
	{
		UpdateVecLayers();
		__super::Init();
	}

	void CLayerLayoutUI::Message(TEventUI* event_, InterNotifyMsg what)
	{
		// 处理所有的子控件内部消息
		if(!m_bTrackEvents) return;
		switch(what)
		{
		case NeedLayoutUpdate: 
			this->Invalidate();
			this->NeedParentUpdate();
			break;
		case LayerItemDrag:
			MakeCursorImage(m_pManager->GetPaintDC(),GetFitLayoutRc(event_->pSender->GetPos()),event_->ptMouse);
			m_miInfo.src = event_->pSender;
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
					PtRoundControl(event_->ptMouse);
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

			MoveItem();
			Invalidate();
			break;
		case LayerItemSelect:
			DUI__Trace(L"name of control = %s\n",event_->pSender->GetName().GetData());
			if (!(GetKeyState(VK_CONTROL) & 0x8000))
			{
				m_mapSel.clear();
			}
			m_mapSel[event_->pSender] = event_->pSender;
			Invalidate();
			break;
		}

	}

	bool CLayerLayoutUI::OnChildEvent(void* event_in)
	{
		return true;
	}

	//Attributes settings
	//--------------------------------------------------------------------------------------------
	void CLayerLayoutUI::SetMultiSel(bool bFlag)
	{
		m_bMultiSel = bFlag;
	}

	bool CLayerLayoutUI::GetMultiSel() const
	{
		return m_bMultiSel;
	}

	void CLayerLayoutUI::SetTrackEvent(bool bFlag)
	{
		m_bTrackEvents = bFlag;
	}

	bool CLayerLayoutUI::GetTrackEvent() const
	{
		return m_bTrackEvents;
	}

	void CLayerLayoutUI::SetLayerSelImage(LPCTSTR pstrImage)
	{
		if(m_spLayerInfo.get())
			m_spLayerInfo->sItemSelectedImage = pstrImage;
	}

	CUIString CLayerLayoutUI::GetLayerSelImage()
	{
		if(!m_spLayerInfo.get()) return CUIString();
		return CUIString(m_spLayerInfo->sItemSelectedImage);
	}

	void CLayerLayoutUI::SetLayerHotImage(LPCTSTR pstrImage)
	{
		if(m_spLayerInfo.get())
			m_spLayerInfo->sItemHotImage = pstrImage;
	}

	CUIString CLayerLayoutUI::GetLayerHotImage()
	{
		if(!m_spLayerInfo.get()) return CUIString();
		return CUIString(m_spLayerInfo->sItemHotImage);
	}

	void CLayerLayoutUI::SetMoveLineColor(DWORD color)
	{
		if (!m_spLayerInfo.get() || m_spLayerInfo->dwItemMoveColor == color) return;
		m_spLayerInfo->dwItemMoveColor = color;
	}

	DWORD CLayerLayoutUI::GetMoveLineColor()
	{
		if(m_spLayerInfo.get())
			return m_spLayerInfo->dwItemMoveColor;
		return 0;
	}

	void CLayerLayoutUI::SetLayerSelColor(DWORD color)
	{
		if (!m_spLayerInfo.get() || m_spLayerInfo->dwItemSelectedBkColor == color) return;
		m_spLayerInfo->dwItemSelectedBkColor = color;
	}

	DWORD CLayerLayoutUI::GetLayerSelColor()
	{
		if(m_spLayerInfo.get())
			return m_spLayerInfo->dwItemSelectedBkColor;
		return 0;
	}

	void CLayerLayoutUI::SetLayerHotColor(DWORD color)
	{
		if (!m_spLayerInfo.get() || m_spLayerInfo->dwItemHotBkColor == color) return;
		m_spLayerInfo->dwItemHotBkColor = color;
	}

	DWORD CLayerLayoutUI::GetLayerHotColor()
	{
		if(m_spLayerInfo.get())
			return m_spLayerInfo->dwItemHotBkColor;
		return 0;
	}

	void CLayerLayoutUI::SetLayerDefaultHeight(int nSize)
	{
		if (!m_spLayerInfo.get() || m_spLayerInfo->nLayerHeight == nSize) return;
		m_spLayerInfo->nLayerHeight = nSize;
	}

	size_t CLayerLayoutUI::GetLayerDefaultHeight()
	{
		if(!m_spLayerInfo.get()) return 0;
		return m_spLayerInfo->nLayerHeight;
	}

	void CLayerLayoutUI::SetGroupDefaultHeight(int nSize)
	{
		if (!m_spLayerInfo.get() || m_spLayerInfo->nGroupHeaderHeight == nSize) return;
		m_spLayerInfo->nGroupHeaderHeight = nSize;
	}

	size_t CLayerLayoutUI::GetGroupDefaultHeight()
	{
		if(!m_spLayerInfo.get()) return 0;
		return m_spLayerInfo->nGroupHeaderHeight;
	}

	void CLayerLayoutUI::SetLayerInset(RECT rc)
	{
		if(m_spLayerInfo.get())
			m_spLayerInfo->rcLayerInset = rc;
	}

	CRect CLayerLayoutUI::GetLayerInset()
	{
		if(!m_spLayerInfo.get()) return CRect();
		return m_spLayerInfo->rcLayerInset;
	}

	void CLayerLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		CContainerUI::SetAttribute(pstrName, pstrValue);
		if( _tcscmp(pstrName, _T("sepwidth")) == 0 ) _ttoi(pstrValue);
		else if (_tcscmp(pstrName, _T("layerheight")) == 0) SetLayerDefaultHeight(_ttoi(pstrValue));
		else if (_tcscmp(pstrName, _T("groupheight")) == 0) SetGroupDefaultHeight(_ttoi(pstrValue));
		if( _tcscmp(pstrName, _T("hscrollbar")) == 0 && _tcscmp(pstrValue, _T("true")) == 0)
		{
			//m_pHorizontalScrollBar->SetVisible(false);
		}
		else if( _tcscmp(pstrName, _T("vscrollbar")) == 0 && _tcscmp(pstrValue, _T("true")) == 0)
		{
			//m_pVerticalScrollBar->SetVisible(false);
		}
		else if( _tcscmp(pstrName, _T("multisel")) == 0 )
			SetMultiSel(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("trackevent")) == 0 )
			SetTrackEvent(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("layerselimage")) == 0 )
			SetLayerSelImage(pstrValue);
		else if( _tcscmp(pstrName, _T("layerhotimage")) == 0 )
			SetLayerHotImage(pstrValue);
		else if (_tcscmp(pstrName, _T("layerselcolor")) == 0)
		{
			while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetLayerSelColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("layerhotcolor")) == 0)
		{
			while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetLayerHotColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("movelinecolor")) == 0)
		{
			while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetMoveLineColor(clrColor);
		}
		else if (_tcscmp(pstrName, _T("layerinset")) == 0)
		{
			RECT rcInset = { 0 };
			LPTSTR pstr = NULL;
			rcInset.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcInset.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcInset.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcInset.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetLayerInset(rcInset);
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

	//Paintting controls
	//--------------------------------------------------------------------------------------------
	void CLayerLayoutUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		CContainerUI::DoPaint(hDC,rcPaint);
		PaintMoveItem(hDC);
	}

	void CLayerLayoutUI::PaintBkColor(HDC hDC)
	{
		__super::PaintBkColor(hDC);
		if(m_spLayerInfo.get())
		{
			//paint selected layer or group
			if(m_spLayerInfo->dwItemSelectedBkColor && m_mapSel.size())
			{
				for(auto iter = m_mapSel.begin(); iter != m_mapSel.end(); ++iter)
				{
					RECT rcItem = iter->first->GetPos();
					int offsetl = max(m_nBorderSize, m_rcInset.left);
					int offsetr = max(m_nBorderSize, m_rcInset.right);
					rcItem.left = m_rcItem.left + offsetl;
					rcItem.right = m_rcItem.right - offsetr;
					if (m_pVerticalScrollBar && !m_pVerticalScrollBar->IsVisible())
					{
						rcItem.right -= m_pVerticalScrollBar->GetFixedWidth();
					}
					CRenderEngine::DrawColor(hDC, rcItem, GetAdjustColor(m_spLayerInfo->dwItemSelectedBkColor));
				}
			}
			if(m_spLayerInfo->dwItemHotBkColor && m_pHotControl != NULL)
			{
				RECT rcItem = m_pHotControl->GetPos();
				CRenderEngine::DrawColor(hDC, rcItem, GetAdjustColor(m_spLayerInfo->dwItemHotBkColor));
			}
		}
	}

	void CLayerLayoutUI::PaintBkImage(HDC hDC)
	{
		__super::PaintBkImage(hDC);
		if (m_pVerticalScrollBar && !m_pVerticalScrollBar->IsVisible())
		{
			CRect rcItem = m_rcItem;
			rcItem.Deflate(&m_rcInset);
			rcItem.left = rcItem.right - m_pVerticalScrollBar->GetFixedWidth();
			CUIString bkImg = m_pVerticalScrollBar->GetBkNormalImage();
			if (!bkImg.IsEmpty()) {
				DrawImage(hDC, rcItem, bkImg);
			}
		}
	}

	void CLayerLayoutUI::PaintStatusImage(HDC hDC)
	{
		__super::PaintStatusImage(hDC);
		if(m_spLayerInfo.get())
		{
			//paint selected layer or group
			if(!m_spLayerInfo->sItemSelectedImage.IsEmpty() && m_mapSel.size())
			{
				for(auto iter = m_mapSel.begin(); iter != m_mapSel.end(); ++iter)
				{
					RECT rcItem = iter->first->GetPos();
					int offsetl = max(m_nBorderSize, m_rcInset.left);
					int offsetr = max(m_nBorderSize, m_rcInset.right);
					rcItem.left = m_rcItem.left + offsetl;
					rcItem.right = m_rcItem.right - offsetr;
					if (m_pVerticalScrollBar && !m_pVerticalScrollBar->IsVisible())
					{
						rcItem.right -= m_pVerticalScrollBar->GetFixedWidth();
					}
					if (!DrawImage(hDC, rcItem, (LPCTSTR)m_spLayerInfo->sItemSelectedImage))
					{
						m_spLayerInfo->sItemSelectedImage.Empty();
						break;
					}
				}
			}
			if(!m_spLayerInfo->sItemHotImage.IsEmpty() && m_pHotControl != NULL)
			{
				RECT rcItem = m_pHotControl->GetPos();
				if (!DrawImage(hDC, rcItem, (LPCTSTR)m_spLayerInfo->sItemHotImage))
					m_spLayerInfo->sItemHotImage.Empty();
			}
		}

	}

	void CLayerLayoutUI::PaintText(HDC hDC)
	{
		__super::PaintText(hDC);
	}

	void CLayerLayoutUI::PaintBorder(HDC hDC)
	{
		__super::PaintBorder(hDC);
	}

	void CLayerLayoutUI::PaintMoveItem(HDC hDC)
	{
		Graphics g(hDC);
		g.SetClip(Rect(m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom));
		if(m_hCursor.bDrag)
		{
			int heigh = (int)m_hCursor.pBitmap->GetHeight();
			POINT pt = { m_hCursor.ptOffset.x, m_hCursor.ptMove.y - m_hCursor.ptOffset.y };
			if (pt.y < m_rcItem.top)
				pt.y = m_rcItem.top;
			else if (pt.y > m_rcItem.bottom - heigh)
				pt.y = m_rcItem.bottom - heigh;
			g.DrawImage(m_hCursor.pBitmap, pt.x, pt.y);
		}
		if (m_miInfo.nLinePos && m_spLayerInfo.get())
		{
			Pen pen(Color(m_spLayerInfo->dwItemMoveColor));
			CRect rcLine = m_rcItem;
			if (m_miInfo.bGroup)
			{
				CRect rc = GetFitLayoutRc(m_miInfo.dst->GetPos());
				rcLine.right = rc.right;
				rcLine.top = rc.top;
				rcLine.bottom = rc.bottom;
			}
			else
			{
				CRect rc = GetFitLayoutRc(rcLine);
				rcLine.right = rc.right;
				rcLine.top = m_miInfo.nLinePos - 1;
				rcLine.bottom = m_miInfo.nLinePos + 1;
			}
			g.DrawLine(&pen, Point(rcLine.left+1, rcLine.top), Point(rcLine.left+1, rcLine.bottom));
			g.DrawLine(&pen, Point(rcLine.left+1, rcLine.top), Point(rcLine.right-1, rcLine.top));
			g.DrawLine(&pen, Point(rcLine.left+1, rcLine.bottom), Point(rcLine.right-1, rcLine.bottom));
			g.DrawLine(&pen, Point(rcLine.right-1, rcLine.top), Point(rcLine.right-1, rcLine.bottom));
		}
	}

	bool CLayerLayoutUI::DrawImage(HDC hDC, const RECT &rcItem, LPCTSTR pStrImage, LPCTSTR pStrModify)
	{
		return CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, pStrImage, pStrModify);
	}

	//Position controls
	//--------------------------------------------------------------------------------------------
	void CLayerLayoutUI::DoEvent(TEventUI& event_)
	{
		if (event_.Type == UIEVENT_BUTTONDOWN && IsEnabled())
		{
			int index = 0;
			if(PtHitControl(event_.ptMouse,index) != NULL)
			{
			}
		}
		if (event_.Type == UIEVENT_WINDOWSIZE)
		{
			//EnsureVisible(m_iCurSel);
		}
		else if (event_.Type == UIEVENT_MOUSEMOVE)
		{
			//int index = 0;
			//CControlUI* pControl = PtHitControl(event_.ptMouse,index);
			//if(pControl && pControl != m_mapSel)
			//{
			//	if(m_pHotControl) m_pHotControl->Invalidate();
			//	m_pHotControl = pControl;
			//	if(m_pHotControl) m_pHotControl->Invalidate();
			//}
			//else
			//{
			//	if(m_pHotControl) m_pHotControl->Invalidate();
			//	m_pHotControl = NULL;
			//}
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

	void CLayerLayoutUI::SetPos(RECT rc)
	{
		//we want Vertical scrollbar show all time
		CControlUI::SetPos(rc);
		rc = m_rcItem;

		// Adjust for inset
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;
		if (m_pVerticalScrollBar/* && m_pVerticalScrollBar->IsVisible()*/) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

		if (m_items.GetSize() == 0) {
			ProcessScrollBar(rc, 0, 0);
			return;
		}

		// Determine the minimum size
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
			szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();

		int nAdjustables = 0;
		int cyFixed = 0;
		int nEstimateNum = 0;
		for (int it1 = 0; it1 < m_items.GetSize(); it1++) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if (!pControl->IsVisible()) continue;
			if (pControl->IsFloat()) continue;
			SIZE sz = pControl->EstimateSize(szAvailable);
			if (sz.cy == 0) {
				nAdjustables++;
			}
			else {
				if (sz.cy < pControl->GetMinHeight()) sz.cy = pControl->GetMinHeight();
				if (sz.cy > pControl->GetMaxHeight()) sz.cy = pControl->GetMaxHeight();
			}
			cyFixed += sz.cy + pControl->GetPadding().top + pControl->GetPadding().bottom;
			nEstimateNum++;
		}
		cyFixed += (nEstimateNum - 1) * m_iChildPadding;

		// Place elements
		int cyNeeded = 0;
		int cyExpand = 0;
		if (nAdjustables > 0) cyExpand = max(0, (szAvailable.cy - cyFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosY = rc.top;
		if (m_pVerticalScrollBar /*&& m_pVerticalScrollBar->IsVisible()*/) {
			iPosY -= m_pVerticalScrollBar->GetScrollPos();
		}
		int iPosX = rc.left;
		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
			iPosX -= m_pHorizontalScrollBar->GetScrollPos();
		}
		int iAdjustable = 0;
		int cyFixedRemaining = cyFixed;
		for (int it2 = 0; it2 < m_items.GetSize(); it2++) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if (!pControl->IsVisible()) continue;
			if (pControl->IsFloat()) {
				SetFloatPos(it2);
				continue;
			}

			RECT rcPadding = pControl->GetPadding();
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
			CheckMapControl(pControl);

			iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
			cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
			szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
		}
		cyNeeded += (nEstimateNum - 1) * m_iChildPadding;

		// Process the scrollbar
		ProcessScrollBar(rc, 0, cyNeeded);
	}

	void CLayerLayoutUI::SetScrollPos(SIZE szPos)
	{
		int cx = 0;
		int cy = 0;
		if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) 
		{
			int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
			m_pVerticalScrollBar->SetScrollPos(szPos.cy);
			cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
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
			CheckMapControl(pControl);
		}

		Invalidate();
	}

	void CLayerLayoutUI::EnsureVisible(int nIndex)
	{
		if(!m_bEnsureVisible) return;
		RECT rcItem = GetItemAt(nIndex)->GetPos();
		RECT rcList = this->GetPos();
		RECT rcListInset = this->GetInset();

		rcList.left += rcListInset.left;
		rcList.top += rcListInset.top;
		rcList.right -= rcListInset.right;
		rcList.bottom -= rcListInset.bottom;

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

	//Add,Del,Sel controls
	//--------------------------------------------------------------------------------------------

	//--CContainerUI inherited functions----
	bool CLayerLayoutUI::Add(CControlUI* pControl)
	{
		if(pControl == NULL) return false;
		if(pControl->GetInterface(DUI_CTR_LAYERITEM))
		{
			CLayerUI* pLayer = (CLayerUI*)(pControl);
			pLayer->m_spLayerInfo = m_spLayerInfo;
			//attach layer self event
			AttachEvent(pLayer,pLayer,(EventFunc)&CLayerUI::OnChildEvent,true);
		}
		else if(pControl->GetInterface(DUI_CTR_GROUPITEM))
		{
			CGroupUI* pGroup = (CGroupUI*)(pControl);
			if(!pGroup->m_spLayerInfo.get()){
				ShareInfo(pGroup,m_spLayerInfo);
			}
		}
		auto ret = __super::Add(pControl);
		if(ret) UpdateToVecLayers(pControl, LayerOpt::LO_Add);
		return ret;
	}

	bool CLayerLayoutUI::AddAt(CControlUI* pControl, int nIndex)
	{
		if(pControl == NULL) return false;
		if(pControl->GetInterface(DUI_CTR_LAYERITEM))
		{
			CLayerUI* pLayer = (CLayerUI*)(pControl);
			pLayer->m_spLayerInfo = m_spLayerInfo;
			//attach layer self event
			AttachEvent(pLayer,pLayer,(EventFunc)&CLayerUI::OnChildEvent,true);
		}
		else if(pControl->GetInterface(DUI_CTR_GROUPITEM))
		{
			CGroupUI* pGroup = (CGroupUI*)(pControl);
			if(!pGroup->m_spLayerInfo.get()){
				ShareInfo(pGroup,m_spLayerInfo);
			}
		}
		auto ret = __super::AddAt(pControl, nIndex);
		if (ret) UpdateToVecLayers(pControl, LayerOpt::LO_AddAt,nIndex);
		return ret;
	}

	bool CLayerLayoutUI::Remove(CControlUI * pControl)
	{
		if (pControl == NULL) return false;
		for (int it = 0; it < m_items.GetSize(); it++)
		{
			if (static_cast<CControlUI*>(m_items[it]) == pControl)
			{
				RemoveMapControl(pControl);
				m_mapSel.erase(pControl);
				UpdateToVecLayers(pControl, LayerOpt::LO_Remove);
				NeedUpdate();
				if (m_bAutoDestroy)
				{
					if (m_bDelayedDestroy && m_pManager)
						m_pManager->AddDelayedCleanup(pControl);
					else delete pControl;
				}
				return m_items.Remove(it);
			}
		}

		return false;
	}

	void CLayerLayoutUI::RemoveAll()
	{
		RemoveMapControl(this);
		m_mapSel.clear();
		UpdateToVecLayers(nullptr, LayerOpt::LO_Clear);
		__super::RemoveAll();
	}

	bool CLayerLayoutUI::RemoveNotDestroy(CControlUI* pControl)
	{
		RemoveMapControl(pControl);
		if(m_bRmSel) m_mapSel.erase(pControl);
		auto ret = __super::RemoveNotDestroy(pControl);
		if(ret) UpdateToVecLayers(pControl, LayerOpt::LO_Remove);
		return ret;
	}

	//--CLayerLayoutUI functions----
	
	void CLayerLayoutUI::LayerRemoveSelected()
	{
		std::map<CControlUI*, void*> mapSel = m_mapSel;
		for(auto iter = mapSel.begin();iter!=mapSel.end();++iter)
		{
			CLayerUI* pLayer = (CLayerUI*)(iter->first->GetInterface(DUI_CTR_LAYERITEM));
			if(pLayer)
			{
				CContainerUI* p = GetParentLayout(pLayer);
				if(p)
				{
					m_mapSel.erase(pLayer);
					p->Remove(pLayer);
				}
			}
		}
		Invalidate();
	}

	bool CLayerLayoutUI::LayerRemove(CLayerUI* rmc)
	{
		if(rmc && GetLayer(rmc->GetZ()) == rmc)
		{
			CContainerUI* p = GetParentLayout(rmc);
			if(p)
			{
				m_mapSel.erase(rmc);
				p->Remove(rmc);
				Invalidate();
				return true;
			}
		}
		return false;
	}

	bool CLayerLayoutUI::LayerSelect(UINT nZIndex)
	{
		CLayerUI* p= GetLayer(nZIndex);
		if(p)
		{
			m_mapSel[p] = p;
			Invalidate();
			return true;
		}
		return false;
	}

	bool CLayerLayoutUI::LayerDeselect(UINT nZIndex)
	{
		CLayerUI* p= GetLayer(nZIndex);
		if(p)
		{
			m_mapSel.erase(p);
			Invalidate();
			return true;
		}
		return false;
	}

	void CLayerLayoutUI::LayerSelectAll()
	{
		if(m_spLayerInfo.get())
		{			
			for(auto iter = m_spLayerInfo->vecLayers.begin();iter!=m_spLayerInfo->vecLayers.end();++iter)
			{
				m_mapSel[*iter] = *iter;
			}
		}
	}

	void CLayerLayoutUI::LayerSelectNone()
	{
		m_mapSel.clear();
		Invalidate();
	}

	bool CLayerLayoutUI::LayerAddEnd(CLayerUI * src)
	{
		if(src && m_spLayerInfo.get())
		{
#ifdef DEBUG
			auto pTestLayer = GetLayerFormContainer(this,FALSE);
			assert((*m_spLayerInfo->vecLayers.rbegin()) == pTestLayer);
#endif
			if(m_spLayerInfo->vecLayers.size() == 0)
			{
				return this->Add(src);
			}
			else
			{
				// may be add to group
				auto pLayer = *m_spLayerInfo->vecLayers.rbegin();
				CContainerUI* p = GetParentLayout(pLayer);
				if(p)
				{
					return p->Add(pLayer);
				}
			}
		}
		return false;
	}

	bool CLayerLayoutUI::LayerAddBegin(CLayerUI* src)
	{
		if(!m_spLayerInfo.get()) return false;
#ifdef DEBUG
		auto pLayer = GetLayerFormContainer(this,TRUE);
		if(pLayer)
		{
			assert(m_spLayerInfo->vecLayers.size() && *m_spLayerInfo->vecLayers.begin() == pLayer);
		}
#endif
		return LayerAddAt(src,0);
	}

	bool CLayerLayoutUI::LayerAddAt(CLayerUI* src, UINT nZIndex)
	{
		if(src && m_spLayerInfo.get())
		{
			if(m_spLayerInfo->vecLayers.size() == 0)
			{
				return this->Add(src);
			}
			else
			{
				auto pDst = GetLayer(nZIndex);
				CContainerUI* p = GetParentLayout(pDst);
				if(p)
				{
					auto index = p->GetItemIndex(pDst);
					return p->AddAt(src,index);
				}
			}
		}
		return false;
	}

	void CLayerLayoutUI::MoveItem(int nSrcIndex, int nDesIndex)
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

	void CLayerLayoutUI::MoveItem()
	{
		CContainerUI* pLayout = nullptr;
		CControlUI* pMove = nullptr;
		auto MoveControl = [&]() -> bool
		{
			pLayout = GetParentLayout(pMove);
			if(pLayout)
			{
				if(pMove->GetInterface(DUI_CTR_GROUPHEADER))
				{
					// pLayout is GroupUI
					pMove = pLayout;
					pLayout = (CGroupUI*)GetParentLayout(pMove);
				}
				DUI__Trace(L"Src %p, pMove %p -> to %p",m_miInfo.src, pMove,m_miInfo.dst);

				int indexMove = m_miInfo.gp_dst->GetItemIndex(pMove);
				if (pLayout->GetInterface(DUI_CTR_LAYERLAYOUT))
				{
					((CLayerLayoutUI*)pLayout)->RemoveNotDestroy(pMove);
				}
				else if (pLayout->GetInterface(DUI_CTR_GROUPITEM))
				{
					((CGroupUI*)pLayout)->RemoveNotDestroy(pMove);
				}
				else
					pLayout->RemoveNotDestroy(pMove);
				if(m_miInfo.bGroup)
				{
					m_miInfo.gp_dst->Add(pMove);
				}
				else
				{
					int index = m_miInfo.gp_dst->GetItemIndex(m_miInfo.dst);
					DUI__Trace(L"index %d, dst count %d",index, m_miInfo.gp_dst->GetCount());

					if(index == -1) 
					{
						pLayout->AddAt(pMove,indexMove);//failed restone 
						return false;
					}
					if(m_miInfo.bTop)
					{
						m_miInfo.gp_dst->AddAt(pMove,index);
					}
					else
					{
						m_miInfo.gp_dst->AddAt(pMove,++index);
					}
				}
			}
			return true;
		};
		if(m_miInfo.dst)
		{
			CSwitch sw(m_bRmSel);
			ASSERT(m_miInfo.gp_dst);
			
			auto iter = m_mapSel.find(m_miInfo.src);
			if(iter == m_mapSel.end())//unselected control move
			{
				pMove = m_miInfo.src;
				MoveControl();
			}
			else
			{
				//selected control move
				for(iter = m_mapSel.begin(); iter != m_mapSel.end(); ++iter)
				{
					pMove = iter->first;
					if(!MoveControl())
					{
						continue;
					}
				}
			}
			m_miInfo.gp_dst->NeedUpdate();
		}
		m_miInfo.Clear();
		PrintLayerZ(this);
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

	CControlUI* CLayerLayoutUI::PtRoundControl(POINT ptMouse)
	{
		CControlUI* pControl = NULL;
		if (m_spLayerInfo.get())
		{
			auto& mapControl = m_spLayerInfo->mapControl;
			auto iter = std::find_if(mapControl.begin(), mapControl.end(),
				MapControlFind(ptMouse,m_miInfo));
			if(iter != mapControl.end()){
				pControl = iter->first;
			}
		}
		return pControl;
	}

	bool CLayerLayoutUI::MakeCursorImage(HDC hDC,const CRect rcItem, const CPoint pt, int alpha)
	{
		//auto GetEncoderClsid=[](LPCTSTR format, CLSID* pClsid)->int
		//{
		//	UINT num= 0;
		//	UINT size= 0;

		//	ImageCodecInfo* pImageCodecInfo= NULL;

		//	GetImageEncodersSize(&num, &size);
		//	if(size== 0)
		//	{
		//		return -1;
		//	}
		//	pImageCodecInfo= (ImageCodecInfo*)(malloc(size));
		//	if(pImageCodecInfo== NULL)
		//	{
		//		return -1;
		//	}

		//	GetImageEncoders(num, size, pImageCodecInfo);

		//	for(UINT j=0; j< num; ++j)
		//	{
		//		if(wcscmp(pImageCodecInfo[j].MimeType, format)== 0)
		//		{
		//			*pClsid= pImageCodecInfo[j].Clsid;
		//			free(pImageCodecInfo);
		//			return j;
		//		}
		//	}

		//	free(pImageCodecInfo);
		//	return -1;
		//};
		if(m_hCursor.pBitmap) m_hCursor.Release();
		if(!m_hCursor.pBitmap)
		{
			m_hCursor.ptOffset.x = rcItem.left;
			m_hCursor.ptOffset.y = pt.y-rcItem.top;
			ICONINFO   ii = {FALSE,(DWORD)(pt.x-rcItem.left),(DWORD)(pt.y-rcItem.top),0,0};
			Color clear(0,0,0,0);
			m_hCursor.pBitmap = new Bitmap( rcItem.GetWidth(),rcItem.GetHeight(), PixelFormat32bppARGB );
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

	CRect CLayerLayoutUI::GetFitLayoutRc(CRect rcItem)
	{
		rcItem.left = m_rcItem.left;
		rcItem.right = m_rcItem.right;
		if (m_pVerticalScrollBar) rcItem.right -= m_pVerticalScrollBar->GetFixedWidth();
		return rcItem;
	}

	bool CLayerLayoutUI::CheckRectInvalid(RECT &rc)
	{
		if(rc.right < rc.left || rc.bottom < rc.top)
			return true;
		return false;
	}	

	std::vector<CLayerUI*> CLayerLayoutUI::MakeSelectItems()
	{
		std::vector<CLayerUI*> vecLayers;		
		for(auto iter = m_mapSel.begin();iter!=m_mapSel.end();++iter)
		{
			CLayerUI* p = (CLayerUI*)iter->first->GetInterface(DUI_CTR_LAYERITEM);
			if(p) vecLayers.push_back(p);
		}
		return vecLayers;
	}

}//namespace DirectUI