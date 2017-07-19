#ifndef __UISLIDERCONTROL_H__
#define __UISLIDERCONTROL_H__

#pragma once

extern const TCHAR* const DUI_CTR_SLIDERCONTROL;

namespace DirectUI
{
	class /*DirectUI_API*/ CSliderControlUI : public CProgressUI
	{
	public:
		CSliderControlUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetEnabled(bool bEnable = true);

		int GetChangeStep();
		void SetChangeStep(int step);
		void SetThumbSize(SIZE szXY);
		void SetThumbCenter(SIZE szXY);
		RECT GetThumbRect() const;
		LPCTSTR GetThumbImage() const;
		void SetThumbImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbHotImage() const;
		void SetThumbHotImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbPushedImage() const;
		void SetThumbPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetForeBkImage() const;
		void SetForeBkImage(LPCTSTR pStrImage);
		void SetReverse(bool b);
		bool GetReverse() const;

		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintStatusImage(HDC hDC);
		RECT GetSlotInset() const;
		void SetSlotInset(RECT rc);
		RECT CalcSlotRc() const;
		SIZE EstimateSize(SIZE szAvailable);
	protected:
		SIZE m_szThumb;
		UINT m_uButtonState;
		int m_nStep;
		bool m_bDoubleClicked;
		bool m_bReverse;

		CUIString m_sThumbImage;
		CUIString m_sThumbHotImage;
		CUIString m_sThumbPushedImage;
		CUIString m_sForeBkImage;

		CUIString m_sImageModify;
		CSize m_szThumbCenter;
		CRect m_rcSlotInset;
	};
}

#endif // __UISLIDERCONTROL_H__