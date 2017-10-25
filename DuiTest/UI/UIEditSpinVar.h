#ifndef __UIEDITEMPTYVAR_H__
#define __UIEDITEMPTYVAR_H__

#pragma once

namespace DirectUI
{
	class CEditUI;
	class CEditWnd;
	/**
	<EditSpinVar parent="Edit" notifies="click setfocus killfocus timer menu return textchanged windowinit(root)">
	<!--ע��: �ÿؼ�����SPIN������ϢDUI_MSGTYPE_CLICK, wparam Ϊ spin��ťid,0Ϊ����ť,1Ϊ����ť, lparamΪ�Ƿ��������Զ�����-->
	<Attribute name="emptytext" default="" type="STRING" comment="Ϊ��ʱ��ʾ���ִ�"/>
	<Attribute name="etdropcolor" default="0x00000000" type="DWORD" comment="Ϊ��ʱ��ǰ������ɫ��ɫ�ĳ̶�,��(0x00111111)"/>
	<Attribute name="headtext" default="" type="STRING" comment="��ʾ�ٱ༭����ߵ�˵���ִ�, �� [ ����headtext:      ������ʾ�ִ� ]"/>
	<Attribute name="enablespin" default="true" type="BOOL" comment="�Ƿ�����SPIN��ť,��(false)"/>
	<Attribute name="enablemaxmin" default="false" type="BOOL" comment="�Ƿ������������͵������Сֵ�ж�,��vmaxmin���ʹ��,��(false)"/>
	<Attribute name="autospin" default="false" type="BOOL" comment="�Ƿ�������������ֵ,��(false), ������SPIN��ťʱ������ֵ(�������ͼ�����������Ҫ����)��������"/>
	<Attribute name="vtype" default="0" type="INT" comment="���������ñ��������κ�v��ĸ��ͷ������,��������, 0-ԭʼ 1-INT 2-double"/>
	<Attribute name="value" default="0" type="INT,DOUBLE" comment="�������ͱ��������,��������ֵ, ��(1��1.00)"/>
	<Attribute name="vstep" default="0.0" type="DOUBLE" comment="���ݲ�������ֵ, ��(1.00)"/>
	<Attribute name="precise" default="1" type="INT" comment="double���ݾ���, С����λ��"/>
	<Attribute name="vmaxmin" default="0,0" type="SIZE[INT,DOUBLE]" comment="���������Сֵ [max,min], ��(100,0)(100.0,0.0)"/>
	<Attribute name="spinslide" default="0,0" type="SIZE" comment="SPIN��ťͼƬ���, ��(20,20)"/>
	<Attribute name="spinalign" default="2" type="INT" comment="SPIN��ť���뷽ʽ, 0-��������� 1-���ҷֱ����һ�� 2-�������ұ�"/>
	<Attribute name="spin1normalimage" default="" type="STRING" comment="SPIN��ť״̬ͼƬ"/>
	<Attribute name="spin1hotimage" default="" type="STRING" comment="SPIN��ť״̬ͼƬ"/>
	<Attribute name="spin1pushedimage" default="" type="STRING" comment="SPIN��ť״̬ͼƬ"/>
	<Attribute name="spin1disabledimage" default="" type="STRING" comment="SPIN��ť״̬ͼƬ"/>
	<Attribute name="spin2normalimage" default="" type="STRING" comment="SPIN��ť״̬ͼƬ"/>
	<Attribute name="spin2hotimage" default="" type="STRING" comment="SPIN��ť״̬ͼƬ"/>
	<Attribute name="spin2pushedimage" default="" type="STRING" comment="SPIN��ť״̬ͼƬ"/>
	<Attribute name="spin2disabledimage" default="" type="STRING" comment="SPIN��ť״̬ͼƬ"/>
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

		CUIString m_sHeadText; // ָ����ʾ���󲿵�˵������
		CUIString m_sEmptyText; //Ϊ��ʱ��ʾ������
		DWORD	m_dwDropColor; //EmptyText����ɫ

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
		//����������ť
		BOOL m_bEnableSpin; //����Spin��ť
		BOOL m_bAutoSpin; //������ֵ������
		BOOL m_bEnableMaxMin; //����MaxMin��ֵ
		UINT m_uSpinAlign; // left/right/split ���뷽ʽ
		SIZE m_uSpinSlide; // ��С
		UINT m_uSpin1State;
		UINT m_uSpin2State;
		CRect m_rcSpin[2];
		CUIString m_sSpin1Image[4];
		CUIString m_sSpin2Image[4];

	};
} //namespace DirectUI
#endif // __UIEDITEMPTYVAR_H__