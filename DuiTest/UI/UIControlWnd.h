#ifndef _CCONTROLWND_H_
#define _CCONTROLWND_H_
#include <vector>
#include <functional>
#include "FunctionSlot.hpp"
namespace DirectUI
{

	class CControlWndUI : public CWindowWnd, public CControlUI
	{
	public:
		CControlWndUI();
		~CControlWndUI();

		//CControlUI
	public:
		virtual void DoInit();
		void Invalidate();
		virtual void DoPaint(HDC hDC, const RECT& rcPaint){} // ������
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
		CFuncSlot_4<UINT, WPARAM, LPARAM ,LRESULT&> Slot_PreHandleMessage;
		CFuncSlot_1<CPoint> Slot_LBD; //����������
		CFuncSlot_1<CPoint> Slot_LBU;//����������
		CFuncSlot_1<CPoint> Slot_MM;//����ƶ�
		CFuncSlot_1<CPoint> Slot_LDB;////������˫��
		CFuncSlot_1<CPoint> Slot_KD; //���̰���
		CFuncSlot_1<CPoint> Slot_KC; //���� Char
		void CP(CPoint) {}
	};


}

#endif //_CCONTROLWND_H_