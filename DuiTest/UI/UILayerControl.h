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

		//����ϢΪwm_onmoveʱ���д��ڸ����ƶ�
		//����Ϊ0ʱ��ʹ�ø����ƶ�
		void SetWMONMOVE(UINT wm_onmove); 
	private:
		BOOL m_bHasParent;
		CUIString m_strSkin;
		BOOL m_bPaintInit;
		
		//���òü�, �����������ķ�Χʱ���д��ڵĲü�
		//��Ӧ�ڶ�̬�ı�Ĳ���,��������Ĺ���.
		//��ѡ����д�������,�̶�����ʱ������ʹ��
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
		enum emClipState { CLIP_NON/*�޲ü�*/,CLIP_HALF/*�ü�����*/,CLIP_FULL/*ȫ���ü�*/};
		UINT m_nClipState; //��ǰ�Ĳü�״̬,�ɹ��������µĳ��������״̬
		BOOL m_bWndVisible;

		//��ҪΪ�����ڶ����ƶ�ʱ֪ͨ�Ӵ��ڵ���Ϣ
		//�������ڻ������Ϣ���и����ƶ�
		UINT m_nWMOnMove; 
	};


} //namespace DirectUI

#endif //_LAYERCONTROLUI_H_