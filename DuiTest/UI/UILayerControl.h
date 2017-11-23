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
		void SetParentClip(BOOL b);

		//当消息为wm_onmove时进行窗口跟随移动
		//设置为0时不使用跟随移动
		void SetWMONMOVE(UINT wm_onmove); 
	private:
		BOOL m_bHasParent;
		CUIString m_strSkin;
		BOOL m_bPaintInit;
		
		//启用裁剪, 当超出父辈的范围时进行窗口的裁剪
		//适应于动态改变的布局,如滚动条的滚动.
		//该选项会有大量计算,固定界面时不建议使用
		BOOL m_bParentClip; 

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
		virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnPaint(WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		virtual CPoint GetPoint(LPARAM lParam){return CPoint(lParam);}
	private:
		void DoParentClip();

		HDC m_hDcOffscreen;
		HBITMAP m_hbmpOffscreen;
		LPBYTE m_pBmpOffscreenBits;

		BOOL m_bFirstLayout;
		BOOL m_bLayeredWindow;
		enum emClipState { CLIP_NON/*无裁剪*/,CLIP_HALF/*裁剪部分*/,CLIP_FULL/*全部裁剪*/};
		UINT m_nClipState; //当前的裁剪状态,由滚动条导致的超出界面的状态
		BOOL m_bWndVisible;

		//需要为主窗口定义移动时通知子窗口的消息
		//而本窗口会根据消息进行跟随移动
		UINT m_nWMOnMove; 
	};


} //namespace DirectUI

#endif //_LAYERCONTROLUI_H_