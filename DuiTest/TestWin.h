#ifndef __NS_TEMP_H__
#define __NS_TEMP_H__

class CTestWin : public CWindowUI
{

public:
	CTestWin(){}
	~CTestWin(){}

	LPCTSTR GetWindowClassName() const;

	virtual CUIString GetSkinFile();
	virtual CUIString GetSkinFolder();
	
virtual CControlUI* CreateControl(LPCTSTR pstrClass);
virtual void OnClick(TNotifyUI& msg);

virtual void Drop();
};
#endif //__NS_TEMP_H__