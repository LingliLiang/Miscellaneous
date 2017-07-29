#include "stdafx.h"
#include "UIXmlPropHelper.h"
#include "xmlfile.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////

CXmlPropDataHelper::CXmlPropDataHelper(CPaintManagerUI* p)
	:CUIDataExchange((CContainerUI*)p->GetRoot())
{}

BOOL CXmlPropDataHelper::LoadXMLBuffer(LPTSTR& lpszXMLBuffer)
{
	m_tempXml.Empty();
	m_tempXml = lpszXMLBuffer;
	if(lpszXMLBuffer == NULL) return FALSE;	
	CXmlFile xmlFile;
	xmlFile.SetDoc(lpszXMLBuffer);
	xmlFile.ResetPos();
	return LoadXml(xmlFile);
}

BOOL CXmlPropDataHelper::SaveXMLBuffer(LPTSTR& lpszXMLBuffer)
{
	CXmlFile xmlFile;
	if(lpszXMLBuffer != NULL) return FALSE;
	xmlFile.SetDoc(m_tempXml);
	xmlFile.ResetPos();
	if(!SaveXml(xmlFile)) return FALSE;
	xmlFile.ResetPos();
	m_tempXml = xmlFile.GetDoc();
	DWORD nSize = m_tempXml.GetLength()+1;
	lpszXMLBuffer = new TCHAR[nSize];
	if(!lpszXMLBuffer) return FALSE; // out of memory
	nSize = nSize*sizeof(TCHAR);
	::memset(lpszXMLBuffer,0,nSize);
	LPTSTR buffer = m_tempXml.LockBuffer();
	::memcpy_s(lpszXMLBuffer,nSize,buffer,nSize-sizeof(TCHAR));
	m_tempXml.UnlockBuffer();
	return TRUE;
}

BOOL CXmlPropDataHelper::LoadXMLFile(LPTSTR lpszXmlFile)
{
	if(!::PathFileExists(lpszXmlFile)) return FALSE;
	CXmlFile xmlFile;
	if(!xmlFile.Load(lpszXmlFile)) return FALSE;
	xmlFile.ResetPos();
	m_tempXml = xmlFile.GetDoc();
	return LoadXml(xmlFile);
}

BOOL CXmlPropDataHelper::SaveXMLFile(LPTSTR lpszXmlFile)
{
	CXmlFile xmlFile;
	if(::PathFileExists(lpszXmlFile))
	{
		if(!xmlFile.Load(lpszXmlFile)) return FALSE;
	}
	else
	{
		xmlFile.SetDoc(m_tempXml);
	}
	xmlFile.ResetPos();
	if(!SaveXml(xmlFile)) return FALSE;
	xmlFile.ResetPos();
	if(!xmlFile.SaveAndFormat(lpszXmlFile)) return FALSE;
	return TRUE;
}

