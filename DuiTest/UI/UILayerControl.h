#ifndef _LAYERCONTROLUI_H_
#define _LAYERCONTROLUI_H_

namespace DirectUI
{

	class CLayerControlUI : public CWindowUI, public CControlUI
	{
	public:
		CLayerControlUI();
		~CLayerControlUI();

		//CControlUI
	public:
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);

		virtual void DoInit();
		virtual void Invalidate();
		virtual void DoPaint(HDC hDC, const RECT& rcPaint);
		virtual void SetInternVisible(bool bVisible = true); 
		virtual void SetVisible(bool bVisible = true); 
		virtual void SetPos(RECT rc);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		BOOL IsHasParentWnd() const;
		void SetHasParentWnd(BOOL b);
		BOOL IsLayeredWnd() const;
		void SetLayeredWnd(BOOL b);
		void SetSkinFile(LPCTSTR pstrSkin);

	private:
		BOOL m_bHasParent;
		CUIString m_strSkin;

		//CWindowUI
	public:
		virtual LPCTSTR GetWindowClassName() const;	
		virtual CUIString GetSkinFile();
		virtual CUIString GetSkinFolder();

		virtual void InitWindow();
		virtual void OnFinalMessage(HWND hWnd);
		virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
		virtual CControlUI* CreateControl(LPCTSTR pstrClass);
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnPaint(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		virtual CPoint GetPoint(LPARAM lParam){return CPoint(lParam);}
	public:
		HDC m_hDcOffscreen;
		HDC m_hDcBackground;
		HBITMAP m_hbmpOffscreen;
		LPBYTE m_pBmpOffscreenBits;
		HBITMAP m_hbmpBackground;
		BOOL m_bFirstLayout;
		BOOL m_bLayeredWindow;
	};


} //namespace DirectUI

#endif //_LAYERCONTROLUI_H_