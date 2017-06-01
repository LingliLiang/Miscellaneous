#ifndef __NS_TEMP_H__
#define __NS_TEMP_H__

class CTemp : public CWindowUI
{

public:
	CTemp();
	~CTemp();

	LPCTSTR GetWindowClassName() const;

	virtual CUIString GetSkinFile();
	virtual CUIString GetSkinFolder();


};
#endif //__NS_TEMP_H__