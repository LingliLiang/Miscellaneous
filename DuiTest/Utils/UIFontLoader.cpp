#include "stdafx.h"
#include "UIFontLoader.h"
#include <Strsafe.h>
#include <cassert>
#include <Shlwapi.h>


namespace DirectUI {

	CFontLoader::CFontLoader()
		:m_strFontFolder(_T("skin\\default\\font"))
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
		m_pfc = new Gdiplus::PrivateFontCollection;
	}

	CFontLoader::~CFontLoader()
	{
		delete m_pfc;
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
		for (auto index = 0; m_hFontLoaded.GetSize() > index;++index)
		{
			HANDLE h = m_hFontLoaded[index];
			if (h != NULL || h != INVALID_HANDLE_VALUE)
				RemoveFontMemResourceEx(h);
		}
	}

	void CFontLoader::SetFontFolder(DirectUI::CUIString folder)
	{
		m_strFontFolder = folder;
	}

	DirectUI::CUIString CFontLoader::GetFontFolder()
	{
		return m_strFontFolder;
	}

	bool CFontLoader::LoadFontsFromPath()
	{
		TCHAR pszAppData[MAX_PATH] = { 0 };

		//default confige
		::GetModuleFileName(NULL, pszAppData, MAX_PATH);
		::PathRemoveFileSpec(pszAppData);
		if (!m_strFontFolder.IsEmpty() && m_strFontFolder.GetAt(0) != _T('\\'))
		{
			DirectUI::CUIString slh(_T('\\'));
			m_strFontFolder = slh + m_strFontFolder;
		}
		_tcscat_s(pszAppData, m_strFontFolder.GetData());
		//StringCbCat(pszAppData,MAX_PATH,path);
		DirectUI::CUIString strpath = pszAppData;
		strpath += _T("\\*.*");
		{
			WIN32_FIND_DATA fileData;
			HANDLE hFind = FindFirstFile(strpath, &fileData);
			if (hFind != INVALID_HANDLE_VALUE)
			{
				do
				{
					if (_tcsicmp(fileData.cFileName, _T(".")) == 0 || _tcsicmp(fileData.cFileName, _T("..")) == 0)
					{
						continue;
					}
					else if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						continue;
					}
					else
					{
						DirectUI::CUIString data(pszAppData);
						data += _T("\\");
						data += fileData.cFileName;
						LoadFromFile(data.GetData());
					}
				} while (FindNextFile(hFind, &fileData));
			}
			::FindClose(hFind);
		}
		return m_hFontLoaded.GetSize() != 0;
	}

	bool CFontLoader::LoadFontsFromResource(DWORD * idr_fons, DWORD count)
	{
		if (!idr_fons || count == 0) return false;
		HMODULE hModule = ::GetModuleHandle(NULL);
		for (DWORD index = 0;index < count;++index)
		{
			HRSRC hResource = ::FindResource(hModule, MAKEINTRESOURCE(idr_fons[index]), RT_FONT);
			if (hResource == NULL)
				continue;

			DWORD dwSize = 0;
			HGLOBAL hGlobal = ::LoadResource(hModule, hResource);
			if (hGlobal == NULL)
			{
				::FreeResource(hResource);
				continue;
			}

			dwSize = ::SizeofResource(hModule, hResource);
			if (dwSize == 0)
				continue;

			AddFont((LPBYTE)::LockResource(hGlobal), dwSize);

			::FreeResource(hResource);
		}
		return m_hFontLoaded.GetSize() != 0;
	}

	void CFontLoader::LoadFromFile(LPCTSTR lpFileName)
	{
		HANDLE hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
		{
			DWORD cdSize = ::GetFileSize(hFile, 0);
			BYTE* pBuffer = new BYTE[cdSize];
			::ZeroMemory(pBuffer, cdSize);
			DWORD cdRead = 0;
			if (::ReadFile(hFile, pBuffer, cdSize, &cdRead, 0))
			{
				assert(cdSize == cdRead);
				AddFont(pBuffer, cdSize);
			}
			delete[] pBuffer;
			CloseHandle(hFile);
		}
	}

	DWORD CFontLoader::AddFont(void * pvData, DWORD cbSize)
	{
		HANDLE hFontLoad = NULL;
		DWORD nFontsInstalled = 0;
		hFontLoad = AddFontMemResourceEx(pvData, cbSize, 0, &nFontsInstalled);
		if (hFontLoad != NULL && hFontLoad != INVALID_HANDLE_VALUE)
		{
			m_pfc->AddMemoryFont(pvData, cbSize);
			m_hFontLoaded.Add(hFontLoad);
		}
		return nFontsInstalled;
	}


	//const LPBYTE CFontLoader::GetFontMemCache(DWORD& cbSize)
	//{
	//	return 0;
	//}

	const Gdiplus::PrivateFontCollection * CFontLoader::GetFontCollection()
	{
		return m_pfc;
	}


} // namespace DirectUI