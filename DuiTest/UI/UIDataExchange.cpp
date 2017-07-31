#include "stdafx.h"
#include "UIDataExchange.h"
#include <TCHAR.H>

CUIDataExchange::CUIDataExchange(CContainerUI* p)
	:m_pRoot(p), m_pPaintManager(*(p->GetManager()))
{
	InitControlUpdataFunc();
}

void CUIDataExchange::UDE_Control(CString strCtlName)
{
	CControlUI* p = m_pRoot->FindSubControl(strCtlName.GetBuffer());
	if (p)
		mapExData[strCtlName].pCtl = p;
}

CString CUIDataExchange::GetData(CControlUI* pCtl)
{
	return GetData(pCtl->GetName().GetData());
}

CString CUIDataExchange::GetData(CString strCtlName)
{
	auto value = mapExData.find(strCtlName);
	if (value != mapExData.end())
	{
		return value->second.cur;
	}
	return CString();
}

BOOL CUIDataExchange::SetData(CControlUI* pCtl, CString strData)
{
	return SetData(pCtl->GetName().GetData(), strData);
}

BOOL CUIDataExchange::SetData(CString strCtlName, CString strData)
{
	auto value = mapExData.find(strCtlName);
	if (value != mapExData.end())
	{
		value->second.cur = strData;
		return TRUE;

	}
	return FALSE;
}

CControlUI*  CUIDataExchange::GetControl(CString strCtlName)
{
	auto value = mapExData.find(strCtlName);
	if (value != mapExData.end())
	{
		return value->second.pCtl;
	}
	return nullptr;
}

void CUIDataExchange::InitControlUpdataFunc()
{
	mapFuncReg[CString(_T("OptionUI"))] = _tagFuncUpdate(
		std::bind(&CUIDataExchange::UpdateGetCheckbox, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&CUIDataExchange::UpdateSetCheckbox, this, std::placeholders::_1, std::placeholders::_2));

	mapFuncReg[CString(_T("CheckBoxUI"))] = mapFuncReg[CString(_T("OptionUI"))];

	mapFuncReg[CString(_T("EditUI"))] = _tagFuncUpdate(
		std::bind(&CUIDataExchange::UpdateGetEdit, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&CUIDataExchange::UpdateSetEdit, this, std::placeholders::_1, std::placeholders::_2));

	mapFuncReg[CString(_T("RichEditUI"))] = mapFuncReg[CString(_T("EditUI"))];
	mapFuncReg[CString(_T("ControlUI"))] = mapFuncReg[CString(_T("EditUI"))];

	mapFuncReg[CString(_T("ComboUI"))] = _tagFuncUpdate(
		std::bind(&CUIDataExchange::UpdateGetCombo, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&CUIDataExchange::UpdateSetCombo, this, std::placeholders::_1, std::placeholders::_2));
}

void CUIDataExchange::UpdateGetCombo(CControlUI* pCtrl, CString& cur)
{
	CComboUI *pcb = dynamic_cast<CComboUI*>(pCtrl);
	if (pcb) {
		CString str;
		int nSel = pcb->GetCurSel();
		CControlUI *pSel = pcb->GetItemAt(nSel);
		if (pSel) {
			cur = pSel->GetUserData().GetData();
		}
	}
}

void CUIDataExchange::UpdateSetCombo(CControlUI* pCtrl, CString& cur)
{
	CComboUI *pcb = dynamic_cast<CComboUI*>(pCtrl);
	if (pcb) {
		CString str;
		int nSel = -1;
		for (int i = 0; i < pcb->GetCount(); ++i)
		{
			CControlUI *pSel = pcb->GetItemAt(i);
			if (pSel) {
				str = pSel->GetUserData();
				if (str.CompareNoCase(cur) == 0) {
					nSel = i;
					break;
				}
			}
		}
		if (nSel != -1) {
			pcb->SelectItem(nSel);
		}
	}
}

void CUIDataExchange::UpdateGetEdit(CControlUI* pCtrl, CString& cur)
{
	if (pCtrl)
	{
		cur = pCtrl->GetText().GetData();
	}
}

void CUIDataExchange::UpdateSetEdit(CControlUI* pCtrl, CString& cur)
{
	if (pCtrl)
	{
		pCtrl->SetText(cur.GetBuffer());
	}
}

void CUIDataExchange::UpdateGetCheckbox(CControlUI* pCtrl, CString& cur)
{
	COptionUI *pCK=dynamic_cast<COptionUI*>(pCtrl);
	if (pCK) {
		pCK->IsSelected() ? cur = _T("1") : cur = _T("0");
	}
}

void CUIDataExchange::UpdateSetCheckbox(CControlUI* pCtrl, CString& cur)
{
	COptionUI *pCK = dynamic_cast<COptionUI*>(pCtrl);
	if (pCK) {
		BOOL bZero = cur.Compare(_T("0")) == 0;
		pCK->Selected(bZero ? false : true);
	}
}

BOOL CUIDataExchange::UpdateData(BOOL bGetFromUI)
{
	auto GetDataFromUI=[&]()->void
	{
		ExDataIter iter = mapExData.begin();
		//获取数据
		while (iter != mapExData.end())
		{
			if (iter->second.pCtl)
			{
				CString strClass = iter->second.pCtl->GetClass();
				auto func = mapFuncReg[strClass];
				func.get(iter->second.pCtl, iter->second.cur);
			}
			iter++;
		}
	};
	auto SetDataToUI = [&]()->void
	{
		ExDataIter iter = mapExData.begin();
		//设置数据
		while (iter != mapExData.end())
		{
			if (iter->second.pCtl)
			{
				CString strClass = iter->second.pCtl->GetClass();
				auto func = mapFuncReg[strClass];
				func.set(iter->second.pCtl, iter->second.cur_);
				iter->second.cur_.Empty();//清空缓存数据
			}
			iter++;
		}
	};
	auto Constraint = [&]()->void 
	{
		ExDataIter iter = mapExData.begin();
		while (iter != mapExData.end())
		{
			PassConstraint(bGetFromUI,iter);
			iter++;
		}
	};
	if (bGetFromUI)
	{
		GetDataFromUI();
		Constraint();
	}
	else
	{
		Constraint();
		SetDataToUI();
	}
	return true;
}

