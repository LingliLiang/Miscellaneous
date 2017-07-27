#ifndef __UISCROLLVIEW_H__
#define __UISCROLLVIEW_H__

#pragma once
extern const TCHAR* DUI_CTR_SCROLLVIEW;

namespace DirectUI {
/////////////////////////////////////////////////////////////////////////////////////
//

class /*DirectUI_API*/ CScrollViewUI : public CContainerUI, public IListOwnerUI
{
public:
    CScrollViewUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void DoInit();
    UINT GetControlFlags() const;

    CUIString GetText() const;
    void SetEnabled(bool bEnable = true);

	virtual TListInfoUI* GetListInfo();
	virtual int GetCurSel() const;
	virtual bool SelectItem(int nIndex, bool bTakeFocus = false);
	virtual bool SelectRange(int nIndex, bool bTakeFocus = false) { return false; }
	virtual void MoveSelectedItemToHotItem() {}//移动项
	virtual void SetHotItemIndex(int nIndex){}
	virtual void SetMouseState(bool bDown){}
	virtual bool NeedCurSelectNotify() { return true; };

    bool SetItemIndex(CControlUI* pControl, int nIndex);

    bool Add(CControlUI* pControl);
    bool AddAt(CControlUI* pControl, int nIndex);
    bool Remove(CControlUI* pControl);
    bool RemoveAt(int nIndex);
    void RemoveAll();


    bool Activate();

    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);
    LPCTSTR GetNormalImage() const;
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage() const;
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage() const;
    void SetPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage() const;
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage() const;
    void SetDisabledImage(LPCTSTR pStrImage);
	LPCTSTR GetForeImage() const;
    void SetForeImage(LPCTSTR pStrImage);

    void SetItemFont(int index);
    void SetItemTextStyle(UINT uStyle);
	RECT GetItemTextPadding() const;
    void SetItemTextPadding(RECT rc);
	DWORD GetItemTextColor() const;
    void SetItemTextColor(DWORD dwTextColor);
	DWORD GetItemBkColor() const;
    void SetItemBkColor(DWORD dwBkColor);
	LPCTSTR GetItemBkImage() const;
    void SetItemBkImage(LPCTSTR pStrImage);
    bool IsAlternateBk() const;
    void SetAlternateBk(bool bAlternateBk);
	DWORD GetSelectedItemTextColor() const;
    void SetSelectedItemTextColor(DWORD dwTextColor);
	DWORD GetSelectedItemBkColor() const;
    void SetSelectedItemBkColor(DWORD dwBkColor);
	LPCTSTR GetSelectedItemImage() const;
    void SetSelectedItemImage(LPCTSTR pStrImage);
	DWORD GetHotItemTextColor() const;
    void SetHotItemTextColor(DWORD dwTextColor);
	DWORD GetHotItemBkColor() const;
    void SetHotItemBkColor(DWORD dwBkColor);
	LPCTSTR GetHotItemImage() const;
    void SetHotItemImage(LPCTSTR pStrImage);
	DWORD GetDisabledItemTextColor() const;
    void SetDisabledItemTextColor(DWORD dwTextColor);
	DWORD GetDisabledItemBkColor() const;
    void SetDisabledItemBkColor(DWORD dwBkColor);
	LPCTSTR GetDisabledItemImage() const;
    void SetDisabledItemImage(LPCTSTR pStrImage);
	DWORD GetItemLineColor() const;
    void SetItemLineColor(DWORD dwLineColor);
    bool IsItemShowHtml();
    void SetItemShowHtml(bool bShowHtml = true);
	//ItemSize 该属性只存储信息,不能改变控件实际大小
	SIZE GetItemSize();
	void SetItemSize(SIZE size);

    SIZE EstimateSize(SIZE szAvailable);
    void SetPos(RECT rc);
    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    
    void DoPaint(HDC hDC, const RECT& rcPaint);
    void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);

	void SetMouseWheelSelect(bool isMouseWheelSelect);
	bool GetMouseWheelSelect();

protected:
	void SetMouseWheeling(BOOL b);
	BOOL IsMouseWheeling();
	RECT CScrollViewUI::GetClipPaintRc();
	long GetSelectLineY(int index);
	long GetMoveLineY(); //make sure m_iCurSel invalid before call
	int GetPreIndex(int index);
	int GetNextIndex(int index);
protected:

    int m_iCurSel;
    UINT m_uButtonState;
    SIZE m_szItem; //该属性只存储信息,不能改变控件实际大小
    RECT m_rcTextPadding;
    CUIString m_sNormalImage;
    CUIString m_sHotImage;
    CUIString m_sPushedImage;
    CUIString m_sFocusedImage;
    CUIString m_sDisabledImage;
    CUIString m_sForeImage;

    TListInfoUI m_ListInfo;
	BOOL m_bMouseWheeling;
	bool m_bMouseWheelSelect;
	POINT m_oldPoint;
	int m_moveOffset;
	int m_ptYCorrect;
};

} // namespace DirectUI

#endif // __UISCROLLVIEW_H__
