#pragma once

#include <vector>

class CFontLoader
{
public:
	CFontLoader();
	~CFontLoader();

	//����λ�õ������൱·��
	void SetFontFolder(CString folder);
	CString GetFontFolder();

	//���ļ��м�������
	bool LoadFontsFromPath();

	//����Դ��������
	bool LoadFontsFromResource(DWORD* idr_fons/*array*/, DWORD count);
private:
	void LoadFromFile(LPCTSTR lpFileName);
	
	//file buffer & size of byte
	//return: count of font installed
	DWORD AddFont(void* pvData, DWORD cbSize);

	std::vector<HANDLE> m_hFontLoaded;
	CString m_strFontFolder;
};