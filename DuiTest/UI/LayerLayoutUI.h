#ifndef __LAYERLISTUI_H__
#define __LAYERLISTUI_H__

#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <limits>
#include "FunctionSlot.hpp"

extern const TCHAR* DUI_CTR_LAYERLAYOUT;
extern const TCHAR* DUI_CTR_LAYERITEM;
extern const TCHAR* DUI_CTR_GROUPITEM;
extern const TCHAR* DUI_CTR_GROUPHEADER;
extern const TCHAR* INTERFACE_INTERMSG;
namespace DirectUI
{
	class CLayerUI;
	class CGroupHeaderUI;
	class CGroupUI;
	class CLayerLayoutUI;

	typedef struct _tagControlPosInfo
	{
		//CControlUI* pControl_;
		CRect rcFit;
		_tagControlPosInfo()
			//:pControl_(nullptr)
		{
		}
	}ControlPosInfo;

	typedef struct _tagMoveItemInfo
	{
		std::map<CControlUI*,void*>* pSelControl;//pointer of mainLayout's selected controls
		CControlUI* src; //moving selected item
		CControlUI* dst; //target clayerui
		CContainerUI* gp_dst; // target drop layout
		BOOL bGroup; //is group,and drop end of group
		BOOL bTop; //is top of dst-control pos
		int nLinePos; //clac-ed line position of drop to
		_tagMoveItemInfo() :pSelControl(0), src(0),dst(0), gp_dst(0),
			bGroup(0),bTop(0),nLinePos(0) {}
		void Clear(){
			//src = nullptr;
			dst = nullptr;
			gp_dst = nullptr;
			bGroup = 0;
			bTop = 0;
			nLinePos = 0;
		}
	}MoveItemInfo;

	typedef struct _tagLayerInfo
	{
		CRect rcLayerInset;
		CRect rcDragTest;
		size_t nLayerHeight; //default layer height
		size_t nGroupHeaderHeight;  //default group...

		DWORD dwItemMoveColor; // line and rect
		//DWORD dwItemBkColor; //bkcolor
		DWORD dwItemHotBkColor;
		DWORD dwItemSelectedBkColor;
		//CString sItemNormalImage; //bkimage
		CString sItemHotImage;
		CString sItemSelectedImage;

		//BOOL bLayerInit; //track main layer init finish;
		/** using painting method to notify z order changed**/
		//DWORD dwPaintIndexCur; //begain LayerLayout repaint set to 0,when paint a layer ,it will increment 1,an change z of layer

		std::map<CControlUI*, ControlPosInfo> mapControl; //all control pos in main layout
		std::vector<CLayerUI*> vecLayers; //all layer, by index of z

		CLayerLayoutUI* pLayout;//main layout pointer

		_tagLayerInfo()
			:rcLayerInset(10,0,0,0),rcDragTest(0,0,4,4),nLayerHeight(30),nGroupHeaderHeight(30),
			dwItemMoveColor(0xFFffae00),dwItemHotBkColor(0xFF6B9299),
			dwItemSelectedBkColor(0xFF258C9C),pLayout(nullptr)
		{}
	} LayerInfo;

	namespace Inter
	{
		enum InterNotifyMsg
		{
			NeedLayoutUpdate,
			LayerItemSelect,
			LayerItemDrag,
			LayerItemMove,
			LayerItemDrop,
			LayerItemZ
		};

		enum LayerOpt
		{
			LO_Add = 1,
			LO_AddAt,
			LO_Remove,
			LO_Clear
		};

		class IInterMessage
		{

		public:
			typedef bool (IInterMessage::*EventFunc)(void*);

			//pass message to high level control, finally to LayerLayout
			virtual void Message(TEventUI* event_,InterNotifyMsg what) = 0;

			//use to attach child control (or self) event before DoEvent call;
			virtual bool OnChildEvent(void* event_in) = 0;
		protected:
			//info of layer
			std::shared_ptr<_tagLayerInfo> m_spLayerInfo;
			//track all controls event
			template<typename O, typename Fn>
			void AttachEvent(CControlUI* pControl, O* pthis, Fn pfunc);
			//share m_spLayerInfo to all relative controls
			void ShareInfo(CControlUI* pControl, std::shared_ptr<_tagLayerInfo>& info);
			///updata controls rect to m_spLayerInfo's mapControl
			void CheckMapControl(CControlUI* pControl);
			///erase m_spLayerInfo's mapControl and it's sub-controls
			void RemoveMapControl(CControlUI* pControl);
			///get Layer item by index of Z
			CLayerUI* GetLayer(UINT nZIndex);
			///update vecLayers by type; opt: enum LayerOpt
			void UpdateToVecLayers(CControlUI* pControl, int opt, UINT placement = 0);
			///reupdate m_spLayerInfo's vecLayers order
			void UpdateVecLayers();
			///for UpdateVecLayers call,don't use it;
			void CheckLayers();
		};

