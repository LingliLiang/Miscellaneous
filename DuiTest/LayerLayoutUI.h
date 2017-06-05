#ifndef __LAYERLISTUI_H__
#define __LAYERLISTUI_H__

#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <map>

extern const TCHAR* DUI_CTR_LAYERLIST;
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
	typedef std::map<CControlUI*, ControlPosInfo> MapCtl;
	typedef std::map<CControlUI*, ControlPosInfo>::value_type MapCtlVt;
	typedef std::map<CControlUI*, ControlPosInfo>::iterator MapCtlIt;

	typedef struct _tagMoveItemInfo
	{
		std::map<CControlUI*,void*>* pSelControl;
		CControlUI* src; //moving selected item
		CControlUI* dst; //
		CContainerUI* gp_dst;
		//int gp_dst_pos;
		BOOL bGroup;
		BOOL bTop;
		int nLinePos;
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
		size_t nLayerHeight; //default layer height
		size_t nGroupHeaderHeight;  //default group...

		//CLayerLayoutUI
		DWORD dwItemMoveColor; // line and rect
		//DWORD dwItemBkColor; //bkcolor
		DWORD dwItemHotBkColor;
		DWORD dwItemSelectedBkColor;
		//CString sItemNormalImage; //bkimage
		CString sItemHotImage;
		CString sItemSelectedImage;
		CLayerLayoutUI* pLayout;
		MapCtl mapControl;
		_tagLayerInfo()
			:nLayerHeight(30),nGroupHeaderHeight(30),
			dwItemMoveColor(0xFFffae00),dwItemHotBkColor(0xFF6B9299),
			dwItemSelectedBkColor(0xFF258C9C),pLayout(0)
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
			LayerItemDrop
		};

		class IInterMessage
		{

		public:
			typedef bool (IInterMessage::*EventFunc)(void*);

			//virtual void MessageAcross(TEventUI* event_,InterNotifyMsg what); // 
			virtual void Message(TEventUI* event_,InterNotifyMsg what) = 0;
			virtual bool OnChildEvent(void* event_in) = 0;
		protected:
			std::shared_ptr<_tagLayerInfo> m_LayerInfo;
			void AttachEvent(CControlUI* pControl, IInterMessage* pthis, EventFunc pfunc, bool self = false);
			void ShareInfo(CControlUI* pControl, std::shared_ptr<_tagLayerInfo>& info);
			void CheckControl(CControlUI* pControl);
		};

	}

	class CLayerUI  : public CHorizontalLayoutUI, public Inter::IInterMessage
	{
		friend class CGroupUI;
		friend class CLayerLayoutUI;
		//friend void Inter::ShareInfo(CControlUI* pControl, std::shared_ptr<_tagLayerInfo>& info);
	public:
		CLayerUI();
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual UINT GetControlFlags() const;

		bool Add(CControlUI* pControl);

		void DoEvent(TEventUI& event_);
		virtual bool OnChildEvent(void* event_in);
	protected:
		virtual void Message(TEventUI* event_, Inter::InterNotifyMsg what);

		bool m_bButtonDown;
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
		//friend void Inter::ShareInfo(CControlUI* pControl, std::shared_ptr<_tagLayerInfo>& info);
	public:
		CGroupUI();
		~CGroupUI();
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual UINT GetControlFlags() const;
		virtual SIZE EstimateSize(SIZE szAvailable);

		//Note: if U add some controls, don't add at index 0; index 0 -  place element GroupHeader
		bool Add(CControlUI* pControl);
		bool Remove(CControlUI* pControl);
		bool RemoveNotDestroy(CControlUI* pControl);
		void RemoveAll();
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetItemAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetPos(RECT rc);

		void SetItemInset(RECT rc);
		CRect GetItemInset();
		bool SetExpand(bool bExpand);
		bool GetExpand();
		size_t SetHeaderHeight(size_t nHeight);
		size_t GetHeaderHeight();
		size_t SetItemHeight(size_t nHeight);
		size_t GetItemHeight();

		CGroupHeaderUI* GetHeaderUI();


		void DoEvent(TEventUI& event_);
		virtual bool OnChildEvent(void* event_in);
	protected:
		virtual void Message(TEventUI* event_, Inter::InterNotifyMsg what);
	private:

		CRect m_itemInset;
		size_t m_itemHeight; //default item height
		size_t m_defaultHeaderHeight; 
		size_t m_bExpand;
		bool m_bButtonDown;
		std::unique_ptr<CGroupHeaderUI> m_header;
	};


	class /*DirectUI_API*/ CLayerLayoutUI 
		: public CVerticalLayoutUI , public Inter::IInterMessage
	{
	public:
		CLayerLayoutUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		static CControlUI* CreateControl(LPCTSTR pstrClass)
		{
			if(_tcsicmp(pstrClass, DUI_CTR_LAYERLIST) == 0)
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

		template <typename T> bool BindFunc(int LayerOper, void(T::*pf)(void), T* p)
		{
			if(LayerOper == Inter::LayerItemDrop)
			{
				std::function<void(void)> func;
				m_funcDrop.push_back(func);
				*(m_funcDrop.rbegin()) = std::bind(pf,p);
			}
			else if(LayerOper == Inter::LayerItemSelect)
			{
				std::function<void(void)> func;
				m_funcSelect.push_back(func);
				*(m_funcSelect.rbegin()) = std::bind(pf,p);
			}
			return true;
		}

	protected:
		virtual void Message(TEventUI* event_, Inter::InterNotifyMsg what);
		virtual bool OnChildEvent(void* event_in);
	public:

		void SetVerticalMode(bool bVertical);
		bool GetVerticalMode() const;
		void SetSelImage(LPCTSTR pstrImage);
		CUIString GetSelImage();
		void SetHotImage(LPCTSTR pstrImage);
		CUIString GetHotImage();

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetItemAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void DoEvent(TEventUI& event_);

		virtual void DoPaint(HDC hDC, const RECT& rcPaint);
		virtual void PaintBkColor(HDC hDC);
		virtual void PaintBkImage(HDC hDC);
		virtual void PaintStatusImage(HDC hDC);
		virtual void PaintText(HDC hDC);
		virtual void PaintBorder(HDC hDC);

		virtual void PaintMoveItem(HDC hDC);

		CControlUI* GetCurSel();
		bool SelectItem(int nIndex, bool bTakeFocus = false);

		bool DrawImage(HDC hDC,const RECT &rcItem, LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);

		bool CheckRectInvalid(RECT &rc);

		void EnsureVisible(int nIndex);

		void SetPos(RECT rc);
		bool Add(CControlUI* pControl);
		bool Remove(CControlUI* pControl);
		bool RemoveNotDestroy(CControlUI* pControl);
		void RemoveAll();

		void MoveItem();
	protected:
		void MoveItem(int nSrcIndex, int nDesIndex);

		virtual CControlUI* PtHitControl(POINT ptMouse, int &nIndex);
		virtual CControlUI* PtRoundControl(POINT ptMouse);
		bool MakeCursorImage(HDC hDC, const CRect rcItem, const CPoint pt, int alpha = 200);
		inline CRect GetFitLayoutRc(CRect rcItem);

	protected:

		struct _tagCursorRes
		{
			HCURSOR hCursor;
			HBITMAP hBitmap;
			Bitmap* pBitmap;
			BOOL bDrag;
			CPoint ptMove;
			CPoint ptOffset;
			_tagCursorRes():pBitmap(0),hCursor(0),bDrag(0){}
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

		std::vector<std::function<void(void)>> m_funcDrop;
		std::vector<std::function<void(void)>> m_funcSelect;

		typedef std::map<CControlUI*, void*> MapSel;
		typedef std::map<CControlUI*, void*>::value_type MapSelVt;
		typedef std::map<CControlUI*, void*>::iterator MapSelIt;
		MapSel m_MapSelControl;
		CControlUI* m_pHotControl;
		bool m_bMultiSel;

		MoveItemInfo m_miInfo;

		POINT ptLastMouse;
		RECT m_rcNewPos;

		bool m_bVertical;
		bool m_bEnsureVisible; //选中的时候是否移动位置
	};
}
#endif // __LAYERLISTUI_H__
