#ifndef _NSCG_STRINGSETS_H_
#define _NSCG_STRINGSETS_H_

using namespace std;

#include <tinyxml2.h>

#pragma warning(push)
#pragma warning(disable:4786)
#include <map>
#pragma warning(pop)

bool UTF_8To_ANSI_String(const char* pszSrc, string &strDes);
bool UTF_8To_UNICODE_String(const char* pszSrc, wstring &strDes);
bool ANSI_To_UNICODE_String(const char* pszSrc, wstring &strDes);
bool UNICODE_To_ANSI_String(const wchar_t* pszSrc, string &strDes);
void ANSI2CUIString(CUIString& output,const char* input);
/**
loading default strings pair from xml file.
xml example:
<Window>
	<Default name="string" value="string" />
</Window>
**/
class CCommonStrSets
{
public:
	CCommonStrSets(LPCTSTR pstrXmlFile);
	~CCommonStrSets(void);

	CUIString GetValue(LPCTSTR pstrName) const;
	bool GetFilter(LPCTSTR pstrName, TCHAR** ppBuffer) const;

	int GetCount() const;
	bool GetFailed() const;
	const char* GetError() const;
protected:
	bool InitStringSets();
private:
	CUIString m_xml;
	bool m_bFailed;
	string m_strError;
	std::map<CUIString, CUIString> m_mStrSets;
};

typedef CCommonStrSets CStrSets;


#endif //_NSCG_STRINGSETS_H_