		class CSwitch
		{
		public:
			CSwitch(bool & b):b_(b)
			{
					b_ = !b_;
			}
			~CSwitch()
			{
					b_ = !b_;			
			}
		private:
			CSwitch(CSwitch&);
			void operator = (const CSwitch&); 
			bool &b_;
		};
	}

	class CLayerUI  : public CHorizontalLayoutUI, public Inter::IInterMessage
	{
		friend class CGroupUI;
		friend class CLayerLayoutUI;
		friend class Inter::IInterMessage;
	public:
		CLayerUI();
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual UINT GetControlFlags() const;
		virtual void Init();

		bool Add(CControlUI* pControl);

		void DoEvent(TEventUI& event_);
		virtual bool OnChildEvent(void* event_in);

		LPVOID GetPtr(UINT index = 0); // get storage of user information by index
		UINT SetPtr(LPVOID ptr, UINT index = 0); //return valid index
		UINT GetZ();
		//void SetZ(UINT z); //Note ,it will change layer position in LayerLayout

	protected:
		void SetInnerZ(UINT z);

		virtual void Message(TEventUI* event_, Inter::InterNotifyMsg what);

		bool m_bButtonDown;
		bool m_bDrag;
		bool m_bInit;
		UINT m_nZ;
		
		POINT m_ptLBDown;

		CStdPtrArray m_ptr;
	};

	class CGroupHeaderUI : public CHorizontalLayoutUI
	{
	public:
		CGroupHeaderUI();
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	};

	class CGroupUI
		: public CVerticalLayoutUI , public Inter::IInterMessage
	{
		friend class CLayerLayoutUI;
	public:
		CGroupUI();
		~CGroupUI();
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual UINT GetControlFlags() const;
		virtual SIZE EstimateSize(SIZE szAvailable);

		//Note: if U add some controls, don't add at index 0,it won't work; index 0 -  place element GroupHeader
		bool Add(CControlUI* pControl);
		bool AddAt(CControlUI* pControl, int nIndex);
		bool Remove(CControlUI* pControl);
		bool RemoveNotDestroy(CControlUI* pControl); //not virtual
		//bool RemoveAt(int nIndex);
		void RemoveAll();

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetItemAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetPos(RECT rc);

		bool SetExpand(bool bExpand);
		bool GetExpand();
		size_t SetHeaderHeight(size_t nHeight);
		size_t GetHeaderHeight();
		size_t SetItemHeight(size_t nHeight);
		size_t GetItemHeight();

		CGroupHeaderUI* GetHeaderUI();

		virtual void DoPaint(HDC hDC, const RECT& rcPaint);

		void DoEvent(TEventUI& event_);
		virtual bool OnChildEvent(void* event_in);
	protected:
		virtual void Message(TEventUI* event_, Inter::InterNotifyMsg what);
	private:
		size_t m_itemHeight; //default item height
		size_t m_defaultHeaderHeight; 
		size_t m_bExpand;
		bool m_bButtonDown;
		POINT m_ptLBDown;
		bool m_bDrag;
		std::unique_ptr<CGroupHeaderUI> m_header;
	};


