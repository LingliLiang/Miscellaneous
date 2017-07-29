#pragma once

#include "XmlFile.h"
#include <vector>
#include <functional>
#include <map>

class _tagXmlProp
{
public:
	CString xmlFullNode;
	std::map<CString, CString> attrib;
	CString data;
	CString getAttrib(CString name);
};

class CXmlPropLoader
{
public:
	CXmlPropLoader();
	~CXmlPropLoader();

	BOOL LoadXMLBuffer(LPTSTR& lpszXMLBuffer);

	BOOL SaveXMLBuffer(LPTSTR& lpszXMLBuffer);

	BOOL LoadXMLFile(LPTSTR lpszXmlFile);

	BOOL SaveXMLFile(LPTSTR lpszXmlFile);

	void SplitCString(CString strSource, LPCTSTR ch, std::vector<CString>& vecString);

	bool SetXmlData(CString node_path, CString data);
	bool GetXmlData(CString node_path, CString& data);

	template<typename T>
	bool SetXmlAttrib(CString node_path, CString Attrib, T data);
	bool GetXmlAttrib(CString node_path, CString& data);

protected:
	void LoadAllProp();

	CString m_xmlCurNode;

	unsigned int m_intoCount;
	inline bool FindElem();
	inline bool IntoElem();
	inline bool OutOfElem();
	
	//获取数据
	void GetElemProp();
	//遍历整个xml
	void LoopFindElem();
	CXmlFile m_XmlFile;


	void GetXmlProp(CString key, std::vector<_tagXmlProp>& props);
	void SetXmlProp(CString key, _tagXmlProp& prop);
	std::multimap<CString, _tagXmlProp> m_xmlProp;
};

