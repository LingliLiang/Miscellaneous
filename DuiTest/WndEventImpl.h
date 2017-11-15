#ifndef __WNDEVENTIMPL_H__
#define __WNDEVENTIMPL_H__

#include "IWndEvent.h"

class CWndEventImpl : public IWndEvent
{
public:
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual bool Notify(TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual bool OnClick(TNotifyUI& msg);
};


#endif //__WNDEVENTIMPL_H__