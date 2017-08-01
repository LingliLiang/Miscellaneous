#pragma once

#define DUI_CTR_BRICKSTAB _T("BricksTab")
#define DUI_CTR_OPTIONCONTAINER _T("OptionContainer")

namespace DirectUI
{

	class COptionContainerUI : public COptionUI , public IContainerUI
	{
	public:
		virtual CControlUI* GetItemAt(int nIndex) const;
		virtual int GetItemIndex(CControlUI* pControl) const;
		virtual bool SetItemIndex(CControlUI* pControl, int nIndex);
		virtual int GetCount() const;
		virtual bool Add(CControlUI* pControl);
		virtual bool AddAt(CControlUI* pControl, int nIndex);
		virtual bool Remove(CControlUI* pControl);
		virtual bool RemoveAt(int nIndex);
		virtual void RemoveAll();
		virtual void MoveItem(int nSrcIndex, int nDesIndex){}

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

	};

	class CBricksTabUI : public CContainerUI
	{
		typedef union INNERUI
		{
			CHorizontalLayoutUI* horizontal;
			CVerticalLayoutUI* vertical;
			CContainerUI* container;
		}InnerLayout;
	public:
		CBricksTabUI();
		~CBricksTabUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetItemAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void SetVerticalMode(bool bVertical);
		bool GetVerticalMode() const;
		void InvalidLayout();//手动设置模式后,需要重设Layout

		virtual bool Add(CControlUI* pControl);
		virtual bool AddAt(CControlUI* pControl, int nIndex);
		virtual bool Remove(CControlUI* pControl);
		virtual void RemoveAll();
		virtual int GetCurSel() const;
		virtual void SetPos(RECT rc);
		virtual bool SelectItem(int nIndex);
		virtual bool SelectItem(CControlUI* pControl);


		virtual void Init();
	protected:
		virtual bool OptionEvent(LPVOID pEvent_);

		InnerLayout m_layout;
		bool m_bVertical;//是否为垂直层叠
		int m_nCurSel;

	};

}

