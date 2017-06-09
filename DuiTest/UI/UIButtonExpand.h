#ifndef __UIBUTTONEXPAND_H__
#define __UIBUTTONEXPAND_H__

#pragma once

namespace DirectUI
{
	class DirectUI_API CButtonExpandUI : public CButtonUI
	{
	public:
		CButtonExpandUI();
		~CButtonExpandUI();
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetButtonState() const;

		void SetEnabled(bool bEnable = true);
		void DoEvent(TEventUI& event);

		LPCTSTR GetExpandNormalImage();
		void SetExpandNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetExpandHotImage();
		void SetExpandHotImage(LPCTSTR pStrImage);
		LPCTSTR GetExpandPushedImage();
		void SetExpandPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetExpandFocusedImage();
		void SetExpandFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetExpandDisabledImage();
		void SetExpandDisabledImage(LPCTSTR pStrImage);
		LPCTSTR GetExpandForeImage();
		void SetExpandForeImage(LPCTSTR pStrImage);
		LPCTSTR GetExpandHotForeImage();
		void SetExpandHotForeImage(LPCTSTR pStrImage);
		LPCTSTR GetExpandPushedForeImage();
		void SetExpandPushedForeImage(LPCTSTR pStrImage);

		LPCTSTR GetFlashHotImage();
		void SetFlashHotImage(LPCTSTR pStrImage);
		LPCTSTR GetFlashPushedImage();
		void SetFlashPushedImage(LPCTSTR pStrImage);


		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintText(HDC hDC);
		void PaintStatusImage(HDC hDC);

		enum BTN_FLASH_IMGS{BTN_FLASH_NORMAL=1,BTN_FLASH_HOT,BTN_FLASH_PUSHED, BTN_FLASH};
		void ChangeNormalImageToOther2(BTN_FLASH_IMGS nStatus);
		void FlashBtn(UINT nCount = 24, int nTime = 250);
		void OnFlashTimer2();
		void Expand();
	protected:
		RECT GetExpandItemRect();
		RECT GetItemRect();
		void OnExpand();
		bool PtInExpendRect(POINT pt);
		bool PtInItemRect(POINT pt);


		CUIString m_sExpandNormalImage;
		CUIString m_sExpandForeImage;
		CUIString m_sExpandHotImage;
		CUIString m_sExpandHotForeImage;
		CUIString m_sExpandPushedImage;
		CUIString m_sExpandPushedForeImage;
		CUIString m_sExpandFocusedImage;
		CUIString m_sExpandDisabledImage;

		CUIString m_sFlashNormalImage;
		CUIString m_sFlashHotImage;
		CUIString m_sFlashPushedImage;

		UINT m_uExpandButtonState;

#define FLASHTIMERID_2 0x0c000
		int m_nFlashTimerID_2;
		int m_nNowNormalID_2;
#define EXPENDBUTTON 0x0a000
		HRGN m_hExpendRgn;
		std::vector<POINT>* m_vecExpendRgn;
		SIZE m_oldFixed;
		UINT m_nExpandAnchor;
		BOOL m_bCollapse;
		BOOL m_bLeft;
	};

}	// namespace DirectUI

#endif // __UIBUTTONEXPAND_H__