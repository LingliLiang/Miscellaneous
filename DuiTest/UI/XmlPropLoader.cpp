#include "stdafx.h"
#include "XmlPropLoader.h"

CString _tagXmlProp::getAttrib(CString name)
{
	auto item = attrib.find(name);
	if (item != attrib.end()) {
		return item->second;
	}
	return CString();
}



CXmlPropLoader::CXmlPropLoader()
	:m_intoCount(0)
{
}


CXmlPropLoader::~CXmlPropLoader()
{
}

BOOL CXmlPropLoader::LoadXMLBuffer(LPTSTR & lpszXMLBuffer)
{
	if (lpszXMLBuffer)
	{
		m_XmlFile.SetDoc(lpszXMLBuffer);
		if(!m_XmlFile.IsWellFormed()) return 0;
		LoadAllProp();
		return 1;
	}
	return 0;
}

BOOL CXmlPropLoader::SaveXMLBuffer(LPTSTR & lpszXMLBuffer)
{
	return 0;
}

BOOL CXmlPropLoader::LoadXMLFile(LPTSTR lpszXmlFile)
{
	if (m_XmlFile.Load(lpszXmlFile))
	{
		if (!m_XmlFile.IsWellFormed()) return 0;
		LoadAllProp();
		return 1;
	}
	return 0;
}

BOOL CXmlPropLoader::SaveXMLFile(LPTSTR lpszXmlFile)
{
	return m_XmlFile.Load(lpszXmlFile) ? 1 : 0;
}

void CXmlPropLoader::SplitCString(CString strSource, LPCTSTR ch, std::vector<CString>& vecString)
{
	TCHAR *NextToken = NULL;
	vecString.clear();
	CString strTmp = _tcstok_s(strSource.GetBuffer(), ch, &NextToken);//(LPSTR)(LPCTSTR)将CString转char*
	if (strTmp.IsEmpty())
		return;
	vecString.push_back(strTmp);
	while (true)
	{
		strTmp = _tcstok_s(NULL, ch, &NextToken);
		if (strTmp == _T(""))
			break;
		strTmp.TrimLeft();
		vecString.push_back(strTmp);
	}
}

void CXmlPropLoader::LoadAllProp()
{
	m_XmlFile.ResetPos();//复位当前位置为文档的起始位置
	m_xmlCurNode.Empty();
	LoopFindElem();
	m_xmlCurNode.Empty();
}

inline bool CXmlPropLoader::FindElem()
{
	bool b = m_XmlFile.FindElem();
	if (b) { 
		m_xmlCurNode.Append(m_XmlFile.GetTagName());
	}
	return b;
}

inline bool CXmlPropLoader::IntoElem()
{
	bool b = m_XmlFile.IntoElem();
	if (b) {
		m_xmlCurNode.AppendChar(_T('/'));
		m_intoCount++;
	}
	return b;
}

inline bool CXmlPropLoader::OutOfElem()
{
	bool b = m_XmlFile.OutOfElem();
	if (b) {
		auto pos = m_xmlCurNode.ReverseFind(_T('/'));
		if (pos != -1)
		{
			m_xmlCurNode = m_xmlCurNode.Left(pos);
		}
		else
			m_xmlCurNode.Empty();
		m_intoCount--;
	}
	return b;
}

void CXmlPropLoader::GetElemProp()
{
	if (!m_xmlCurNode.IsEmpty())
	{
		_tagXmlProp prop;
		prop.data = m_XmlFile.GetData();
		prop.xmlFullNode = m_xmlCurNode;
		for (int n = 0;;++n)
		{
			CString strName, strValue;
			if (!m_XmlFile.GetNthAttrib(n, strName, strValue))
				break;
			prop.attrib[strName] = strValue;
		}
		SetXmlProp(m_xmlCurNode, prop);
	}
}

void CXmlPropLoader::LoopFindElem()
{
	static bool endCurElem = false;
	while (FindElem())
	{
		GetElemProp();
		bool bChild = m_XmlFile.FindChildElem();
		m_XmlFile.ResetChildPos();

		//if (bChild) {
		IntoElem();
		LoopFindElem();
		OutOfElem();
		//}
		if (!bChild || endCurElem)
		{
			if (endCurElem) endCurElem = false;
			//查找结束时删除节点
			auto pos = m_xmlCurNode.ReverseFind(_T('/'));
			if (pos != -1)
			{
				m_xmlCurNode = m_xmlCurNode.Left(pos + 1);
			}
			//无元素时继续查找
		}
	}
	endCurElem = true;
}

void CXmlPropLoader::GetXmlProp(CString key, std::vector<_tagXmlProp>& props)
{
	auto iter_begin = m_xmlProp.lower_bound(key);
	auto iter_end = m_xmlProp.upper_bound(key);
	props.clear();
	for (auto iter = iter_begin; iter != iter_end; ++iter)
	{
		props.push_back(iter->second);
	}
}

void CXmlPropLoader::SetXmlProp(CString key, _tagXmlProp & prop)
{
	m_xmlProp.insert(std::make_pair(key, prop));
}

