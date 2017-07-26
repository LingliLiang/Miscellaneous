#ifndef __UICHECKBOX_H__
#define __UICHECKBOX_H__

#pragma once

namespace DirectUI
{
	/// ����ͨ�ĵ�ѡ��ť�ؼ���ֻ���ǡ������ֽ��
	/// ������COptionUI��ֻ��ÿ��ֻ��һ����ť���ѣ�����Ϊ�գ������ļ�Ĭ�����Ծ�����
	/// <CheckBox name="CheckBox" value="height='20' align='left' textpadding='24,0,0,0' normalimage='file='sys_check_btn.png' source='0,0,20,20' dest='0,0,20,20'' selectedimage='file='sys_check_btn.png' source='20,0,40,20' dest='0,0,20,20'' disabledimage='file='sys_check_btn.png' source='40,0,60,20' dest='0,0,20,20''"/>

	class DirectUI_API CCheckBoxUI : public COptionUI
	{
	public:
		CCheckBoxUI();

		LPCTSTR GetClass() const;

		void SetCheck(bool bCheck);
		bool GetCheck() const;

		void SetAutoCheck(bool isAutoCheck);
		bool GetAutoCheck() const;

		bool Activate();

		LPCTSTR GetSelectedNormalImage();
		void SetSelectedNormalImage(LPCTSTR pStrImage);

		LPCTSTR GetSelectedSelectedImage();
		void SetSelectedSelectedImage(LPCTSTR pStrImage);

		LPCTSTR GetSelectedDisabledImage();
		void SetSelectedDisabledImage(LPCTSTR pStrImage);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintStatusImage(HDC hDC);
	protected:
		CUIString		m_sSelectedNormalImage;
		CUIString		m_sSelectedSelectedImage;
		CUIString		m_sSelectedDisabledImage;
		bool            m_isAutoCheck;  //Ĭ�����Զ�ѡ��
	};
}

#endif // __UICHECKBOX_H__
