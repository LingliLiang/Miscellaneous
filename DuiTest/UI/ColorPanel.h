#ifndef _UI_COLORPANEL_H_
#define _UI_COLORPANEL_H_

namespace NSCGUI
{

#define COLORPANEL_MSGTYPE_SELECT _T("colorpanel_select")   //wparam为COLORREF值 lparam alpha
#define COLORPANEL_MSGTYPE_ALPHA _T("colorpanel_alpha")  //wparam为透明值(0-255)

/**
*
*CColorChunkUI
*
*/
class CColorChunkUI : public CControlUI
{
public:
	CColorChunkUI();
	virtual void DoEvent(TEventUI& event);
	virtual void DoPaint(HDC hDC, const RECT& rcPaint);
	virtual LPVOID GetInterface(LPCTSTR pstrName);

	virtual void SetOwner(INotifyUI * pOwner);

	virtual void Select(bool bSelect);
	virtual bool IsSelected();

	virtual  void SetDwColor(DWORD color);
	virtual  void SetColor(COLORREF color);

	virtual COLORREF GetColor();
	
protected:
	INotifyUI * m_pOwner;

	bool m_bSelect;
	bool m_bHot;
};



/**
*
*CHSVContainerUI : The color gradient for Hue  Saturation and Value
*
*/
class CHSVContainerUI : public CVerticalLayoutUI
{
public:
	CHSVContainerUI();
	void Init();
	virtual void DoEvent(TEventUI& event);
	void DoPaint(HDC hDC, const RECT& rcPaint);
	void SetPos(RECT rc);
	void SetSelectColor(COLORREF color);
	COLORREF GetSelectColor();
	void SetHue(UINT nHue); //0~360
	UINT GetHue(); //0~360

	void SetSelecterImage(LPCTSTR strImage, SHORT nWidth, SHORT nHeight);
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	COLORREF hsvToRgb(double h, double s, double v);
	void Rgb2Hsv(double R, double G, double B, double& H, double& S, double&V);
protected:

	void PaintColorRect(HDC hDC, const RECT& rcColor, const RECT& rcItem, double Hue);
	void PaintColorRect2(HDC hDC, const RECT& rcPaint, const RECT& rcColor , COLORREF color);
	void PaintColorSelecter(HDC hDC);
private:
	void SetSelecterPos();
	void SelectColor();
	void CalculateClickPos(POINT ptMouse);
protected:
	bool m_bInit;
	UINT m_uiWidth;
	UINT m_uiHeight;
	double m_dHue; ///the value of hue ,range [0.0,1.0]
	double m_dAValue; //the value of (S)y ,range [1.0,0.0]
	double m_dBValue; //the value of (V)x, range [0.0,1.0]
	POINT m_ptSelect;

	COLORREF m_selColor;

	CUIString m_strImageSelecter;
	SHORT m_nSelWidth;
	SHORT m_nSelHeight;

	DirectUI::CRect m_rcColor; //paint Color rect

private:
	bool m_bLBDown;
};


/**
*
*CColorPanel
*描述: 调色板对话框
*
*/
class CColorPanel : public CWindowUI
{
public:
	CColorPanel();
	/**
	*autoUpate : true: update on change false: upadte on close panel
	*color : init color 
	*/
	virtual void Init(HWND hwndParent, POINT point, CPaintManagerUI* pManager = NULL,  DWORD color = 0xFF000000, bool autoUpate = false);
	
	void SetDisContorl(CControlUI* pDisable);

	virtual void InitWindow();
	virtual CUIString GetSkinFolder();
	virtual CUIString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void Notify(TNotifyUI& msg);
	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnFinalMessage(HWND hWnd);

	void SetValue(DWORD dwValue);
	void SetValue(BYTE dwAlpha, COLORREF color);

	DWORD GetValue();
	COLORREF GetColor();
	BYTE GetAlpha();
private:
	bool OnHSVNotify(LPVOID notify);
	inline void SetColorEditText();
	
	CPaintManagerUI * m_pManager;

protected:
	void SendNotify(CUIString strMsg,WPARAM wParam = 0,LPARAM lParam = 0);

	COLORREF GetColorByValue(DWORD dwValue);
	BYTE GetAlphaByValue(DWORD dwValue);

	CUIString m_strClassName;

	DWORD m_dwValue; //value of color and  alpha
	CEditUI * m_pValueEdit; //Hex edit
	CHSVContainerUI * m_pHSVContainer;
	CSliderUI * m_pHueSlider;//
	CSliderUI * m_pAlphaSlider;

	bool m_bAutoUpate;

	CControlUI* m_pHitControl;
};

} //namespace NSCGUI

#endif //_UI_COLORPANEL_H_