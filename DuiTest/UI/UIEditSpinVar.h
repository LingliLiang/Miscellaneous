#ifndef __UIEDITEMPTYVAR_H__
#define __UIEDITEMPTYVAR_H__

#pragma once

namespace DirectUI
{
	class CEditUI;
	class CEditWnd;
	/**
	<EditSpinVar parent="Edit" notifies="click setfocus killfocus timer menu return textchanged windowinit(root)">
	<!--注意: 该控件按下SPIN会有消息DUI_MSGTYPE_CLICK, wparam 为 spin按钮id,0为增按钮,1为减按钮, lparam为是否设置了自动增减-->
	<Attribute name="emptytext" default="" type="STRING" comment="为空时显示的字串"/>
	<Attribute name="etdropcolor" default="0x00000000" type="DWORD" comment="为空时当前字体颜色退色的程度,如(0x00111111)"/>
	<Attribute name="headtext" default="" type="STRING" comment="显示再编辑框左边的说明字串, 如 [ 这是headtext:      这是显示字串 ]"/>
	<Attribute name="enablespin" default="true" type="BOOL" comment="是否启用SPIN按钮,如(false)"/>
	<Attribute name="enablemaxmin" default="false" type="BOOL" comment="是否启用数据类型的最大最小值判定,与vmaxmin配合使用,如(false)"/>
	<Attribute name="autospin" default="false" type="BOOL" comment="是否启用自增减数值,如(false), 当按下SPIN按钮时进行数值(数据类型及步伐幅度需要设置)的自增减"/>
	<Attribute name="vtype" default="0" type="INT" comment="该类型设置必须先于任何v字母开头的设置,数据类型, 0-原始 1-INT 2-double"/>
	<Attribute name="value" default="0" type="INT,DOUBLE" comment="数据类型必须先设好,数据类型值, 如(1或1.00)"/>
	<Attribute name="vstep" default="0.0" type="DOUBLE" comment="数据步伐幅度值, 如(1.00)"/>
	<Attribute name="precise" default="1" type="INT" comment="double数据精度, 小数点位置"/>
	<Attribute name="vmaxmin" default="0,0" type="SIZE[INT,DOUBLE]" comment="数据最大最小值 [max,min], 如(100,0)(100.0,0.0)"/>
	<Attribute name="spinslide" default="0,0" type="SIZE" comment="SPIN按钮图片宽高, 如(20,20)"/>
	<Attribute name="spinalign" default="2" type="INT" comment="SPIN按钮对齐方式, 0-放置在左边 1-左右分别放置一个 2-放置在右边"/>
	<Attribute name="spin1normalimage" default="" type="STRING" comment="SPIN按钮状态图片"/>
	<Attribute name="spin1hotimage" default="" type="STRING" comment="SPIN按钮状态图片"/>
	<Attribute name="spin1pushedimage" default="" type="STRING" comment="SPIN按钮状态图片"/>
	<Attribute name="spin1disabledimage" default="" type="STRING" comment="SPIN按钮状态图片"/>
	<Attribute name="spin2normalimage" default="" type="STRING" comment="SPIN按钮状态图片"/>
	<Attribute name="spin2hotimage" default="" type="STRING" comment="SPIN按钮状态图片"/>
	<Attribute name="spin2pushedimage" default="" type="STRING" comment="SPIN按钮状态图片"/>
	<Attribute name="spin2disabledimage" default="" type="STRING" comment="SPIN按钮状态图片"/>
	</EditSpinVar>
	**/
	class DirectUI_API CEditSpinVarUI : public CEditUI
	{
		friend class CEditWnd;
	public:
		CEditSpinVarUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		LPCTSTR GetEmptyText();
		void SetEmptyText(LPCTSTR pStrText);
		void SetDropColor(DWORD dwTextColor);

		void DoEvent(TEventUI& eventui);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		virtual void PaintStatusImage(HDC hDC);
		virtual void PaintText(HDC hDC);
		virtual void SetPos(RECT rc);
		virtual void SetText(LPCTSTR pstrText);

		LPCTSTR GetSpin1NormalImage();
		void SetSpin1NormalImage(LPCTSTR pStrImage);
		LPCTSTR GetSpin1HotImage();
		void SetSpin1HotImage(LPCTSTR pStrImage);
		LPCTSTR GetSpin1PushedImage();
		void SetSpin1PushedImage(LPCTSTR pStrImage);
		LPCTSTR GetSpin1DisabledImage();
		void SetSpin1DisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetSpin2NormalImage();
		void SetSpin2NormalImage(LPCTSTR pStrImage);
		LPCTSTR GetSpin2HotImage();
		void SetSpin2HotImage(LPCTSTR pStrImage);
		LPCTSTR GetSpin2PushedImage();
		void SetSpin2PushedImage(LPCTSTR pStrImage);
		LPCTSTR GetSpin2DisabledImage();
		void SetSpin2DisabledImage(LPCTSTR pStrImage);

		void SetHeadText(LPCTSTR pStr);
		LPCTSTR GetHeadText();

		/*0 - raw ,1 - int ,2 - double*/
		void SetValueType(unsigned int type, unsigned short precise = 1);
		unsigned int GetValueType() const;
		bool SetValue(int value);
		bool SetValue(double value);
		int GetValue() const;
		double GetValue(double) const;
		bool SetMaxValue(int value);
		bool SetMaxValue(double value);
		int GetMaxValue() const;
		double GetMaxValue(double) const;
		bool SetMinValue(int value);
		bool SetMinValue(double value);
		int GetMinValue() const;
		double GetMinValue(double) const;
		void SetValueStep(double value);
	protected:
		void CalcSpinPos();
		void PaintSpin(HDC& hDC, UINT state, CUIString (&spinImg)[4], RECT rcPaint);
		void PaintImg(HDC& hDC, CUIString &strImg, RECT* rectPaint = NULL);
		void ActiveSpin(int index);

		bool OnInnerNotify(LPVOID lpMsg);
		void ChangedText();

		CUIString m_sHeadText; // 指定显示在左部的说明文字
		CUIString m_sEmptyText; //为空时显示的文字
		DWORD	m_dwDropColor; //EmptyText的颜色

		enum BTN_INDEX{ID_NOR, ID_HOT, ID_PUSH,ID_DIS};
		enum SPIN_ALIGN{AL_LEFT = 1, AL_RIGHT, AL_SPLIT};
		union uiVar{
			int varInt;
			double varDouble;
		}; 
		uiVar m_unVar;
		uiVar m_unMinVar;
		uiVar m_unMaxVar;
		UINT m_varType; // 0 - raw 1 - int 2 - double
		unsigned short m_varPrecise; // varDouble
		double m_varStep;
		//增减计数按钮
		BOOL m_bEnableSpin; //启用Spin按钮
		BOOL m_bAutoSpin; //启用数值自增减
		BOOL m_bEnableMaxMin; //启用MaxMin数值
		UINT m_uSpinAlign; // left/right/split 对齐方式
		SIZE m_uSpinSlide; // 大小
		UINT m_uSpin1State;
		UINT m_uSpin2State;
		CRect m_rcSpin[2];
		CUIString m_sSpin1Image[4];
		CUIString m_sSpin2Image[4];

	};
} //namespace DirectUI
#endif // __UIEDITEMPTYVAR_H__