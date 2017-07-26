#ifndef __HOTKEYCTRLUI_H__
#define __HOTKEYCTRLUI_H__

#pragma once
namespace DirectUI
{
class DirectUI_API CHotKeyCtrlUI : public CLabelUI
{
public:
	CHotKeyCtrlUI(void);
	~CHotKeyCtrlUI(void);

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void SetFocusedImage(LPCTSTR szImg);

	void DoEvent(TEventUI& event);
	void SetKeyCode(DWORD dwKeyCode);
	DWORD GetKeyCode()const {return m_dwKeyCode;}

	void PaintStatusImage(HDC hDC);

protected:
	void ParserCode(DWORD dwKey);
protected:
	DWORD m_dwKeyCode;
	CUIString m_sFocusedImage;
};
}
#endif