BOOL CXmlPropDataHelper::LoadXml(CXmlFile& xmlFile)
{
	TCHAR split = _T('/');
	if(!InPropElem(&xmlFile,vecXmlMain,true)) return FALSE;
	auto GetPropData=[&](XmlPropIter& iter)->void
	{
		CString strElem;
		CString strNode;
		std::vector<CString> vecXmlNode;
		vecXmlNode.clear();
		if(iter->second.IsEmpty())
		{
			strElem = iter->first;
		}
		else if(iter->second.GetAt(0) == split)
		{
			strElem = iter->second;
			strElem.Remove(split);
		}
		else
		{
			strNode = iter->second;
			int nPos = 0;
			if(iter->second.GetAt(iter->second.GetLength()-1) == split)
			{
				strElem = iter->first;
			}
			else
			{
				nPos = iter->second.ReverseFind(split);
				if(nPos != -1)
				{
					strNode = iter->second.Left(nPos);
					strElem = iter->second.Right(iter->second.GetLength()-nPos-1);
				}
				nPos = 0;
			}

			while((nPos = strNode.Find(split)) != -1)
			{
				vecXmlNode.push_back(strNode.Left(nPos));
				strNode = strNode.Right(strNode.GetLength()-nPos-1);
			}
			strNode.IsEmpty()? 0: vecXmlNode.push_back(strNode);
		}
		//��ȡ����
		InPropElem(&xmlFile,vecXmlNode,true);
		if(!strElem.IsEmpty() && xmlFile.FindElem(strElem))
		{
			CUIDataExchange::SetData(iter->first,xmlFile.GetData());
		}
		OutPropElem(&xmlFile,vecXmlNode,true);
	};
	auto GetCollData=[&](XmlCollPropIter& iter)->void
	{
		ULONGLONG ullData = 0;
		CString strText,strElem;
		std::vector<CString> vecMainNode;
		CollectionInfo& info= iter->second;
		info.collValues.clear();
		int base = info.adapt_base;
		int count_of_data = 0;
		SplitCString(info.mainNode,&split,vecMainNode);
		//����mainNode
		if(!InPropElem(&xmlFile,vecMainNode,true)) return;
		do{
			if(info.b_attribute){
				strText = xmlFile.GetAttrib(info.countNode);
			}else{
				if(!xmlFile.FindElem(info.countNode))  break;
				strText = xmlFile.GetData();
			}
			ullData = _tcstoui64(strText.GetBuffer(),0,10);
			if(!ullData){
				break;
			}
			strElem.Format(info.adaptNode.GetBuffer(),base);
			while(xmlFile.FindElem(strElem.GetBuffer()))
			{
				if(info.collValues.size()<=count_of_data)
				{
					XmlCollValue collValue;
					info.collValues.push_back(collValue);
				}
				XmlCollValue& collValueRef = info.collValues[count_of_data];
				collValueRef.index = count_of_data;
				if(info.vecXmlNode.empty())
				{
					if(collValueRef.vecValues.empty())
						collValueRef.vecValues.push_back(xmlFile.GetData());
					else
						collValueRef.vecValues[0] = xmlFile.GetData();
				}
				else
				{
					xmlFile.IntoElem();
					std::vector<CString> vecXmlNode;
					std::vector<CString>::iterator node_iter = info.vecXmlNode.begin();
					if( info.vecXmlNode.size() != collValueRef.vecValues.size())
						collValueRef.vecValues.resize( info.vecXmlNode.size(),CString());
					std::vector<CString>::iterator value_iter = collValueRef.vecValues.begin();
					while(node_iter != info.vecXmlNode.end())
					{
						SplitCString(node_iter->GetBuffer(),&split,vecXmlNode);
						InPropElem(&xmlFile,vecXmlNode,false);
						*value_iter = xmlFile.GetData();
						OutPropElem(&xmlFile,vecXmlNode,false);
						value_iter++;
						node_iter++;
					}
					xmlFile.OutOfElem();
				}
				count_of_data++;
				base++;
				strElem.Format(info.adaptNode.GetBuffer(),base);
			}
		}while(false);
		OutPropElem(&xmlFile,vecMainNode,true);
	};
	//update data from xml prop
	XmlPropIter iter = mapXmlProp.begin();
	while(iter != mapXmlProp.end())
	{
		GetPropData(iter);
		iter++;
	}

	XmlCollPropIter coll_iter = mapXmlCollProp.begin();
	while(coll_iter != mapXmlCollProp.end())
	{
		GetCollData(coll_iter);
		coll_iter++;
	}
	//OutPropElem(&xmlFile,vecXmlMain,true);
	return TRUE;
}

