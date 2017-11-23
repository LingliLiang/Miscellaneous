#ifndef __WNDEVENTIMPL_H__
#define __WNDEVENTIMPL_H__

#include "IWndEvent.h"

class CWndEventImpl : public IWndEvent
{
public:
	CWndEventImpl(HWND& hWnd);
	virtual ~CWndEventImpl();
	virtual void InitWindow();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual bool Notify(TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	virtual bool OnClick(TNotifyUI& msg);

	virtual LRESULT HandleWindowMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);
	virtual LRESULT HandleAddMoveNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle);

	bool OnInit(LPVOID lpControl);

	HWND& m_hWnd;
	//POPUP 类型的自窗口合集,用于窗口的移动联动
	std::vector<HWND> m_vecPopChild;
};


#endif //__WNDEVENTIMPL_H__