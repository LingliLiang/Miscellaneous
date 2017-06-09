#ifndef __AUTOLAYOUTUI_H__
#define __AUTOLAYOUTUI_H__

#pragma once

extern const TCHAR* DUI_CTR_AUTOLAYOUT;

namespace DirectUI
{

	class CImagePaint
	{
	public:
		void SetSelImage(LPCTSTR pstrImage);
		CUIString GetSelImage();
		void SetHotImage(LPCTSTR pstrImage);
		CUIString GetHotImage();
		virtual void PaintBkImage(HDC hDC);
		virtual void PaintStatusImage(HDC hDC);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void DoImgPaint(HDC hDC, const RECT& rcPaint);
	protected:
		CUIString m_strSelImg;
		CUIString m_strHotImg;
	};

	class /*DirectUI_API*/ CAutoLayoutUI : public CContainerUI
	{
	public:
		enum LayoutMode{
			Vertical,
			Horizontal,
			FitMaxLength,
			Tile
		};

	public:
		CAutoLayoutUI();
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
	public:

		void SetMode(size_t nLayoutMode);
		size_t GetMode() const;

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetItemAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void DoEvent(TEventUI& event);

		void SetPos(RECT rc);
		void DoPaint(HDC hDC, const RECT& rcPaint);
		void DoSelPaint(HDC hDC, const RECT& rcPaint);
		void DoHotPaint(HDC hDC, const RECT& rcPaint);

		virtual SIZE GetScrollPos() const;
		virtual SIZE GetScrollRange() const;
		virtual void SetScrollPos(SIZE szPos);
		virtual void PageUp();
		virtual void PageDown();
		virtual void PageLeft();
		virtual void PageRight();

		int GetCurSel() const;
		CControlUI* GetCurSel();
		bool SelectItem(int nIndex, bool bTakeFocus = false);
		void EnsureVisible(int nIndex);

		bool DrawImage(HDC hDC,const RECT &rcItem, LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);

		void LoadItemBkImage(CUIString strFormat);

		void ClearAll();
		bool CheckRectInvalid(RECT &rc);
	protected:

		virtual void ProcessScrollBar(RECT rc, int cxRequired, int cyRequired);
		virtual CControlUI* PtHitControl(POINT ptMouse, int &nIndex);
		
		CControlUI* m_pSelControl;
		CControlUI* m_pHotControl;
		size_t m_bMode;
		int m_iSepWidth;
		UINT m_uButtonState;
		POINT ptLastMouse;
		RECT m_rcNewPos;
		bool m_bImmMode;

		int m_iCurSel;
		bool m_bEnsureVisible; //选中的时候是否移动位置

	};
}
#endif // __LAYERLISTUI_H__
