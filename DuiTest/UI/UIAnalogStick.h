#ifndef __UIANALOGSTICK_H__
#define __UIANALOGSTICK_H__


extern const TCHAR* DUI_CTR_ANALOGSTICK;

namespace DirectUI
{
	class /*DirectUI_API*/ CAnalogStickUI : public CControlUI
	{
	public:
		CAnalogStickUI();
		~CAnalogStickUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		LPCTSTR GetNormalImage() const;
		void SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage() const;
		void SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetStickImage() const;
		void SetStickImage(LPCTSTR pStrImage);
		int	 GetRadius() const;
		void SetRadius(LPCTSTR pStrRadius);
		POINT GetCenterPoint() const;
		void SetCenterPoint(LPCTSTR pStrPoint);
		SIZE GetStickImageRowCol() const;
		void SetStickImageRowCol(SIZE szRowcol);
		RECT GetStickRect();
		void SetStickRect(RECT rc);
		bool Activate();
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		SIZE EstimateSize(SIZE szAvailable);

		void AdjustAngle(POINT &ptMove);

		void PaintStatusImage(HDC hDC);
	protected:
		RECT GetAnalogStickImageRect(unsigned int step);

		unsigned int m_nRadius;
		unsigned int m_nIterRadius;
		UINT m_uButtonState;
		Gdiplus::REAL m_fAngle;
		POINT m_ptCenter;
		POINT m_ptCurrent;
		unsigned int m_nAnalogStickCount;
		unsigned int m_nStep;
		SIZE m_szRowcol;
		RECT m_rcAnalogStickBase;

		std::basic_string<TCHAR> m_strNormalImage;
		std::basic_string<TCHAR> m_strHotImage;
		std::basic_string<TCHAR> m_strAnalogStick;
		std::basic_string<TCHAR> m_strDirection;
		HRGN m_hInteracRgn;
		//Gdiplus::Bitmap m_bmpAnalogStick;
	};
}

#endif //__UIANALOGSTICK_H__