BOOL CXmlPropDataHelper::SaveXml(CXmlFile& xmlFile)
{
	TCHAR split = _T('/');
	auto SetPropData=[&](XmlPropIter& iter)->void
	{
		CString strElem;
		CString strNode;
		std::vector<CString> vecXmlNode;
		vecXmlNode.clear();
		if(iter->second.IsEmpty())
		{
			strElem = iter->first;
		}
		else if(iter->second.GetAt(0) == split)
		{
			strElem = iter->second;
			strElem.Remove(split);
		}
		else
		{
			strNode = iter->second;
			int nPos = 0;
			if(iter->second.GetAt(iter->second.GetLength()-1) == split)
			{
				strElem = iter->first;
			}
			else
			{
				nPos = iter->second.ReverseFind(split);
				if(nPos != -1)
				{
					strNode = iter->second.Left(nPos);
					strElem = iter->second.Right(iter->second.GetLength()-nPos-1);
				}
				nPos = 0;
			}
			
			while((nPos = strNode.Find(split)) != -1)
			{
				vecXmlNode.push_back(strNode.Left(nPos));
				strNode = strNode.Right(strNode.GetLength()-nPos-1);
			}
			strNode.IsEmpty()? 0: vecXmlNode.push_back(strNode);
		}
		//��������
		InPropElem(&xmlFile,vecXmlNode,true);
		if(!strElem.IsEmpty() && xmlFile.FindElem(strElem))
			xmlFile.SetData(CUIDataExchange::GetData(iter->first));
		OutPropElem(&xmlFile,vecXmlNode,true);
	};
	auto SetCollData=[&](XmlCollPropIter& iter)->void
	{
		ULONGLONG ullData = 0;
		CString strElem;
		std::vector<CString> vecMainNode;
		CollectionInfo& info= iter->second;
		int base = info.adapt_base;
		SplitCString(info.mainNode,&split,vecMainNode);
		//����mainNode
		if(!InPropElem(&xmlFile,vecMainNode,true)) return;
		//�Ƴ�mainNode���и���
		xmlFile.OutOfElem();
		xmlFile.ResetMainPos();
		xmlFile.FindElem(vecMainNode.rbegin()->GetBuffer());
		xmlFile.RemoveNode();
		xmlFile.AddNode(1,vecMainNode.rbegin()->GetBuffer());
		xmlFile.IntoElem();
		ullData = info.collValues.size();
		//���ø�������
		if(info.b_attribute){
			xmlFile.SetAttrib(info.countNode,ullData);
		}else{
			xmlFile.AddElem(info.countNode,ullData);
		}	
		if(!ullData) return;
		
		for(ULONGLONG index = 0;index<ullData;index++)
		{
			XmlCollValue& collValueRef = info.collValues[index];
			strElem.Format(info.adaptNode.GetBuffer(),base++);
			xmlFile.AddElem(strElem.GetBuffer());
			if(info.vecXmlNode.empty())
			{
				xmlFile.SetData(collValueRef.vecValues[0].GetBuffer());
			}
			else
			{
				xmlFile.IntoElem();
				std::vector<CString> vecXmlNode;
				for(size_t index2 = 0;index2<info.vecXmlNode.size();index2++)
				{
					SplitCString(info.vecXmlNode[index2].GetBuffer(),&split,vecXmlNode);
					InAddPropElem(&xmlFile,vecXmlNode);
					xmlFile.SetData(collValueRef.vecValues[index2].GetBuffer());
					OutAddPropElem(&xmlFile,vecXmlNode);
				}
				xmlFile.OutOfElem();
			}
		}
		OutPropElem(&xmlFile,vecMainNode,true);
	};
	if(!InPropElem(&xmlFile,vecXmlMain,true)) return FALSE;
	//update data to xml prop
	XmlPropIter iter = mapXmlProp.begin();
	while(iter != mapXmlProp.end())
	{
		SetPropData(iter);
		iter++;
	}
	XmlCollPropIter coll_iter = mapXmlCollProp.begin();
	while(coll_iter != mapXmlCollProp.end())
	{
		SetCollData(coll_iter);
		coll_iter++;
	}
	return TRUE;
}

void CXmlPropDataHelper::UDE_Control(CControlUI* pCtl,CString xmlTreeNode)
{
	UDE_Control(pCtl->GetName().GetData(), xmlTreeNode);
}

void CXmlPropDataHelper::UDE_Control(CString strCtlName,CString xmlTreeNode)
{
	mapXmlProp[strCtlName] = xmlTreeNode;
	CUIDataExchange::UDE_Control(strCtlName);
}

void CXmlPropDataHelper::PushMainProp(CString xmlNode)
{
	vecXmlMain.push_back(xmlNode);
}

void CXmlPropDataHelper::UDE_Control_Coll(CollectionInfo& datas, UDECollectionCallBack* pCallBack, void* pFuncGet,  void* pFuncSet)
{
	assert(datas.ctls.size());
	if(pCallBack)
		datas.value.pCall = pCallBack;
	else
	{
		datas.value.pFunc[0] = (pFuncCollection)pFuncGet;
		datas.value.pFunc[1] = (pFuncCollection)pFuncSet;
	}
	mapXmlCollProp[datas.ctls[0]] = datas;
}

