#include "stdafx.h"
#include "FontLoader.h"
#include <Strsafe.h>
#include <cassert>

CFontLoader::CFontLoader()
	:m_strFontFolder(_T("\\skin\\default\\font"))
{
}

CFontLoader::~CFontLoader()
{
	for (auto iter = m_hFontLoaded.begin(); m_hFontLoaded.end() != iter;++iter)
	{
		if (*iter != NULL || *iter != INVALID_HANDLE_VALUE)
			RemoveFontMemResourceEx(*iter);
	}
}

void CFontLoader::SetFontFolder(CString folder)
{
	m_strFontFolder = folder;
}

CString CFontLoader::GetFontFolder()
{
	return m_strFontFolder;
}

bool CFontLoader::LoadFontsFromPath()
{
	TCHAR pszAppData[MAX_PATH] = { 0 };

	//default confige
	::GetModuleFileName(NULL, pszAppData, MAX_PATH);
	PathRemoveFileSpec(pszAppData);
	if (!m_strFontFolder.IsEmpty() && m_strFontFolder.GetAt(0) != _T('\\'))
	{
		m_strFontFolder.Insert(0, _T('\\'));
	}
	_tcscat_s(pszAppData, m_strFontFolder.GetBuffer());
	//StringCbCat(pszAppData,MAX_PATH,path);
	CString strpath = pszAppData;
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
					CString data(pszAppData);
					data += _T("\\");
					data += fileData.cFileName;
					LoadFromFile(data.GetBuffer());
				}
			} while (FindNextFile(hFind, &fileData));
		}
		::FindClose(hFind);
	}
	return m_hFontLoaded.size() != 0;
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
	return m_hFontLoaded.size() != 0;
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
		m_hFontLoaded.push_back(hFontLoad);
	}
	return nFontsInstalled;
}


