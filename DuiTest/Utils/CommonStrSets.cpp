#include "stdafx.h"
#include "CommonStrSets.h"

bool UTF_8To_ANSI_String(const char* pszSrc, std::string &strDes)
{
	_ATLTRY
	{
		int nLen = MultiByteToWideChar(CP_UTF8, 0, pszSrc, strlen(pszSrc), NULL, 0);
		if (!nLen){
			return false;
		}

		WCHAR *lpszBuf = new WCHAR[nLen + 1];
		nLen = MultiByteToWideChar(CP_UTF8, 0, pszSrc, strlen(pszSrc), lpszBuf, nLen);
		if (!nLen){
			delete [] lpszBuf;
			return false;
		}
		lpszBuf[nLen] = L'\0';
		strDes = (const char*)CW2A(lpszBuf,CP_UTF8);
		delete [] lpszBuf;
	}
	_ATLCATCHALL()
	{
		return false;
	}
	return true;
}

bool UTF_8To_UNICODE_String(const char* pszSrc, std::wstring &strDes)
{
	_ATLTRY
	{
		int nLen = MultiByteToWideChar(CP_UTF8, 0, pszSrc, strlen(pszSrc), NULL, 0);
		if (!nLen){
			return false;
		}

		WCHAR *lpszBuf = new WCHAR[nLen + 1];
		nLen = MultiByteToWideChar(CP_UTF8, 0, pszSrc, strlen(pszSrc), lpszBuf, nLen);
		if (!nLen){
			delete [] lpszBuf;
			return false;
		}
		lpszBuf[nLen] = L'\0';
		strDes = lpszBuf;
		delete [] lpszBuf;
	}
	_ATLCATCHALL()
	{
		return false;
	}
	return true;
}

bool ANSI_To_UNICODE_String(const char* pszSrc, std::wstring &strDes)
{
	_ATLTRY
	{
		int nLen = MultiByteToWideChar(CP_ACP, 0, pszSrc, strlen(pszSrc), NULL, 0);
		if (!nLen){
			return false;
		}

		WCHAR *lpszBuf = new WCHAR[nLen + 1];
		nLen = MultiByteToWideChar(CP_ACP, 0, pszSrc, strlen(pszSrc), lpszBuf, nLen);
		if (!nLen){
			delete [] lpszBuf;
			return false;
		}
		lpszBuf[nLen] = L'\0';
		strDes = lpszBuf;
		delete [] lpszBuf;
	}
	_ATLCATCHALL()
	{
		return false;
	}
	return true;
}

bool UNICODE_To_ANSI_String(const wchar_t* pszSrc, std::string &strDes)
{
	_ATLTRY
	{
		int nLen = WideCharToMultiByte(CP_ACP, 0, pszSrc, _tcslen(pszSrc), NULL, 0,NULL,NULL);
		if (!nLen){
			return false;
		}

		CHAR *lpszBuf = new CHAR[nLen + 1];
		nLen = WideCharToMultiByte(CP_ACP, 0, pszSrc, _tcslen(pszSrc), lpszBuf, nLen,NULL,NULL);
		if (!nLen){
			delete [] lpszBuf;
			return false;
		}
		lpszBuf[nLen] = '\0';
		strDes = lpszBuf;
		delete [] lpszBuf;
	}
	_ATLCATCHALL()
	{
		return false;
	}
	return true;
}

void ANSI2CUIString(CUIString& output,const char* input)
{
	std::wstring str;
	//UTF_8To_ANSI_String(input,str);
	//output = CA2W(str.c_str());
	UTF_8To_UNICODE_String(input,str);
	output = str.c_str();
	//output.Format(_T("%s"),CA2W(str.c_str())); //error
	
}

CCommonStrSets::CCommonStrSets(LPCTSTR pstrXmlFile)
	:m_xml(pstrXmlFile),
	m_bFailed(false)
{
	m_bFailed = InitStringSets();
}

CCommonStrSets::~CCommonStrSets(void)
{
}

bool  CCommonStrSets::InitStringSets()
{
	tinyxml2::XMLDocument doc;
	std::string xml;
#ifdef _UNICODE
	UNICODE_To_ANSI_String(m_xml.GetData(),xml);
#else
	xml = m_xml.GetData();
#endif
	doc.LoadFile(xml.c_str());
	if(doc.Error())
	{
		m_strError.clear();
		m_strError.append(doc.ErrorName());
		return false;
	}
	tinyxml2::XMLElement* root = doc.FirstChildElement( "Window" );
	if(!root)
		return false;
	tinyxml2::XMLElement* element = root->FirstChildElement("Default");
	if(!element)
		return false;
	while (element)  
	{  
		element->FirstAttribute();
		const tinyxml2::XMLAttribute* attribute1 = const_cast<const tinyxml2::XMLElement*>(element)->FindAttribute("name");
		const tinyxml2::XMLAttribute* attribute2 = const_cast<const tinyxml2::XMLElement*>(element)->FindAttribute("value");
		if(attribute1 && attribute2)
		{

			CUIString name,value;
#ifdef _UNICODE
			ANSI2CUIString(name,attribute1->Value());
			ANSI2CUIString(value,attribute2->Value());
#else
			name.Format("%s",attribute1->Value());
			value.Format("%s",attribute2->Value());
#endif
			m_mStrSets[name] = value;
#ifdef _DEBUG
			//::OutputDebugString(name);
			//::OutputDebugStringA("  ");
			//::OutputDebugString(value);
			//::OutputDebugStringA("\n");
#endif
		}
		element = element->NextSiblingElement("Default");
	}
	
	doc.Clear();
	return true;
}

int CCommonStrSets::GetCount() const
{
	return m_mStrSets.size();
}

bool CCommonStrSets::GetFailed() const
{
	return m_bFailed;
}

const char* CCommonStrSets::GetError() const
{
	return m_strError.c_str();
}

CUIString CCommonStrSets::GetValue(LPCTSTR pstrName) const
{
	map<CUIString,CUIString>::const_iterator cit;
	CUIString name = pstrName;
	cit = m_mStrSets.find(name);
	if(cit == m_mStrSets.end())
	{
		return _T("");
	}

	return cit->second;
}

bool CCommonStrSets::GetFilter(LPCTSTR pstrName, TCHAR** ppBuffer) const
{
	CUIString strFilter = GetValue(pstrName);
	if( strFilter.IsEmpty() || strFilter.Find(_T("\\0")) == -1 )
		return false;

	*ppBuffer = new TCHAR [strFilter.GetLength() * sizeof(TCHAR)];
	ZeroMemory(*ppBuffer, strFilter.GetLength() * sizeof(TCHAR) );

	int nBufferPos = 0;
	int nPos = strFilter.Find(_T("\\0"));
	while( nPos != - 1)
	{
		_tcscat(*ppBuffer + nBufferPos, strFilter.Left(nPos));
		nBufferPos += nPos + 1;

		strFilter = strFilter.Right( strFilter.GetLength() - nPos - _tcslen(_T("\\0")));
		nPos = strFilter.Find(_T("\\0"));
	}

	
	return true;
}

