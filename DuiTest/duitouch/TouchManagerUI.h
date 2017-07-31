#pragma once

#include <directui\Core\UIManager.h>
#include "TouchUtils.h"
#include <map>
#include <memory>
namespace DirectUI {

	//TEventUI 的wKeyState标明是非是主触控点
	typedef enum EVENTTYPE_TOUCH_UI
	{
		UIEVENT__TOUCHBEGIN = UIEVENT__LAST,
		//在主触控点的UIEVENT_TAP_DOWN消息
		//总是比UIEVENT_BUTTONDOWN快一步出现
		UIEVENT_TAP_DOWN,
		UIEVENT_TAP_UP,
		UIEVENT_TAP,
		UIEVENT_DBTAP,
		UIEVENT_PAN,
		UIEVENT_ROTAE,
		UIEVENT_ZOOM,
		UIEVENT_PRESSHOLD,
		UIEVENT__TOUCHEND
	};

	class CTouchManagerUI
		: public CPaintManagerUI,
		IHandleTouchInput
	{
	public:
		CTouchManagerUI(void);
		~CTouchManagerUI(void);

		bool IsMultiTouchMode() const;
		void EnableMultiTouch(bool bEnable);


		bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	public:
		virtual bool HandleTouchInput(__in POINT ptScreen, PTOUCHINPUT pIn);
		virtual bool HandleGestureInput(PGESTUREINFO pIn);
		std::unique_ptr<TouchUtils> m_uniTouchUtils;
	protected:
		bool TouchMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);

		struct TouchInfo
		{
			CControlUI* pControl;
			POINT oldTouchPt;
			TouchInfo():pControl(NULL)
			{
				ZeroMemory(&oldTouchPt,sizeof(POINT));
			}
		};
		typedef DWORD TOUCHID;
		std::map<TOUCHID,TouchInfo> m_mapTouchFocus;
		BOOL m_bMultiTouch;
	};
}