	class /*DirectUI_API*/ CLayerLayoutUI 
		: public CVerticalLayoutUI , public Inter::IInterMessage
	{
		friend class CLayerUI;
		friend class CGroupUI;
	public:
		CLayerLayoutUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		static CControlUI* CreateControl(LPCTSTR pstrClass)
		{
			if(_tcsicmp(pstrClass, DUI_CTR_LAYERLAYOUT) == 0)
			{
				return new CLayerLayoutUI;
			}
			else if(_tcsicmp(pstrClass, DUI_CTR_LAYERITEM) == 0)
			{
				return new CLayerUI;
			}
			else if(_tcsicmp(pstrClass, DUI_CTR_GROUPITEM) == 0)
			{
				return new CGroupUI;
			}
			else if(_tcsicmp(pstrClass, DUI_CTR_GROUPHEADER) == 0)
			{
				return new CGroupHeaderUI;
			}
			return NULL;
		}

	protected:
		//IInterMessage
		virtual void Message(TEventUI* event_, Inter::InterNotifyMsg what);
		virtual bool OnChildEvent(void* event_in);

	public:

		void SetMultiSel(bool bFlag);
		bool GetMultiSel() const;
		void SetTrackEvent(bool bFlag);
		bool GetTrackEvent() const;
		//also effect to group
		void SetLayerSelImage(LPCTSTR pstrImage);
		CUIString GetLayerSelImage();
		void SetLayerHotImage(LPCTSTR pstrImage);
		CUIString GetLayerHotImage();
		void SetMoveLineColor(DWORD color);
		DWORD GetMoveLineColor();
		void SetLayerSelColor(DWORD color);
		DWORD GetLayerSelColor();
		void SetLayerHotColor(DWORD color);
		DWORD GetLayerHotColor();
		void SetLayerDefaultHeight(int nSize);
		size_t GetLayerDefaultHeight();
		void SetGroupDefaultHeight(int nSize);
		size_t GetGroupDefaultHeight();
		void SetLayerInset(RECT rc);
		CRect GetLayerInset();
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetItemAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		virtual void DoPaint(HDC hDC, const RECT& rcPaint);
		virtual void PaintBkColor(HDC hDC);
		virtual void PaintBkImage(HDC hDC);
		virtual void PaintStatusImage(HDC hDC);
		virtual void PaintText(HDC hDC);
		virtual void PaintBorder(HDC hDC);
		virtual void PaintMoveItem(HDC hDC);
		bool DrawImage(HDC hDC,const RECT &rcItem, LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);
		
		void DoEvent(TEventUI& event_);
		void SetPos(RECT rc);
		virtual void SetScrollPos(SIZE szPos);
		void EnsureVisible(int nIndex);

		bool Add(CControlUI* pControl);
		bool AddAt(CControlUI* pControl, int nIndex);
		bool Remove(CControlUI* pControl);
		bool RemoveNotDestroy(CControlUI* pControl); //not virtual
		//bool RemoveAt(int nIndex);
		void RemoveAll();

		void RemoveSelected(); //inc layer and group
		void LayerRemoveSelected();
		bool LayerRemove(CLayerUI* rmc);
		bool LayerSelect(UINT nZIndex);
		bool LayerDeselect(UINT nZIndex);
		void LayerSelectAll();
		void LayerSelectNone();
		bool LayerAddEnd(CLayerUI* src);
		bool LayerAddBegin(CLayerUI* src);
		bool LayerAddAt(CLayerUI* src, UINT nZIndex);
	public:
		virtual void Init();
		
		// Event fire functions
		CFuncSlot_1<CLayerUI*> slot_ZChange; //Fire z order changed
		CFuncSlot_1<std::vector<CLayerUI*> > slot_SelectChange; //Fire select items function
	protected:
		void MoveItem(int nSrcIndex, int nDesIndex);
		void MoveItem();

		virtual CControlUI* PtHitControl(POINT ptMouse, int &nIndex);
		virtual CControlUI* PtRoundControl(POINT ptMouse);
		bool MakeCursorImage(HDC hDC, const CRect rcItem, const CPoint pt, int alpha = 200);
		inline CRect GetFitLayoutRc(CRect rcItem);
		bool CheckRectInvalid(RECT &rc);
		std::vector<CLayerUI*> MakeSelectItems();
private:
		std::map<CControlUI*, void*> m_mapSel;
		MoveItemInfo m_miInfo;
		struct _tagCursorRes
		{
			HCURSOR hCursor;
			HBITMAP hBitmap;
			Bitmap* pBitmap;
			BOOL bDrag;
			CPoint ptMove;
			CPoint ptOffset;
			_tagCursorRes():pBitmap(0),hCursor(0),hBitmap(0),bDrag(0){}
			~_tagCursorRes(){Release();}
			void Release(){
				if(pBitmap)
				{
					delete pBitmap;
					pBitmap = NULL;
					::DeleteObject(hBitmap);
					::DestroyIcon((HICON)hCursor);
				}
			}
		} m_hCursor;

		CControlUI* m_pHotControl;

		POINT ptLastMouse;
		RECT m_rcNewPos;

		bool m_bTrackEvents; //监控子控件的InterNotifyMsg事件
		bool m_bMultiSel;
		bool m_bEnsureVisible; //选中的时候是否移动位置

		//bool m_bForbidPaint; // 不绘制
		bool m_bRmSel; //
	};


	class CLayerLayoutCallback
	{
	public:
		virtual CControlUI* CreateControl(LPCTSTR pstrClass)
		{
			return CLayerLayoutUI::CreateControl(pstrClass);
		}
	};


	template <typename TSrc, typename TDst> TDst* CastCallback(TSrc* t_src)
	{
		return reinterpret_cast<TDst*>(t_src);
	}

	void PrintLayerZ(CLayerLayoutUI* pLayout);
}
#endif // __LAYERLISTUI_H__
