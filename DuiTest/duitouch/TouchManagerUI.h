#pragma once

#include <..\directui\Core\UIManager.h>
#include "TouchUtils.h"
#include <map>
namespace DirectUI {

	typedef enum EVENTTYPE_TOUCH_UI
	{
		UIEVENT__TOUCHBEGIN = UIEVENT__LAST,
		UIEVENT__TAP,
		UIEVENT_DBTAP,
		UIEVENT_PAN,
		UIEVENT_ROTAE,
		UIEVENT_ZOOM,
		UIEVENT_PRESSHOLD,
		UIEVENT__TOUCHEND
	};

	class CTouchManagerUI
		: public CPaintManagerUI
	{
	public:
		CTouchManagerUI(void);
		~CTouchManagerUI(void);

		bool IsMultiTouchMode() const;
		void EnableMultiTouch(bool bEnable);


		bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	public:
		std::unique_ptr<TouchUtils> m_uniTouchUtils;
	protected:
		typedef DWORD TOUCHID;
		std::map<TOUCHID,CControlUI*> m_mapTouchFocus;
		BOOL m_bMultiTouch;

	};
}