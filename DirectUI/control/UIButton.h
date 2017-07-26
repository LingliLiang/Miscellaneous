#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#pragma once

namespace DirectUI
{
	class DirectUI_API CButtonUI : public CLabelUI
	{
	public:
		CButtonUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;
		UINT GetButtonState() const;

		bool Activate();
		void SetEnabled(bool bEnable = true);
		void DoEvent(TEventUI& event);

		LPCTSTR GetNormalImage();
		void SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage();
		void SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetPushedImage();
		void SetPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetFocusedImage();
		void SetFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetDisabledImage();
		void SetDisabledImage(LPCTSTR pStrImage);
		LPCTSTR GetForeImage();
		void SetForeImage(LPCTSTR pStrImage);
		LPCTSTR GetHotForeImage();
		void SetHotForeImage(LPCTSTR pStrImage);

		void SetHotBkColor(DWORD dwColor);
		DWORD GetHotBkColor() const;
		void SetHotTextColor(DWORD dwColor);
		DWORD GetHotTextColor() const;
		void SetPushedTextColor(DWORD dwColor);
		DWORD GetPushedTextColor() const;
		void SetFocusedTextColor(DWORD dwColor);
		DWORD GetFocusedTextColor() const;
		SIZE EstimateSize(SIZE szAvailable);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void SetHandCursor(BOOL bHand);
		BOOL IsHandCursor()const;

		void PaintText(HDC hDC);
		void PaintStatusImage(HDC hDC);

		enum BTN_IMGS{BTN_IMG_NORMAL=0,BTN_IMG_HOT,BTN_IMG_PUSHED,BTN_IMG_DISABLE,BTN_IMG_FOCUS,BTN_IMG_FORCE,BTN_IMG_HOTFORCE,};
		void ChangeNormalImageToOther(BTN_IMGS nStatus);
		void FlashBtn(int nTimerID, int nTime=250);
		void StopFlash();
		void OnFlashTimer();
	protected:
		UINT m_uButtonState;

		DWORD m_dwHotBkColor;
		DWORD m_dwHotTextColor;
		DWORD m_dwPushedTextColor;
		DWORD m_dwFocusedTextColor;

		CUIString m_sNormalImage;
		CUIString m_sHotImage;
		CUIString m_sHotForeImage;
		CUIString m_sPushedImage;
		CUIString m_sPushedForeImage;
		CUIString m_sFocusedImage;
		CUIString m_sDisabledImage;

		CUIString m_sFirstNormalImage;

		int m_nFlashTimerID;
		int m_nNowNormalID;
		BOOL m_bHandCursor;
	};

}	// namespace DirectUI

#endif // __UIBUTTON_H__