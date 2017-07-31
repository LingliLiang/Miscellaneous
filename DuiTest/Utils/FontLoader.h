#pragma once

#include <vector>

class CFontLoader
{
public:
	CFontLoader();
	~CFontLoader();

	//程序位置的设置相当路径
	void SetFontFolder(CString folder);
	CString GetFontFolder();

	//从文件夹加载字体
	bool LoadFontsFromPath();

	//从资源加载字体
	bool LoadFontsFromResource(DWORD* idr_fons/*array*/, DWORD count);
private:
	void LoadFromFile(LPCTSTR lpFileName);
	
	//file buffer & size of byte
	//return: count of font installed
	DWORD AddFont(void* pvData, DWORD cbSize);

	std::vector<HANDLE> m_hFontLoaded;
	CString m_strFontFolder;
};