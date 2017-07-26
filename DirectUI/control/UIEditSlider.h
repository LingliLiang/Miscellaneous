
#ifndef __UIEDITSLIDER_H_
#define __UIEDITSLIDER_H_

#pragma once

namespace DirectUI
{
	class DirectUI_API CEditSliderUI : public CVerticalLayoutUI
	{
	public:
		CEditSliderUI();
		~CEditSliderUI();
		enum {DEFAULT_TIMER_ID=1001,DEFAULT_TIME=500,};
		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void SetEnabled(bool bEnable = true);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetSliderName(LPCTSTR pstrName);
		LPCTSTR GetSliderName() const;
		void SetEditName(LPCTSTR pstrName);
		LPCTSTR GetEditName() const;
		void SetEditState(bool bEdit);
		bool IsEditState() const;
		void SetDefaultValue(float fValue);
		float GetDefaultValue() const;
		void SetMultipleValue(float fValue);
		float GetMultipleValue() const;
		bool OnNotify(void *pNotify);
	protected:
		CUIString m_strSliderName;
		CUIString m_strEditName;
		CEditUI* m_pEdit;
		CSliderUI* m_pSlider;
		bool m_bEditState;
		bool m_bInitDone;
		bool m_bEditFocus;
		float m_fDefaultValue;
		float m_fMultipleValue;
	};
}


#endif //__UIEDITSLIDER_H_