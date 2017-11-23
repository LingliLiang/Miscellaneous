#ifndef __IWNDEVENT_H__
#define __IWNDEVENT_H__

class IWndEvent :
	public IDialogBuilderCallback
{
public:
	virtual void InitWindow() = 0;
	virtual bool Notify(TNotifyUI& msg) = 0;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) = 0;
	virtual bool OnClick(TNotifyUI& msg) = 0;
};


#endif //__IWNDEVENT_H__