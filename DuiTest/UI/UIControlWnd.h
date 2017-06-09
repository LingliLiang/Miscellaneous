#ifndef _CCONTROLWND_H_
#define _CCONTROLWND_H_
#include <vector>
#include <functional>
namespace DirectUI
{
	template<
		typename PM_1 = unsigned long,
		typename PM_2 = unsigned long,
		typename PM_3 = unsigned long>
	class CFuncSlot
	{
	public:
		typedef void FnType(PM_1,PM_2,PM_3);
		template <typename O, typename OFnType>
		void Plug(O* pthis, OFnType pFn, bool first = false)
		{
			std::function<FnType> insert;
			if(first)
				vecFuncs.insert(vecFuncs.begin(),insert);
			else				
				vecFuncs.push_back(insert);
			*(vecFuncs.rbegin()) = std::bind(pFn,pthis,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
		}
		void Empty()
		{
			vecFuncs.clear();
		}
		void Active(PM_1 p1, PM_2 p2 , PM_3 p3)
		{
			for(auto iter = vecFuncs.begin();iter != vecFuncs.end(); ++iter)
			{
				(*iter)(p1,p2,p3);
			}
		}
	protected:
		std::vector<std::function<FnType> > vecFuncs;
	};

	class CControlWndUI : public CWindowWnd, public CControlUI
	{
	public:
		CControlWndUI();
		~CControlWndUI();

		//CControlUI
	public:
		virtual void DoInit();
		void Invalidate();
		virtual void DoPaint(HDC hDC, const RECT& rcPaint){} // ≤ªªÊ÷∆
		virtual void SetInternVisible(bool bVisible = true); 
		virtual void SetPos(RECT rc);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		BOOL IsHasParentWnd() const;
		void SetHasParentWnd(BOOL b);

	private:
		BOOL m_bHasParent;

		//CWindowWnd
	protected:
		virtual LPCTSTR GetWindowClassName() const;
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual void OnFinalMessage(HWND hWnd);

		virtual LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnLButtonDBClick(WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnChar(WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnSetCursor(WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnClose(WPARAM wParam, LPARAM lParam);

		virtual CPoint GetPoint(LPARAM lParam){return CPoint(lParam);}
	public:
#define MSG_HANDLED (~0)
		CFuncSlot<UINT, WPARAM, LPARAM > Slot_PreHandleMessage;
		CFuncSlot<CPoint> Slot_LButtonDown;
		CFuncSlot<CPoint> Slot_LButtonUp;
		CFuncSlot<CPoint> Slot_MouseMove;
		CFuncSlot<CPoint> Slot_LButtonDBClick;
		CFuncSlot<CPoint> Slot_KeyDown;
		CFuncSlot<CPoint> Slot_KeyChar;
	};


}

#endif //_CCONTROLWND_H_