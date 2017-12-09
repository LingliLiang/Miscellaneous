#ifndef __UISPINBUTTON_H__
#define __UISPINBUTTON_H__


#define DUI_CTR_SPINBUTTON _T("SpinButton")

namespace DirectUI
{
	class CAngleRecord {
	public:
		CAngleRecord();
		float CalculateAngle(
			float* pfRoll,
			const POINT ptMove,
			const POINT ptCenter,
			ULONG* pulDistance,
			BOOL bFirst);
	private:
		int m_nQuadrant;
		BOOL m_b4to1;//第四象限到第一象限的移动
		BOOL m_b1to4; //第一象限到第四象限的移动
		float m_fOffsetAngle; //当前计算的角度矫正值
		BOOL m_bClockwise; //移动的趋势,是否顺时钟
		float m_fLastAngle; //记录最后一次更新的角度
		float m_fFirstAngle; //记录第一次计算的角度
	};

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

		int	 GetCapRadius() const;
		void SetCapRadius(UINT nRadius); //reset Radius rgn
		int	 GetRingRadius() const;
		void SetRingRadius(UINT nRadius);//reset Radius rgn
		int	 GetRingInnerRadius() const;
		void SetRingInnerRadius(UINT nRadius); // call before SetRingRadius
		void SetCapMax(float fValue);
		float GetCapMax() const;
		void SetRingMax(float fValue);
		float GetRingMax() const;
		void SetCapMin(float fValue);
		float GetCapMin() const;
		void SetRingMin(float fValue);
		float GetRingMin() const;
		void SetCapSpace(float fValue);
		float GetCapSpace() const;
		void SetRingSpace(float fValue);
		float GetRingSpace() const;

		POINT GetCenterPoint() const;

		bool Activate();
		void DoEvent(TEventUI& tevent);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		SIZE EstimateSize(SIZE szAvailable);
		 virtual void SetPos(RECT rc);

		virtual void DoPaint(HDC hDC, const RECT& rcPaint);
		virtual void PaintStatusImage(HDC hDC);
		virtual void PaintText(HDC hDC);
	protected:
		struct _tagGearProp
		{
			float fMax;
			float fMin;
			float fSpace;
			float fAngle;
			float fAngleInit;
			unsigned int nRadius;
			unsigned int nInnerRadius;
			POINT ptCenter;
			HRGN hRgn;
			CAngleRecord angle;
		};

		RECT GetAnalogStickImageRect(unsigned int step);
		void GetImageProp(const CUIString& strImg,
			__out CUIString* pstrFile,
			__out Gdiplus::Rect* prcSrc,
			__out Gdiplus::Rect* prcDest,
			__out int* pAlpha);
		void AdjustAngle(POINT &ptMove, _tagGearProp& gp, BOOL isbegin);

		bool PtMouseInRegion(HRGN& hRgn, const POINT& ptMouse, _tagGearProp& gp);

		POINT m_ptCenter;
		POINT m_ptCurrent;

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

		std::unique_ptr<Gdiplus::Graphics> m_pGraphics;
		ULONG_PTR m_gdiplusToken;
		GdiplusStartupInput m_gdiplusStartupInput;
	};
}

#endif //__UISPINBUTTON_H__