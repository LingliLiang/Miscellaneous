#ifndef __UICOMBOBOX_H__
#define __UICOMBOBOX_H__

#pragma once

namespace DirectUI
{
	/// ��չ�����б��
	/// ����arrowimage����,һ��ͼƬƽ���ֳ�5��,Normal/Hot/Pushed/Focused/Disabled(������source����), �Ƿ���ʾ�ı���
	/// <Default name="ComboBox" value="arrowimage=&quot;file='sys_combo_btn.png' source='0,0,16,16'&quot; textvisible="true" "/>
	class DirectUI_API CComboBoxUI : public CComboUI
	{
	public:
		CComboBoxUI();
		LPCTSTR GetClass() const;

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintText(HDC hDC);
		void PaintStatusImage(HDC hDC);

	protected:
		bool	   m_bPaintText;
		int        m_nArrowWidth;
		CUIString  m_sArrowImage;
	};
}

#endif // __UICOMBOBOX_H__
