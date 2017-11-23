#ifndef __UISPINBUTTON_H__
#define __UISPINBUTTON_H__


#define DUI_CTR_SPINBUTTON _T("SpinButton")

namespace DirectUI
{
	class /*DirectUI_API*/ CSpinButtonUI : public CControlUI
	{
	public:
		CSpinButtonUI();
		~CSpinButtonUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		LPCTSTR GetCapNormalImage() const;
		void SetCapNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetCapHotImage() const;
		void SetCapHotImage(LPCTSTR pStrImage);
		LPCTSTR GetCapPushImage() const;
		void SetCapPushImage(LPCTSTR pStrImage);
		LPCTSTR GetCapDisableImage() const;
		void SetCapDisableImage(LPCTSTR pStrImage);

		LPCTSTR GetRingNormalImage() const;
		void SetRingNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetRingHotImage() const;
		void SetRingHotImage(LPCTSTR pStrImage);
		LPCTSTR GetRingPushImage() const;
		void SetRingpPushImage(LPCTSTR pStrImage);
		LPCTSTR GetRingDisableImage() const;
		void SetRingDisableImage(LPCTSTR pStrImage);
		
		LPCTSTR GetCalibrationFrontImage() const;
		void SetCalibrationFrontImage(LPCTSTR pStrImage);
		LPCTSTR GetCalibrationBackImage() const;
		void SetCalibrationBackImage(LPCTSTR pStrImage);

		LPCTSTR GetFocusImage() const;
		void SetFocusImage(LPCTSTR pStrImage);
		LPCTSTR GetFrontImage() const;
		void SetFrontImage(LPCTSTR pStrImage);

		int	 GetRadius() const;
		void SetRadius(LPCTSTR pStrRadius);
		POINT GetCenterPoint() const;
		void SetCenterPoint(LPCTSTR pStrPoint);
		RECT GetStickRect();
		void SetStickRect(RECT rc);
		bool Activate();
		void DoEvent(TEventUI& tevent);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		SIZE EstimateSize(SIZE szAvailable);

		void AdjustAngle(POINT &ptMove);

		virtual void DoPaint(HDC hDC, const RECT& rcPaint);
		virtual void PaintStatusImage(HDC hDC);
		virtual void PaintText(HDC hDC);
	protected:
		RECT GetAnalogStickImageRect(unsigned int step);
		void GetImageProp(const CUIString& strImg,
			__out CUIString* pstrFile,
			__out Gdiplus::Rect* prcSrc,
			__out Gdiplus::Rect* prcDest);
		class CAngle {
		public:
			float CalculateAngle(
				float& fRoll,
				//float& fOffset, 
				const POINT ptMove,
				const POINT ptCenter,
				ULONG& ulDistance,
				BOOL bAddCre);
		private:
		};
		struct _tagGearProp
		{
			float fAngle;
			float fAngleInit;
			unsigned int nRadius;
			unsigned int nIterRadius;
			POINT ptCenter;
			HRGN hRgn;
			CAngle angle;
		};
		unsigned int m_nRadius;
		unsigned int m_nIterRadius;
		float m_fAngle;
		POINT m_ptCenter;
		POINT m_ptCurrent;
		unsigned int m_nStickStep;
		unsigned int m_nStep;

		CUIString m_strCalibrationBackImg;
		CUIString m_strCalibrationFrontImg;
		
		_tagGearProp  m_gpCap;		
		UINT m_uButtonState;
		CUIString m_strCapNormalImg;
		CUIString m_strCapHotImg;
		CUIString m_strCapPushImg;
		CUIString m_strCapDisableImg;

		_tagGearProp  m_gpOutRing;		
		UINT m_uOutButtonState;
		CUIString m_strRingNormalImg;
		CUIString m_strRingHotImg;
		CUIString m_strRingPushImg;
		CUIString m_strRingDisableImg;

		CUIString m_strFocusImg;
		CUIString m_strFrontImg;
		
		CUIString m_strDirection;
		HRGN m_hInteracRgn;

		std::unique_ptr<Gdiplus::Graphics> m_pGraphics;
		ULONG_PTR m_gdiplusToken;
		GdiplusStartupInput m_gdiplusStartupInput;
	};
}

#endif //__UISPINBUTTON_H__