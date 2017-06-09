#ifndef __UIEDITEMPTYVAR_H__
#define __UIEDITEMPTYVAR_H__

#pragma once

namespace DirectUI
{
	class CEditUI;
	class CEditEmptyVarUI;
	class CEditWnd;

	class DirectUI_API CEditEmptyVarUI : public CEditUI
	{
		friend class CEditWnd;
	public:
		CEditEmptyVarUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		LPCTSTR GetEmptyText();
		void SetEmptyText(LPCTSTR pStrText);
		void SetDropColor(DWORD dwTextColor);

		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintText(HDC hDC);
	protected:
		CUIString m_sEmptyText;
		DWORD	m_dwDropColor;
	};
}
#endif // __UIEDITEMPTYVAR_H__