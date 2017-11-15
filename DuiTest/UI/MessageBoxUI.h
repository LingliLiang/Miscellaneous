#ifndef __MESSAGEBOXUI_H_
#define __MESSAGEBOXUI_H_


namespace DirectUI
{
//MB_OK,MB_YESNO,MB_YESNOCANCEL
#define MB_ICONINFO 0x00000050L
#define MB_ICONQUERY 0x00000060L
//#define MB_ICONERROR 0x00000010L
//#define MB_ICONWARNING 0x00000030L

	class CMessageBoxUI : public CWindowUI
	{
		friend static int CMessageBox(_In_opt_ HWND, _In_opt_ LPCTSTR, _In_opt_ LPCTSTR, _In_ UINT, _In_ bool, _In_ SIZE);
	public:
		
		~CMessageBoxUI()
		{
			//SetCaretBlinkTime(530);
		}

		LPCTSTR GetWindowClassName() const;

		virtual CUIString GetSkinFile();
		virtual CUIString GetSkinFolder();
		virtual void InitWindow();
		virtual void OnClick(TNotifyUI& msg);
		virtual void Notify(TNotifyUI& msg);
		virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
		virtual void OnFinalMessage(HWND hWnd);
	protected:
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		CMessageBoxUI(CUIString strText, CUIString strCaption, UINT uType)
			:m_strText(strText),
			m_strCaption(strCaption),
			m_uType(uType)
		{
			//SetCaretBlinkTime(UINT_MAX);
		}

		void SetCopyable(bool bCopy);
		void ShowWindow(bool bShow = true, bool bTakeFocus = true);
		UINT ShowModal();
	private:
		CUIString m_strCaption;
		CUIString m_strText;
		UINT m_uType;
	};

	/**
	hWnd [in, optional]
    Type: HWND
	A handle to the owner window of the message box to be created. If this parameter is NULL, the message box has no owner window.
	lpText [in, optional]
    Type: LPCTSTR
	The message to be displayed. If the string consists of more than one line, you can separate the lines using a carriage return and/or linefeed character between each line.
	lpCaption [in, optional]
	Type: LPCTSTR
	The dialog box title. If this parameter is NULL, the default title is Error.
	uType [in]
	Type: UINT 
	The contents and behavior of the dialog box. This parameter can be a combination of flags from the following groups of flags.
	Groups buttons: MB_OK,MB_YESNO,MB_YESNOCANCEL
	Groups icons: MB_ICONINFO,MB_ICONQUERY,MB_ICONERROR,MB_ICONWARNING
	bCopyable [in]
	Type: bool
	texts cpoyable.
	szWindow [in]
	Type: SIZE
	windows size.
	Return value
	Type: int
	Groups return value - IDCANCEL,IDNO,IDOK,IDYES
	**/
	static int CMessageBox(_In_opt_ HWND hWnd, _In_opt_ LPCTSTR lpText, _In_opt_ LPCTSTR lpCaption, _In_ UINT uType = MB_OK, _In_ bool bCopyable = false, _In_ SIZE szWindow = CSize(420,240))
	{
		CMessageBoxUI* pMessageBox = new CMessageBoxUI(lpText,lpCaption,uType);
		if( pMessageBox == NULL ) return 0;
		if(szWindow.cx < 300) szWindow.cx = 300;
		if(szWindow.cy < 200) szWindow.cy = 200;
		
		pMessageBox->Create(hWnd, _T("CMessageBoxUI"), UI_WNDSTYLE_FRAME, UI_WNDSTYLE_EX_DIALOG, 0, 0, szWindow.cx, szWindow.cy,0);
		pMessageBox->CenterWindow();
		pMessageBox->SetCopyable(bCopyable);
		return pMessageBox->ShowModal();
	}

} //namespace DirectUI


#endif //__MESSAGEBOXUI_H_