void CUIDataExchange::UDE_Constraint(CString strCtlName, ConstraintValue opt, LONG value)
{
	if(opt > END) return;
	mapExData[strCtlName].opt = opt;
	mapExData[strCtlName].value.vLong = value;
}

void CUIDataExchange::PassConstraint(BOOL bGetFromUI, ExDataIter& iter)
{
	//获取和设置UI数据不在这里操作,
	//而是使用UpdateSet/Getxxxx函数
	long long data  = 0;
	CString strText;
	if(bGetFromUI)
	{
		//UI to Memory
		data = _tcstoi64(iter->second.cur.GetBuffer(),0,10) ;
		if (iter->second.opt == NON)
		{ }
		else if(iter->second.opt & ADD){
			data -= iter->second.value.vLong;
			iter->second.cur.Format(_T("%I64d"),data);
		}
		else if(iter->second.opt & SUB){
			data += iter->second.value.vLong;
			iter->second.cur.Format(_T("%I64d"),data);
		}
		else if(iter->second.opt & MUL){
			data = iter->second.value.vLong;
			if(data != 0)
				data = _tcstoi64(iter->second.cur.GetBuffer(),0,10) / data;
			else
				data = _tcstoi64(iter->second.cur.GetBuffer(),0,10);
			iter->second.cur.Format(_T("%I64d"),data);
		}
		else if(iter->second.opt & DIV){
			data *= iter->second.value.vLong;
			iter->second.cur.Format(_T("%I64d"),data);
		}
		else if(iter->second.opt & ZFIXED){
			if(data == 0)
			{
				data = iter->second.value.vLong;
				iter->second.cur.Format(_T("%I64d"),data);
			}
		}
		else if(iter->second.opt & ILTFIXED){
			if(data < iter->second.value.vLong)
			{
				data = iter->second.value.vLong;
				iter->second.cur.Format(_T("%I64d"),data);
			}
		}
		else if(iter->second.opt & IGTFIXED){
			if(data >  iter->second.value.vLong)
			{
				data = iter->second.value.vLong;
				iter->second.cur.Format(_T("%I64d"),data);
			}
		}
		else if(iter->second.opt & FUNC || iter->second.opt & OFUNC){
			if(iter->second.pfun)
			{
				iter->second.pfun(bGetFromUI,iter->second.pCtl,iter->first,iter->second.cur);
			}
		}
	}
	else
	{
		//Memory to UI
		data = _tcstoul(iter->second.cur.GetBuffer(),0,10);
		if (iter->second.opt == NON)
		{
			strText = iter->second.cur;
		}
		else if(iter->second.opt & ADD){
			data += iter->second.value.vLong;
			strText.Format(_T("%I64d"),data);
		}
		else if(iter->second.opt & SUB){
			data -= iter->second.value.vLong;
			strText.Format(_T("%I64d"),data);
		}
		else if(iter->second.opt & MUL){
			data *= iter->second.value.vLong;
			strText.Format(_T("%I64d"),data);
		}
		else if(iter->second.opt & DIV){
			data = iter->second.value.vLong;
			if(data != 0)
				data = _tcstoi64(iter->second.cur.GetBuffer(),0,10) / data;
			else
				data = _tcstoi64(iter->second.cur.GetBuffer(),0,10);
			strText.Format(_T("%I64d"),data);
		}
		else if(iter->second.opt & ZFIXED){
			if(data == 0)
			{
				data = iter->second.value.vLong;
				iter->second.cur.Format(_T("%I64d"),data);
			}
			strText = iter->second.cur;
		}
		else if(iter->second.opt & ILTFIXED){
			if(data < iter->second.value.vLong)
			{
				data = iter->second.value.vLong;
				iter->second.cur.Format(_T("%I64d"),data);
			}
			strText = iter->second.cur;
		}
		else if(iter->second.opt & IGTFIXED){
			if(data >  iter->second.value.vLong)
			{
				data = iter->second.value.vLong;
				iter->second.cur.Format(_T("%I64d"),data);
			}
			strText = iter->second.cur;
		}
		else if(iter->second.opt & FUNC || iter->second.opt & OFUNC){
			if (iter->second.pfun)
			{
				strText = iter->second.cur;
				iter->second.pfun(bGetFromUI, iter->second.pCtl, iter->first, strText);
			}
		}
		iter->second.cur_ = strText;
	}
}

void CUIDataExchange::SetEnabled(BOOL bEnable)
{
	auto iter = mapExData.begin();
	while(iter != mapExData.end())
	{
		if(iter->second.pCtl)
			iter->second.pCtl->SetEnabled(bEnable?true:false);
		iter++;
	}
}

void CUIDataExchange::ClearText()
{
	ExDataIter iter = mapExData.begin();
	while (iter != mapExData.end())
	{
		CUIString className = iter->second.pCtl->GetClass();
		if (className == _T("EditUI") ||
			className == _T("RichEditUI") ||
			className == _T("ControlUI"))
		{
			iter->second.pCtl->SetText(_T(""));
		}
		iter++;
	}
}