BOOL CXmlPropDataHelper::UpdateCollData(BOOL bGet)
{
	XmlCollPropIter iter = mapXmlCollProp.begin();
	auto CallFunc=[&](XmlCollValue& data)->void
	{
		if(bGet)
		{
			if(iter->second.value.pCall)
			{
				iter->second.value.pCall->CollectionGetData(iter->second.ctls,iter->second.ctls.size(),data);
			}
			else if(iter->second.value.pFunc[0])
			{
				(iter->second.value.pFunc[0])(iter->second.ctls,iter->second.ctls.size(),data);
			}
		}
		else
		{
			if(iter->second.value.pCall)
			{
				iter->second.value.pCall->CollectionSetData(iter->second.ctls,iter->second.ctls.size(),data);
			}
			else if(iter->second.value.pFunc[1])
			{
				(iter->second.value.pFunc[1])(iter->second.ctls,iter->second.ctls.size(),data);
			}
		}
	};
	while(iter != mapXmlCollProp.end())
	{
		CollectionInfo& info= iter->second;
		if(info.b_update){
			if(info.change_index >= info.collValues.size())
			{
#ifdef DEBUG
				DUI__Trace(_T("CXmlPropDataHelper:%s, index out of range"),iter->first->GetName().GetData());
#endif
				iter++;
				continue;
			}
			XmlCollValue& collValue = info.collValues[info.change_index];
			if(bGet)
			{
				//collValue.index = info.change_index;
				CallFunc(info.collValues[info.change_index]);//update Memory value
			}
			for(int index = 0; index < info.ctls.size();index++)
			{
				CControlUI*& pCtl = info.ctls[index];
				CUIString strClass = pCtl->GetClass();
				if(strClass == _T("CheckBoxUI") || strClass == _T("OptionUI"))
				{
					XmlUpdateCheckbox(bGet,pCtl,collValue.vecValues[index]);
				}
				else if(strClass == _T("EditUI") || strClass == _T("RichEditUI"))
				{
					XmlUpdateEdit(bGet,pCtl,collValue.vecValues[index]);
				}
				else if(strClass == _T("ComboUI"))
				{
					XmlUpdateCombo(bGet,pCtl,collValue.vecValues[index]);
				}
				else
				{
					XmlUpdateControl(bGet,pCtl,collValue.vecValues[index]);
				}
			}
			if(!bGet)
			{
				//collValue.index = info.change_index;
				CallFunc(info.collValues[info.change_index]);//update ui  value
			}
		}
		else //run all value
		{
			for(int index = 0; index < info.collValues.size();index++)
			{
				//info.collValues[index].index = index;
				CallFunc(info.collValues[index]);
			}
		}
		iter++;
	}
	return 1;
}

BOOL CXmlPropDataHelper::SetCollData(std::vector<CString>& values,CControlUI* pCtl,int index)
{
	assert(pCtl);
	size_t size = mapXmlCollProp[pCtl].collValues.size();
	if(size<index && index<0)
		return 0;
	if(mapXmlCollProp[pCtl].vecXmlNode.size() != values.size()) // ���ݸ���Ҫ���
		return 0;
	if(index == size) //add data
	{
		XmlCollValue collValue;
		collValue.index = index;
		collValue.vecValues = values;
		mapXmlCollProp[pCtl].collValues.push_back(collValue);
	}
	else
	{
		mapXmlCollProp[pCtl].collValues[index].vecValues = values;
	}
	return 1;
}

BOOL CXmlPropDataHelper::GetCollData(std::vector<CString>& values,CControlUI* pCtl,int index)
{
	assert(pCtl);
	size_t size = mapXmlCollProp[pCtl].collValues.size();
	if(size<=index && index<0)
		return 0;
	values.empty();
	values = mapXmlCollProp[pCtl].collValues[index].vecValues;
	return 1;
}

int CXmlPropDataHelper::GetCollSize(CControlUI* pCtl)
{
	assert(pCtl);
	return mapXmlCollProp[pCtl].collValues.size();
}

BOOL CXmlPropDataHelper::SetCollUpdate(CControlUI* pCtl,int index)
{
	assert(pCtl);
	mapXmlCollProp[pCtl].change_index = index;
	return 1;
}

