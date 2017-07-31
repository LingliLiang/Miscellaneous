#pragma once

#include <directui\Core\UIManager.h>
#include "TouchUtils.h"
#include <map>
#include <memory>
namespace DirectUI {

	//TEventUI ��wKeyState�����Ƿ��������ص�
	typedef enum EVENTTYPE_TOUCH_UI
	{
		UIEVENT__TOUCHBEGIN = UIEVENT__LAST,
		//�������ص��UIEVENT_TAP_DOWN��Ϣ
		//���Ǳ�UIEVENT_BUTTONDOWN��һ������
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