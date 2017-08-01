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

		//����λ�õ������൱·��
		void SetFontFolder(DirectUI::CUIString folder);
		DirectUI::CUIString GetFontFolder();

		//���ļ��м�������
		bool LoadFontsFromPath();

		//����Դ��������
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