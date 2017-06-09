#ifndef _PRELOADER_WIN_H_
#define _PRELOADER_WIN_H_

//CPreloader
//icon from
//http://preloaders.net/en/free
//http://ajaxload.info/
class CPreloaderUI : public CWindowUI
{
public:
	CPreloaderUI(HWND hParent);
	~CPreloaderUI(){};
public:
	LPCTSTR GetWindowClassName() const;
	virtual CUIString GetSkinFile();
	virtual CUIString GetSkinFolder();
	virtual void InitWindow();
	virtual void OnFinalMessage(HWND hWnd);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Notify(TNotifyUI& msg);
	void SetLoaderText(LPCTSTR lpszText);
private:
	HWND m_hParentWnd;
	CLabelEx* m_pLoaderIcon;
	CTextUI* m_pLoaderText;
};

//CPreloaderWin
class CPreloaderWin : public CWindowWnd
{
public:
	CPreloaderWin(HWND hParent = NULL);
	void SetLoaderText(LPCTSTR lpszText);
	void SetThreadHandle(HANDLE hThread){m_hThread = hThread;}
protected:
	virtual LPCTSTR GetWindowClassName() const;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	CPreloaderUI* m_pLoaderUI;
	HANDLE m_hThread;
};


#endif //_PRELOADER_WIN_H_