void CXmlPropDataHelper::XmlUpdateCombo(bool bGetFromUI,CControlUI* pCtl, CString& value)
{
	CComboUI *pcb = dynamic_cast<CComboUI*>(pCtl);
	if (pcb) {
		CString str;
		if (bGetFromUI) 
		{
			int nSel=pcb->GetCurSel();
			CControlUI *pSel = pcb->GetItemAt(nSel);
			if (pSel) {
				value = pSel->GetUserData().GetData();
			}
		}
		else
		{
			int nSel=-1;
			for (int i =0; i < pcb->GetCount(); ++i)
			{
				CControlUI *pSel = pcb->GetItemAt(i);
				if (pSel) {
					str=pSel->GetUserData();
					if (str.CompareNoCase(value) == 0) {
						nSel=i;
						break;
					}
				}
			}
			if (nSel != -1) {
				pcb->SelectItem(nSel);
			}
		}
	}
}

void CXmlPropDataHelper::XmlUpdateEdit(bool bGetFromUI,CControlUI* pCtl, CString& value)
{
	if (pCtl)
	{
		if(bGetFromUI)
			value = pCtl->GetText();
		else
			pCtl->SetText(value.GetBuffer());
	}
}

void CXmlPropDataHelper::XmlUpdateCheckbox(bool bGetFromUI,CControlUI* pCtl, CString& value)
{
	CCheckBoxUI *pCK=dynamic_cast<CCheckBoxUI*>(pCtl);
	if (pCK) {
		BOOL bZero = value.Compare(_T("0")) == 0;
		if (bGetFromUI) {
			value.Empty();
			pCK->IsSelected()?value = _T("1") : value = _T("0");
		} else {
			pCK->Selected(bZero?false:true);
		}
	}
}

void CXmlPropDataHelper::XmlUpdateControl(bool bGetFromUI,CControlUI* pCtl, CString& value)
{
	if (pCtl)
	{
		if(bGetFromUI)
			value = pCtl->GetText();
		else
			pCtl->SetText(value.GetBuffer());
	}
}

void CXmlPropDataHelper::SplitCString(CString strSource, LPCTSTR ch, std::vector<CString>& vecString)
{
	TCHAR *NextToken = NULL;
	vecString.clear();
	CString strTmp=_tcstok_s(strSource.GetBuffer(),ch,&NextToken);//(LPSTR)(LPCTSTR)��CStringתchar*
	if(strTmp.IsEmpty())
		return;
	vecString.push_back(strTmp);
	while(true)
	{    
		strTmp=_tcstok_s(NULL,ch,&NextToken);
		if (strTmp==_T(""))				 
			break;
		strTmp.TrimLeft(); 
		vecString.push_back(strTmp);
	}
}

bool CXmlPropDataHelper::InPropElem(CXmlFile* xmlFile, std::vector<CString>& vecXmlNode, bool last_into)
{
	size_t size = vecXmlNode.size();
	for(size_t i = 0; i < size; i++)
	{
		if(xmlFile->FindElem(vecXmlNode[i]))
		{
			if(!last_into&&i==size-1)
				break;
			xmlFile->IntoElem();
		}
		else
			return false;
	}
	return true;
}

void CXmlPropDataHelper::OutPropElem(CXmlFile* xmlFile, std::vector<CString>& vecXmlNode, bool last_out)
{
	size_t size = vecXmlNode.size();
	for(size_t i = 0; i < size; i++)
	{
		if(!last_out&&i==size-1)
			break;
		xmlFile->OutOfElem();
	}
	xmlFile->ResetMainPos();
}

void CXmlPropDataHelper::InAddPropElem(CXmlFile* xmlFile, std::vector<CString>& vecXmlNode)
{
	size_t i = 0;
	for(i = 0; i < vecXmlNode.size()-1; i++)
	{
		while(true)
		{
			if(!xmlFile->FindElem()) //next exist - true 
			{
				xmlFile->AddElem(vecXmlNode[i]);
			}
			else
			{
				CString tname = xmlFile->GetTagName();
				if(vecXmlNode[i] != tname)
				{
					continue; //Keep looking
				}
			}
			xmlFile->IntoElem();
			break;
		}
	}
	xmlFile->AddElem(vecXmlNode[i]);
}

void CXmlPropDataHelper::OutAddPropElem(CXmlFile* xmlFile, std::vector<CString>& vecXmlNode)
{
	for(size_t i = 0; i < vecXmlNode.size()-1; i++)
	{
		xmlFile->OutOfElem();
	}
	xmlFile->ResetMainPos();
}
