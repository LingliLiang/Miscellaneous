#pragma once

#include <vector>
#include <gdiplus.h>
namespace DirectUI {

	class CFontLoader;


	class DirectUI_API CFontLoader
	{
	public:
		CFontLoader();
		~CFontLoader();

		//程序位置的设置相当路径
		void SetFontFolder(DirectUI::CUIString folder);
		DirectUI::CUIString GetFontFolder();

		//从文件夹加载字体
		bool LoadFontsFromPath();

		//从资源加载字体
		bool LoadFontsFromResource(DWORD* idr_fons/*array*/, DWORD count);

		//const LPBYTE GetFontMemCache(DWORD& cbSize);
		const Gdiplus::PrivateFontCollection * GetFontCollection();
	private:
		void LoadFromFile(LPCTSTR lpFileName);

		//file buffer & size of byte
		//return: count of font installed
		DWORD AddFont(void* pvData, DWORD cbSize);

		CStdPtrArray m_hFontLoaded;
		//CStdPtrArray m_lpByteFontCache;
		//CStdPtrArray m_dwFontSize;

		ULONG_PTR m_gdiplusToken;
		Gdiplus::PrivateFontCollection* m_pfc;
		DirectUI::CUIString m_strFontFolder;
	};

} // namespace DirectUI